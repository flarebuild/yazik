#pragma once

#include <yazik/concurrency/executor.hpp>

namespace yazik::concurrency {

    class Scheduler
    : public virtual Executor {

    public:

        virtual void schedule_delayed_impl(
            concurrency::unique_function<void()>&& clbk,
            std::chrono::nanoseconds delay
        ) = 0;

        virtual void schedule_periodic_impl(
            concurrency::unique_function<bool()>&& clbk,
            std::chrono::nanoseconds period
        ) = 0;

        class DelayedAwaitable {
            Scheduler* _scheduler = nullptr;
            const std::chrono::nanoseconds _delay;
        public:
            DelayedAwaitable(Scheduler* scheduler, std::chrono::nanoseconds delay)
            : _scheduler { scheduler }
            , _delay { delay }
            {}

            bool await_ready() const noexcept { return _scheduler == nullptr; }
            void await_suspend(std::experimental::coroutine_handle<> awaiter) noexcept {
                if (!_scheduler) {
                    awaiter.resume();
                    return;
                }
                _scheduler->schedule_delayed_impl(
                    [awaiter]() mutable {
                        awaiter.resume();
                    },
                    _delay
                );
            }
            void await_resume() const noexcept {}
        };

        template<typename REP, typename RATIO>
        DelayedAwaitable schedule_delayed(std::chrono::duration<REP, RATIO> delay) {
            return DelayedAwaitable {
                this,
                std::chrono::duration_cast<std::chrono::nanoseconds>(delay)
            };
        }

    };

    using scheduler_ptr_t = intrusive_ptr<Scheduler>;

    class ThreadScheduler
    : public virtual Scheduler
    , public virtual concurrency::ThreadExecutor
    {};

    class WorkScheduler
    : public virtual Scheduler
    , public virtual ThreadIdHolder {
    public:
        virtual void mark_thread() noexcept = 0;
        virtual Future<> run_until_done(Future<> work) = 0;
    };

    intrusive_ptr<WorkScheduler> create_asio_scheduler();
    intrusive_ptr<ThreadScheduler> create_asio_thread_scheduler();

    using work_scheduler_ptr_t = intrusive_ptr<WorkScheduler>;
    using thread_scheduler_ptr_t = intrusive_ptr<ThreadScheduler>;

} // end of ::yazik::concurrency namespace