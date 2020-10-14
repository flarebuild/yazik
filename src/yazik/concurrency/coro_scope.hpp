#pragma once

#include <stack>

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

} // end of ::yazik::concurrency namespace