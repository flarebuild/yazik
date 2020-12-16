#include "grpc_scheduler.hpp"

#include <grpcpp/alarm.h>
#include <grpc++/completion_queue.h>

#include <yazik/utility/lambda_continuation.hpp>
#include <yazik/utility/utility_defs.hpp>

//#ifndef NDEBUG
//void grpc_cq_internal_ref(grpc_completion_queue* cc, const char* reason,
//                          const char* file, int line);
//void grpc_cq_internal_unref(grpc_completion_queue* cc, const char* reason,
//                            const char* file, int line);
//#define GRPC_CQ_INTERNAL_REF(cc, reason) \
//  grpc_cq_internal_ref(cc, reason, __FILE__, __LINE__)
//#define GRPC_CQ_INTERNAL_UNREF(cc, reason) \
//  grpc_cq_internal_unref(cc, reason, __FILE__, __LINE__)
//#else
//void grpc_cq_internal_ref(grpc_completion_queue* cc);
//void grpc_cq_internal_unref(grpc_completion_queue* cc);
//#define GRPC_CQ_INTERNAL_REF(cc, reason) grpc_cq_internal_ref(cc)
//#define GRPC_CQ_INTERNAL_UNREF(cc, reason) grpc_cq_internal_unref(cc)
//#endif

namespace {

#ifdef YA_DEBUG
    template <typename T>
    inline void* with_mark(T* ptr) {
        return reinterpret_cast<void*>(uintptr_t(ptr) | uintptr_t(1));
    }
    template <typename T>
    inline  T* clear_mark(void* ptr) {
        return reinterpret_cast<T*>(uintptr_t(ptr) & ~uintptr_t(1));
    }

    inline bool is_marked(void* ptr) {
        return uintptr_t(ptr) & uintptr_t(1);
    }
#endif

}

namespace yazik::rpc::grpc {

    void IGrpcQueueAction::set_is_ok(bool ok) {
        _is_ok = ok;
    }

    bool IGrpcQueueAction::is_ok() const {
        return _is_ok;
    }

    void* IGrpcQueueAction::self_tagged() {
#ifdef YA_DEBUG
        return with_mark(this);
#else
        return this;
#endif
    }

    GrpcQueueAction::GrpcQueueAction(concurrency::DispatchAction&& action)
    : _action { std::forward<concurrency::DispatchAction>(action) }
    {}

    void GrpcQueueAction::proceed(const concurrency::cancel_token_ptr& is_cancelled) {
        yaz_defer { delete this; };
        _action.execute(is_cancelled);
    }

    void GrpcQueueAction::dispatch(::grpc::CompletionQueue* queue) {
        _alarm.Set(queue, gpr_now(gpr_clock_type::GPR_CLOCK_REALTIME), self_tagged());
    }

    GrpcDelayedAction::GrpcDelayedAction(unique_function<void()>&& clbk)
    : _clbk { std::forward<unique_function<void()>>(clbk) }
    {}

    void GrpcDelayedAction::proceed(const concurrency::cancel_token_ptr&) {
        yaz_defer { delete this; };
        _clbk();
    }

    void GrpcDelayedAction::dispatch(::grpc::CompletionQueue* queue, std::chrono::nanoseconds delay) {
        auto deadline = gpr_time_add(
            gpr_now(gpr_clock_type::GPR_CLOCK_REALTIME),
            gpr_time_from_nanos(delay.count(), gpr_clock_type::GPR_TIMESPAN)
        );
        _alarm.Set(queue, deadline, self_tagged());
    }

    GrpcScheduledAction::GrpcScheduledAction(
        unique_function<bool()>&& clbk,
        std::chrono::nanoseconds period,
        ::grpc::CompletionQueue* queue,
        bool strict
    )
    : _clbk { std::forward<unique_function<bool()>>(clbk) }
    , _period { period }
    , _queue { queue }
    , _strict { strict }
    {}

    void GrpcScheduledAction::proceed(const concurrency::cancel_token_ptr& is_cancelled) {
        bool need_stop = is_cancelled->is_cancelled();
        yaz_defer { if (need_stop) delete this; };
        if (need_stop) return;
        need_stop = !_clbk();
        if (!need_stop) dispatch();
    }

