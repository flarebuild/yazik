#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <optional>

#include <yazik/utility/lambda_continuation.hpp>
#include <yazik/utility/result.hpp>
#include <yazik/concurrency/stack.hpp>

namespace yazik::concurrency {

    class Event {
        std::mutex _wait_mutex;
        std::condition_variable _cv;
        std::atomic_bool _notified{false};

    public:
        Event();
        void wait();
        void wait_for(std::chrono::milliseconds ms);
        void set();
        void clear();
        bool is_set();
    };

    template <typename T>
    class MPSCResultEvent {
        Stack<Result<T>> _results_stack;
        std::mutex _wait_mutex;
        std::unique_lock<std::mutex> _wait_lock{
            _wait_mutex,
            std::defer_lock_t{}
        };
        std::condition_variable _wait_cv;

        enum class State {
            Waiting,
            InWork,
            Completed,
        };
        std::atomic<State> _state{State::Waiting};

        bool process_results(
            const std::function<Result<bool>(Result<T>)>& wait_clbk,
            std::optional<Result<void>>& final_result
        ) {
            return _results_stack.sweep_ordered([&](auto&& result) {
                auto clbk_result = wait_clbk(std::move(result));
                if (!clbk_result)
                    final_result = yaz_fail(clbk_result.error());
                else if (clbk_result.value() && !final_result)
                    final_result = yaz_ok();
            });
        }

    public:

        bool empty() {
            return _results_stack.empty();
        }

        Result<void> wait_multiple(
            std::function<void()> on_start_wait_clbk,
            std::function<Result<bool>(Result<T>)> wait_clbk
        ) {
            if (!_wait_lock.try_lock())
                return yaz_fail<string>("already waiting for event");
            yaz_defer { _wait_lock.unlock(); };

            std::optional<Result<void>> result;
            if (on_start_wait_clbk) on_start_wait_clbk();

            bool processed = false;
            unsigned wait_millisec = 10;

            while(!result) {
                if (processed)
                    wait_millisec = 10;
                else if (wait_millisec < 1'00)
                    wait_millisec *= 2;

                processed = false;
                _wait_cv.wait_for(_wait_lock, std::chrono::milliseconds(wait_millisec));
                _state = State::InWork;
                yaz_defer { _state = State::Waiting; };
                while (process_results(wait_clbk, result)) processed = true;
            }
            _state = State::Completed;
            return *result;
        }

        void set(Result<T>&& result) {
            if (_state == State::Completed) return;

            _results_stack.push(std::move(result));
            _wait_cv.notify_one();
        }

        void set_if_empty(Result<T>&& result) {
            if (_state == State::Completed) return;
            if (!_results_stack.push_if_empty(std::move(result))) return;
            _wait_cv.notify_one();
        }
    };

    // from https://github.com/lewissbaker/cppcoro/blob/master/include/cppcoro/async_manual_reset_event.hpp
    class CoroEventOp;
	class CoroEvent {
	public:
		CoroEvent(bool initially_set = false) noexcept;

		~CoroEvent();

		CoroEventOp operator co_await() const noexcept;

		bool is_set() const noexcept;

		void set() noexcept;

		void reset() noexcept;

	private:

		friend class CoroEventOp;

		// This variable has 3 states:
		// - this    - The state is 'set'.
		// - nullptr - The state is 'not set' with no waiters.
		// - other   - The state is 'not set'.
		//             Points to an 'async_manual_reset_event_operation' that is
		//             the head of a linked-list of waiters.
		mutable std::atomic<void*> _state;
	};

	class CoroEventOp {
	public:

		explicit CoroEventOp(const CoroEvent& event) noexcept;

		bool await_ready() const noexcept;
		bool await_suspend(std::experimental::coroutine_handle<> awaiter) noexcept;
		void await_resume() const noexcept {}

	private:

		friend class CoroEvent;

		const CoroEvent& _event;
		CoroEventOp* _next;
		std::experimental::coroutine_handle<> _awaiter;

	};

} // end of ::yazik::concurrency namespace