#pragma once

#include <grpcpp/grpcpp.h>
#include <grpcpp/alarm.h>
#include <yazik/utility/result.hpp>
#include <yazik/concurrency/scheduler.hpp>

namespace yazik::rpc::grpc {

    class GrpcQueueScheduler;

    class IGrpcQueueAction {
        bool _is_ok = false;
    public:
        virtual void proceed(const std::shared_ptr<std::atomic_bool>& is_cancelled) = 0;
        void set_is_ok(bool ok);
        bool is_ok() const;
        void* self_tagged();
        virtual ~IGrpcQueueAction() {}
    };

    class GrpcQueueAction
    : public IGrpcQueueAction {
        concurrency::DispatchAction _action;
        ::grpc::Alarm _alarm;
    public:
        GrpcQueueAction(concurrency::DispatchAction&& action);
        void proceed(const std::shared_ptr<std::atomic_bool>& is_cancelled) override;
        void dispatch(::grpc::CompletionQueue* queue);
    };

    class GrpcDelayedAction
    : public IGrpcQueueAction {
        unique_function<void()> _clbk;
        ::grpc::Alarm _alarm;
    public:
        GrpcDelayedAction(unique_function<void()>&& clbk);
        void proceed(const std::shared_ptr<std::atomic_bool>& is_cancelled) override;
        void dispatch(::grpc::CompletionQueue* queue, std::chrono::nanoseconds delay);
    };

    class GrpcScheduledAction
    : public IGrpcQueueAction {
        unique_function<bool()> _clbk;
        std::chrono::nanoseconds _period;
        ::grpc::CompletionQueue* _queue;
        ::grpc::Alarm _alarm;
    public:
        GrpcScheduledAction(
            unique_function<bool()>&& clbk,
            std::chrono::nanoseconds period,
            ::grpc::CompletionQueue* queue
        );
        void proceed(const std::shared_ptr<std::atomic_bool>& is_cancelled) override;
        void dispatch();
    };

    class GrpcQueueTag
    : public IGrpcQueueAction {
        friend class GrpcQueueScheduler;

        mutable GrpcQueueScheduler* _scheduler = nullptr;
        mutable std::experimental::coroutine_handle<> _handle;

        void proceed(const std::shared_ptr<std::atomic_bool>& is_cancelled) override;

    $yaz_debug(
        const char* _descr = nullptr;
    )

    public:

        class GrpcQueueTagOp;
        friend class GrpcQueueTagOp;
        class GrpcQueueTagOp {
            friend class GrpcQueueTag;
            const GrpcQueueTag* _tag;
            GrpcQueueTagOp(const GrpcQueueTag* tag);

        public:
            bool await_ready() const noexcept;
            bool await_resume();
            void await_suspend(std::experimental::coroutine_handle<> handle);
        };

        GrpcQueueTag($yaz_debug(const char* descr));

        GrpcQueueTagOp operator co_await() const noexcept;

    $yaz_debug(
        inline void set_descr(const char* descr) {
            _descr = descr;
        }

        inline const char* descr() {
            return _descr;
        }
    )
    };

    class GrpcQueueScheduler
    : public virtual concurrency::Scheduler
    , protected concurrency::Waiter {
        std::unique_ptr<::grpc::CompletionQueue> _queue;
        concurrency::Stack<concurrency::unique_function<void()>> _deferred_stack;
    protected:
        std::shared_ptr<std::atomic_bool> _need_cancel = std::make_shared<std::atomic_bool>(false);


        Result<bool> check_status(::grpc::CompletionQueue::NextStatus sts);
        Result<IGrpcQueueAction*> wait_for_action();
        void poll_all_deferred();
        bool step();
    public:
        GrpcQueueScheduler(std::unique_ptr<::grpc::CompletionQueue> queue);
        virtual ~GrpcQueueScheduler();
        static utility::intrusive_ptr<GrpcQueueScheduler> create();

        concurrency::Disposer dispatch_impl(
            concurrency::unique_function<void(concurrency::CancellationToken&&)>&& clbk
            $yaz_debug(, const char* descr)
        ) override;

        void dispatch_impl(
            concurrency::unique_function<void()>&& clbk
            $yaz_debug(, const char* descr)
        ) override;

        void defer_impl(
            concurrency::unique_function<void()>&& clbk
            $yaz_debug(, const char* descr)
        ) override;

        void schedule_delayed_impl(
            concurrency::unique_function<void()>&& clbk,
            std::chrono::nanoseconds delay
        ) override;

        void schedule_periodic_impl(
            concurrency::unique_function<bool()>&& clbk,
            std::chrono::nanoseconds period
        ) override;

        GrpcQueueTag::GrpcQueueTagOp wait_for_tag(GrpcQueueTag& tag);
        bool check_need_stop();
        const std::shared_ptr<std::atomic_bool>& need_cancel();

        ::grpc::CompletionQueue* queue();
        void stop_impl();
        Future<> run_work(std::function<Future<>()>&& work);
    };

    class GrpcQueueThreadScheduler
    : public GrpcQueueScheduler
    , public concurrency::ThreadScheduler {
        std::shared_ptr<std::thread> _thread;
        std::optional<uint64_t> _thread_id;
        Promise<> _on_stop;

        void thread_loop();
    public:
        using GrpcQueueScheduler::GrpcQueueScheduler;

        static utility::intrusive_ptr<GrpcQueueThreadScheduler> create();

        virtual ~GrpcQueueThreadScheduler();

        const std::optional<uint64_t>& thread_id() const override;

        Result<void> start() override;
        void stop() override;
        void wait() override;

        Future<> on_stop();
    };

    class GrpcServerQueueThreadScheduler
    : public GrpcQueueThreadScheduler {
    public:
        GrpcServerQueueThreadScheduler(std::unique_ptr<::grpc::ServerCompletionQueue> queue);
        ::grpc::ServerCompletionQueue* server_queue();
    };

    using queue_scheduler_ptr_t = utility::intrusive_ptr<GrpcQueueScheduler>;
    using queue_thread_scheduler_ptr_t = utility::intrusive_ptr<GrpcQueueThreadScheduler>;
    using server_queue_thread_scheduler_ptr_t = utility::intrusive_ptr<GrpcServerQueueThreadScheduler>;

//    template <typename Tag>
//    using GrpcExecutorProvider =  ExecutorProvider<Tag, GrpcServerQueueThreadExecutor>;
//    template <typename Tag>
//    using GrpcExecutorPoolProvider =  ExecutorPoolProvider<Tag, GrpcServerQueueThreadExecutor>;

} // end of ::yazik::rpc::grpc namespace
