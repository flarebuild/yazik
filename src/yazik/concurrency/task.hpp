#pragma once

// from https://github.com/lewissbaker/cppcoro/blob/master/include/cppcoro/config.hpp

#include <folly/Function.h>

#include <yazik/utility/detail/promise_base.hpp>
#include <yazik/concurrency/future_result.hpp>

namespace yazik {

    template<typename T = void, typename Error = string> class Task;

namespace promises {

    enum class TaskPromiseState { Empty, Value, Exception };

    template<typename T, typename Error>
    class TaskPromiseBase
    : public ErrorPropagationPromise<Error> {
        PromiseContinuationHolder<Error> _continuation;

        struct FinalAwaitable {
            bool await_ready() const noexcept { return false; }

            template<typename Promise>
            std::experimental::coroutine_handle<> await_suspend(
                std::experimental::coroutine_handle<Promise> coro
            ) noexcept {
                return coro.promise().continuation().continuation_handle();
            }

            void await_resume() noexcept {
                $breakpoint_hint
            }
        };

        friend struct FinalAwaitable;

    protected:

        std::optional<Result<T, Error>> _result;
        TaskPromiseBase() = default;
    public:

        ~TaskPromiseBase() = default;

        auto initial_suspend() noexcept {
            return std::experimental::suspend_always{};
        }

        auto final_suspend() noexcept {
            return FinalAwaitable{};
        }

        template <typename Promise>
        void set_continuation(
            std::experimental::coroutine_handle<> self_h,
            std::experimental::coroutine_handle<Promise> handle
        ) noexcept {
            _continuation.template set_continuation<Promise>(self_h, handle);
        }

        PromiseContinuationHolder<Error>& continuation() {
            return _continuation;
        }

        void unhandled_exception() noexcept {
            auto handle = propagate_error_impl(
                detail::current_exception_to_error<Error>(),
                _continuation.self_handle()
            );
            if (handle && _continuation.can_propagate() && !_continuation.need_rethrow())
                _continuation.change_continuation_handle(handle);
        }

        template<
            typename UError,
            typename = std::enable_if_t<std::is_convertible_v<UError&&, Error>>
        > void set_error(UError&& e) noexcept(std::is_nothrow_constructible_v<Error, UError&&>) {
            _result = yaz_fail<Error>(std::forward<UError>(e));
        }

//        bool need_rethrow() override {
//            return !_continuation.can_propagate();
//        }

        std::experimental::coroutine_handle<>
        propagate_error_impl(Error&& error, std::experimental::coroutine_handle<>& self_h) override {
            std::experimental::coroutine_handle<> handle;
            if (_continuation.can_propagate()) {
                handle =  _continuation.propagate_error(error);
            } else {
                handle = _continuation.continuation_handle();
            }

            if (_continuation.need_rethrow()) {
                set_error(std::forward<Error>(error));
            }

            return handle;
        }

        std::optional<Result<T, Error>>& result() {
            return _result;
        }

    };

    template<typename Error>
    class TaskPromise<void, Error> final
    : public TaskPromiseBase<void, Error> {
        using Base = TaskPromiseBase<void, Error>;
    public:
        void return_void() noexcept {
            this->_result = yaz_ok<Error>();
        }
        Task<void, Error> get_return_object() noexcept;
    };

    template<typename T, typename Error>
    class TaskPromise final
    : public TaskPromiseBase<T, Error> {
        using Base = TaskPromiseBase<T, Error>;
    public:
        template<typename Value>
        requires std::is_convertible_v<Value&&, T> || std::is_constructible_v<T, Value>
        void return_value(Value&& value) noexcept(std::is_nothrow_constructible_v<T, Value&&>) {
            this->_result = std::forward<Value>(value);
        }

        Task<T, Error> get_return_object() noexcept;
    };

} // end of promises namespace

    struct OneWayTask {
        struct promise_type {
            std::experimental::suspend_never initial_suspend() noexcept { return {}; }
            std::experimental::suspend_never final_suspend() noexcept { return {}; }
            void unhandled_exception() noexcept {
                ::yazik::L::warn(
                    "❗❗❗❗❗ unhandled_exception: {} ❗❗❗❗",
                    ::yazik::detail::current_exception_to_error<string>()
                );
            }
            OneWayTask get_return_object() noexcept { return {}; }
            void return_void() noexcept {}
        };
    };

    template<typename T, typename Error>
    class TaskAwaitableBase {
        using coroutine_handle_t = std::experimental::coroutine_handle<
            promises::TaskPromise<T, Error>
        >;
    protected:
        coroutine_handle_t _coroutine;
    public:
        TaskAwaitableBase(coroutine_handle_t coroutine) noexcept
        : _coroutine { coroutine }
        {}

        bool await_ready() const noexcept {
            return !_coroutine || _coroutine.done();
        }
    };

    template<typename T, typename Error>
    struct ErrorPropagationAwaitable: TaskAwaitableBase<T, Error> {
        using TaskAwaitableBase<T, Error>::TaskAwaitableBase;

        template <typename Promise>
        std::experimental::coroutine_handle<> await_suspend(
            std::experimental::coroutine_handle<Promise> awaiting_coroutine
        ) noexcept {
            this->_coroutine.promise().template set_continuation<Promise>(
                (std::experimental::coroutine_handle<>)this->_coroutine,
                awaiting_coroutine
            );
            return this->_coroutine;
        }
    };

    template<typename T, typename Error>
    struct NoErrorPropagationAwaitable: TaskAwaitableBase<T, Error> {
        using TaskAwaitableBase<T, Error>::TaskAwaitableBase;

