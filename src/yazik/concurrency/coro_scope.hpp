#pragma once

#include <stack>

#include <yazik/utility/lambda_continuation.hpp>
#include <yazik/concurrency/executor.hpp>
#include <yazik/concurrency/sequence_barrier.hpp>
#include <yazik/concurrency/task.hpp>

namespace yazik::concurrency {

    class CoroScope {
        SequenceBarrier<> _barrier;
        std::stack<OneWayTask> _deferred_stack;
        std::size_t _size = 0;
        std::atomic_size_t _cur_published {0};
    public:
        CoroScope() = default;

        template <typename Error>
        void push_deferred(Task<void, Error>&& task) {
            ++_size;
            _deferred_stack.push(
                [this, _l_move(task)] () -> OneWayTask {
                    co_await task.when_ready();
                    _barrier.publish(_cur_published.fetch_add(1));
                    co_return;
                }()
            );
        }

        Task<> join(concurrency::Executor* ex);
    };

    template <typename Error>
    void operator >> (Task<void, Error>&& f, CoroScope& scope) {
        scope.push_deferred(std::forward<Task<void, Error>>(f));
    }

    class CoroDefer {
        concurrency::executor_ptr_t _ex;
        using continuation_t = std::function<void()>;
        continuation_t _fn;

        Future<> defer();

    public:

        CoroDefer(CoroDefer&& other);
        CoroDefer(concurrency::executor_ptr_t ex);
        ~CoroDefer();

        CoroDefer apply(continuation_t&& continuation);
    };

} // end of ::yazik::concurrency namespace

#define yaz_coro_defer(ex) \
    auto YAZ_CONCAT(__CORO_DEFER_, __LINE__ ) \
        = ::yazik::utility::LambdaContinuationMark<::yazik::concurrency::CoroDefer>{ex} <<=