    void GrpcScheduledAction::dispatch() {
        if (_strict && _deadline) {
            auto now = gpr_now(gpr_clock_type::GPR_CLOCK_REALTIME);
            auto diff = gpr_time_sub(now, *_deadline);
            auto diff_millis = gpr_time_to_millis(diff);
            auto delay = _period.count() - (diff_millis * GPR_NS_PER_MS);
            _deadline = gpr_time_add(now, gpr_time_from_nanos(delay, gpr_clock_type::GPR_TIMESPAN));
        } else {
            _deadline = gpr_time_add(
                gpr_now(gpr_clock_type::GPR_CLOCK_REALTIME),
                gpr_time_from_nanos(_period.count(), gpr_clock_type::GPR_TIMESPAN)
            );
        }
        _alarm.Set(_queue, *_deadline, self_tagged());
    }

    void GrpcQueueTag::proceed(const concurrency::cancel_token_ptr& /*is_cancelled*/) {
        _scheduler = nullptr;

        if (_handle) {
            if (!_handle.done())
                _handle.resume();
        } else {
            auto err = do_format(
                "❗❗❗❗❗ ️No handle from: {} ❗❗❗❗",
                $yaz_debug(_descr) $yaz_release((void*)this)
            );
            L::error(err);
            throw std::runtime_error(err.toStdString());
        }
    }

    GrpcQueueTag::GrpcQueueTag($yaz_debug(const char* descr))
    $yaz_debug(: _descr { descr })
    {}

    GrpcQueueTag::GrpcQueueTagOp::GrpcQueueTagOp(const GrpcQueueTag* tag)
    : _tag { tag }
    {}

    auto GrpcQueueTag::operator co_await() const noexcept -> GrpcQueueTagOp {
        return GrpcQueueTagOp{ this };
    }

    bool GrpcQueueTag::GrpcQueueTagOp::await_ready() const noexcept {
        return _tag->_scheduler == nullptr;
    }

    bool GrpcQueueTag::GrpcQueueTagOp::await_resume() {
        return _tag->is_ok();
    }

    void GrpcQueueTag::GrpcQueueTagOp::await_suspend(std::experimental::coroutine_handle<> handle) {
        _tag->_handle = handle;
    }

    GrpcQueueScheduler::GrpcQueueScheduler(
        std::unique_ptr<::grpc::CompletionQueue> queue
    )
    : _queue { std::move(queue) }
    {
//        GRPC_CQ_INTERNAL_REF(_queue->cq(), "crash guard");
    }

    GrpcQueueScheduler::~GrpcQueueScheduler() {
        _queue->Shutdown();
    }

    intrusive_ptr<GrpcQueueScheduler> GrpcQueueScheduler::create() {
        return new GrpcQueueScheduler{ std::make_unique<::grpc::CompletionQueue>() };
    }

    void GrpcQueueScheduler::poll_all_deferred() {
        _deferred_stack.sweep([](auto&& fn) { fn(); });
    }

    const concurrency::cancel_token_ptr& GrpcQueueScheduler::need_cancel() {
        return _need_cancel;
    }

    concurrency::Disposer GrpcQueueScheduler::dispatch_impl(
        unique_function<void(concurrency::CancellationToken&&)>&& clbk
        $yaz_debug(, const char* descr)
    ) {
        auto [action, disposer] = concurrency::DispatchAction::managed(
            std::forward<unique_function<void(concurrency::CancellationToken&&)>>(clbk)
        );
        (new GrpcQueueAction(std::move(action)))->dispatch(queue());
        return disposer;
    }

    void GrpcQueueScheduler::dispatch_impl(
        concurrency::unique_function<void()>&& clbk
        $yaz_debug(, const char* descr)
    ) {
        (new GrpcQueueAction(concurrency::DispatchAction::plain(
            std::forward<unique_function<void()>>(clbk)
        )))->dispatch(queue());
    }

    void GrpcQueueScheduler::defer_impl(
        concurrency::unique_function<void()>&& clbk
        $yaz_debug(, const char* descr)
    ) {
        _deferred_stack.push(std::move(clbk));
    }

    void GrpcQueueScheduler::schedule_delayed_impl(
        concurrency::unique_function<void()>&& clbk,
        std::chrono::nanoseconds delay
    ) {
        (new GrpcDelayedAction(
            std::forward<concurrency::unique_function<void()>>(clbk)
        ))->dispatch(queue(), delay);
    }

    void GrpcQueueScheduler::schedule_periodic_impl(
        concurrency::unique_function<bool()>&& clbk,
        std::chrono::nanoseconds period,
        bool strict
    ) {
        (new GrpcScheduledAction(
            std::forward<concurrency::unique_function<bool()>>(clbk),
            period,
            queue(),
            strict
        ))->dispatch();
    }

    bool GrpcQueueScheduler::has_ops_since_last_check() noexcept {
        bool has = _has_ops_since_last_check;
        _has_ops_since_last_check = false;
        return has;
    }