        std::experimental::coroutine_handle<> await_suspend(
            std::experimental::coroutine_handle<> awaiting_coroutine
        ) noexcept {
            this->_coroutine.promise().template set_continuation<void>(
                (std::experimental::coroutine_handle<>)this->_coroutine,
                awaiting_coroutine
            );
            return this->_coroutine;
        }
    };

	/// \brief
	/// A task represents an operation that produces a result both lazily
	/// and asynchronously.
	///
	/// When you call a coroutine that returns a task, the coroutine
	/// simply captures any passed parameters and returns exeuction to the
	/// caller. Execution of the coroutine body does not start until the
	/// coroutine is first co_await'ed.
	template<typename T, typename Error>
	class [[nodiscard]] Task {
	public:

		using promise_type = promises::TaskPromise<T, Error>;
		using value_type = T;

	private:
        std::experimental::coroutine_handle<promise_type> _coroutine;

	public:

		Task() noexcept
        : _coroutine { nullptr }
		{}

		explicit Task(std::experimental::coroutine_handle<promise_type> coroutine)
        : _coroutine { coroutine }
		{}

		Task(Task&& t) noexcept
        : _coroutine(t._coroutine) {
			t._coroutine = nullptr;
		}

		/// Disable copy construction/assignment.
		Task(const Task&) = delete;
		Task& operator=(const Task&) = delete;

		/// Frees resources used by this task.
		~Task() {
			if (_coroutine) {
				_coroutine.destroy();
			}
		}

		Task& operator=(Task&& other) noexcept {
			if (std::addressof(other) != this) {
				if (_coroutine) {
					_coroutine.destroy();
				}

				_coroutine = other._coroutine;
				other._coroutine = nullptr;
			}

			return *this;
		}

		/// \brief
		/// Query if the task result is complete.
		///
		/// Awaiting a task that is ready is guaranteed not to block/suspend.
		bool is_ready() const noexcept {
			return !_coroutine || _coroutine.done()
			    || _coroutine.promise().promise_state() != promises::TaskPromiseState::Empty;
		}

		auto operator co_await() const & noexcept {
			struct Awaitable: ErrorPropagationAwaitable<T, Error> {
				using ErrorPropagationAwaitable<T, Error>::ErrorPropagationAwaitable;

				T await_resume() {
					if (!this->_coroutine) {
						throw promises::broken_promise{};
					}
					auto& result = this->_coroutine.promise().result();
                    if (result && result->hasError()) {
                        throw std::move(result->error());
                    }

                    if constexpr (std::is_same_v<T, void>)
                        return;
                    else
                        return std::move(result->value());
				}
			};

			return Awaitable { _coroutine };
		}

		auto operator co_await() const && noexcept {
			struct Awaitable: ErrorPropagationAwaitable<T, Error> {
				using ErrorPropagationAwaitable<T, Error>::ErrorPropagationAwaitable;

				T await_resume() {
					if (!this->_coroutine)
						throw promises::broken_promise{};

					auto& result = this->_coroutine.promise().result();
                    if (result && result->hasError()) {
                        throw std::move(result->error());
                    }

                    if constexpr (std::is_same_v<T, void>)
                        return;
                    else
                        return std::move(result->value());
				}
			};

			return Awaitable { _coroutine };
		}

		/// \brief
		/// Returns an awaitable that will await completion of the task without
		/// attempting to retrieve the result.
		auto when_ready() const noexcept {
		    struct Awaitable: NoErrorPropagationAwaitable<T, Error> {
		        using NoErrorPropagationAwaitable<T, Error>::NoErrorPropagationAwaitable;
		        void await_resume() const noexcept {}
		    };
			return Awaitable { _coroutine };
		}

		auto wrapped() const noexcept {
		    struct Awaitable: NoErrorPropagationAwaitable<T, Error> {
                using NoErrorPropagationAwaitable<T, Error>::NoErrorPropagationAwaitable;

                Result<T, Error> await_resume() {
                    if (!this->_coroutine) {
                        throw promises::broken_promise{};
                    }
                    auto& result = this->_coroutine.promise().result();
                    if (!result)
                        return {};
                    return *result;
                }
		    };
		    return Awaitable { _coroutine };
		}

		Future<T, Error> fut() {
		    Task self_move = std::move(*this);
		    if constexpr (std::is_same_v<T, void>) {
		        co_await self_move;
		        co_return;
		    } else {
		        co_return co_await self_move;
		    }
		}

		OneWayTask one_way() {
		    Task self_move = std::move(*this);
		    co_await self_move.when_ready();
            co_return;
		}
	};

    template <typename Error = string>
    Task<void, Error> yaz_ok_t() noexcept {
        co_return;
    }

    template <typename T, typename Error = string>
    Task<T, Error> yaz_ok_t(T&& value) noexcept {
        co_return std::forward<T>(value);
    }

    template <typename T = void, typename Error = string>
    Task<T, Error> yaz_fail_t(Error e) noexcept {
        co_await yaz_fail(std::move(e));
        __builtin_unreachable();
    }


namespace promises {
    template<typename T, typename Error>
    Task<T, Error> TaskPromise<T, Error>::get_return_object() noexcept {
        return Task<T, Error>{ std::experimental::coroutine_handle<TaskPromise>::from_promise(*this) };
    }

    template<typename Error>
    Task<void, Error> TaskPromise<void, Error>::get_return_object() noexcept {
        return Task<void, Error>{ std::experimental::coroutine_handle<TaskPromise>::from_promise(*this) };
    }
} // end of promises namespace

    template <typename T, typename Error = string>
    using TaskIPtr = Task<intrusive_ptr<T>, Error>;

} // end of ::yazik namespace