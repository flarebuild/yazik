#include <boost/asio.hpp>

#include "scheduler.hpp"

namespace yazik::concurrency {

    class AsioScheduler
    : public WorkScheduler
    , protected concurrency::Waiter {
    protected:
        ::boost::asio::io_service _io;
        cancel_token_ptr _need_cancel;
        std::optional<uint64_t> _thread_id;
    public:

        Disposer dispatch_impl(
            concurrency::unique_function<void(CancellationToken&&)>&& clbk
            $yaz_debug(, const char* descr)
        ) override {
            auto [ action, disposer ] = DispatchAction::managed(std::move(clbk));
            dispatch_impl(
                [ this, _l_move(action) ]() mutable {
                    action.execute(_need_cancel);
                }
                $yaz_debug(, descr)
            );
            return disposer;
        }

        void dispatch_impl(
            concurrency::unique_function<void()>&& clbk
            $yaz_debug(, const char* descr)
        ) override {
            ::boost::asio::post(_io.get_executor(), std::move(clbk));
        }

        void defer_impl(
            concurrency::unique_function<void()>&& clbk
            $yaz_debug(, const char* descr)
        ) override {
            ::boost::asio::defer(_io.get_executor(), std::move(clbk));
        }

        void schedule_delayed_impl(
            concurrency::unique_function<void()>&& clbk,
            std::chrono::nanoseconds delay
        ) override {
            auto timer = std::make_shared<::boost::asio::steady_timer>(_io);
            timer->expires_from_now(delay);
            timer->async_wait(
                [_l_move(clbk), timer]
                (const ::boost::system::error_code&) mutable
                { clbk(); }
            );
        }

        struct PeriodicTimerControl {
            ::boost::asio::steady_timer timer;
            concurrency::unique_function<bool()> clbk;
            std::chrono::nanoseconds period;
            std::function<void(const boost::system::error_code&)> tick;

            PeriodicTimerControl(
                ::boost::asio::io_service& io,
                concurrency::unique_function<bool()>&& clbk,
                std::chrono::nanoseconds period
            )
            : timer { io }
            , clbk { std::move(clbk) }
            , period { period }
            {
                timer.expires_from_now(period);
            }
        };

        void schedule_periodic_impl(
            concurrency::unique_function<bool()>&& clbk,
            std::chrono::nanoseconds period
        ) override {
            auto control = std::make_shared<PeriodicTimerControl>(
                _io,
                std::move(clbk),
                period
            );
            control->tick = [control](const boost::system::error_code&) mutable {
                if (!control->clbk()) {
                    control->tick = nullptr;
                    return;
                }
                control->timer.expires_at(control->timer.expires_at() + control->period);
                control->timer.async_wait(control->tick);
            };
            control->timer.expires_from_now(period);
            control->timer.async_wait(control->tick);
        }

        void mark_thread() noexcept override {
            _thread_id = thread_idx();
        }

        Future<> run_until_done(Future<> work) override {
            mark_thread();
            _need_cancel = new CancellationTokenAtomic;
            auto io_work = ::boost::asio::io_service::work { _io };
            do {
                _io.run_for(std::chrono::milliseconds{100});
            } while (!work.is_ready());
            return work;
        }

        const std::optional<uint64_t>& thread_id() const override {
            return _thread_id;
        }

        bool is_on() noexcept override {
            return thread_id() == thread_idx();
        }
    };

    class AsioThreadScheduler
    : public AsioScheduler
    , public ThreadScheduler {
        std::shared_ptr<std::thread> _thread;

        void thread_loop() {
            _need_cancel = new CancellationTokenAtomic;
            _thread_id = thread_idx();
            auto io_work = ::boost::asio::io_service::work { _io };
            do {
                _io.run_for(std::chrono::milliseconds{100});
            } while (!_need_cancel->is_cancelled());
            $breakpoint_hint
        }

    public:
        ~AsioThreadScheduler() {
            stop();
            wait();
        }

        Result<void> start() override {
            if (_thread && _thread->joinable())
                return yaz_fail<string>("already started");
            _thread = std::make_shared<std::thread>(std::bind(&AsioThreadScheduler::thread_loop, this));
            return yaz_ok();
        }

        void stop() override {
            if (_need_cancel)
                _need_cancel->cancel();
            $breakpoint_hint
        }

        void wait() override {
            if (thread_id() == thread_idx())
                return;
            if (_thread && _thread->joinable())
                _thread->join();
            _thread.reset();
            _thread_id = {};
        }
    };

    intrusive_ptr<WorkScheduler> create_asio_scheduler() {
        return new AsioScheduler{};
    }

    intrusive_ptr<ThreadScheduler> create_asio_thread_scheduler() {
        return new AsioThreadScheduler{};
    }

} // end of ::yazik::concurrency namespace