    bool GrpcQueueScheduler::check_need_stop() {
        return _need_cancel->is_cancelled();
    }

    Result<bool> GrpcQueueScheduler::check_status(::grpc::CompletionQueue::NextStatus sts) {
        using NS = ::grpc::CompletionQueue::NextStatus;
        switch (sts) {
        case NS::GOT_EVENT:
            flush();
            return true;
        case NS::TIMEOUT:
            increment();
            return false;
        default:
            return yaz_fail("unsuspicious grpc thread loop stop");
        }
    }

    Result<IGrpcQueueAction*> GrpcQueueScheduler::wait_for_action() {
        void* tag = nullptr;
        bool ok;
        auto deadline = gpr_time_add(
            gpr_now(gpr_clock_type::GPR_CLOCK_REALTIME),
            gpr_time_from_nanos(wait_nanosec_count(), gpr_clock_type::GPR_TIMESPAN)
        );
        if (!co_await check_status(_queue->AsyncNext(&tag, &ok, deadline)))
            co_return nullptr;
        if (!tag) co_await yaz_fail("unexpected behaviour");
#ifdef YA_DEBUG
        if (!is_marked(tag)) {
            L::warn("❗❗❗❗❗ Unknown tag: {} ❗❗❗❗", tag);
            co_await yaz_fail("unexpected behaviour");
        }
        auto* action = clear_mark<IGrpcQueueAction>(tag);
#else
        auto* action = static_cast<GrpcQueueTag*>(tag);
#endif
        action->set_is_ok(ok);
        co_return action;
    }

    bool GrpcQueueScheduler::step() {
        auto action = wait_for_action();
        if (!action) {
            stop_impl();
            return false;
        }
        if (!action.value()) return false;
        _has_ops_since_last_check = true;
        IGrpcQueueAction* action_ptr = action.value();
        action_ptr->proceed(_need_cancel);
        poll_all_deferred();
        return true;
    }

    void GrpcQueueScheduler::stop_impl() {
        if (_need_cancel)
            _need_cancel->cancel();
    }

    ::grpc::CompletionQueue* GrpcQueueScheduler::queue() {
        return _queue.get();
    }

    Future<> GrpcQueueScheduler::run_work(std::function<Future<>()>&& work) {
        auto future = work();
        while(step() || !check_need_stop())
            if (future.is_ready())
                break;
        while(step());
        co_await future;
        co_return;
    }

    GrpcQueueTag::GrpcQueueTagOp GrpcQueueScheduler::wait_for_tag(GrpcQueueTag& tag) {
        tag._scheduler = this;
        return tag.operator co_await();
    }

    intrusive_ptr<GrpcQueueThreadScheduler> GrpcQueueThreadScheduler::create() {
        return new GrpcQueueThreadScheduler { std::make_unique<::grpc::CompletionQueue>() };
    }

    GrpcQueueThreadScheduler::~GrpcQueueThreadScheduler() {
        stop();
        wait();
    }

    const std::optional<uint64_t>& GrpcQueueThreadScheduler::thread_id() const {
        return _thread_id;
    }

    void GrpcQueueThreadScheduler::thread_loop() {
        _thread_id = concurrency::thread_idx();
        while(step() || !check_need_stop());
        while(step());
        _on_stop.set_value();
    }

    Result<void> GrpcQueueThreadScheduler::start() {
        if (_thread && _thread->joinable())
            return yaz_fail("already started");
        _need_cancel = new concurrency::CancellationTokenAtomic;
        _thread = std::make_shared<std::thread>(std::bind(&GrpcQueueThreadScheduler::thread_loop, this));
        return yaz_ok();
    }

    void GrpcQueueThreadScheduler::stop() {
        stop_impl();
    }

    void GrpcQueueThreadScheduler::wait() {
        if (_thread && _thread->joinable()) _thread->join();
        _thread.reset();
        _thread_id = {};
        _need_cancel.reset();
    }

    Future<> GrpcQueueThreadScheduler::on_stop() {
        return _on_stop.get_future();
    }

    GrpcServerQueueThreadScheduler::GrpcServerQueueThreadScheduler(std::unique_ptr<::grpc::ServerCompletionQueue> queue)
    : GrpcQueueThreadScheduler { std::move(queue) }
    {}

    ::grpc::ServerCompletionQueue* GrpcServerQueueThreadScheduler::server_queue() {
        return static_cast<::grpc::ServerCompletionQueue*>(queue());
    }

} // end of ::yazik::rpc::grpc namespace