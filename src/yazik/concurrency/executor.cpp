#include "executor.hpp"
#include "event.hpp"

#include <folly/system/ThreadId.h>
#include <folly/synchronization/HazptrDomain.h>

namespace yazik {
namespace concurrency {

    uint64_t thread_idx() {
        return ::folly::getCurrentThreadID();
    }

    CancellationToken::CancellationToken(cancel_token_ptr impl)
    : _impl { std::move(impl) }
    {}

    bool CancellationToken::is_cancelled() const noexcept {
        return _impl && _impl->is_cancelled();
    }

    CancellationTokenAtomic::CancellationTokenAtomic()
    : _is_cancelled { false }
    {}

    bool CancellationTokenAtomic::is_cancelled() const noexcept {
        return _is_cancelled.load(std::memory_order_relaxed);
    }

    void CancellationTokenAtomic::cancel() noexcept {
        _is_cancelled.store(true);
    }

    CancellationTokenComposite::CancellationTokenComposite(vector<CancellationToken>&& tokens)
    : _tokens { std::move(tokens) }
    {}

    bool CancellationTokenComposite::is_cancelled() const noexcept {
        for (auto& token: _tokens)
            if (token.is_cancelled())
                return true;
        return false;
    }

    void CancellationTokenComposite::cancel() noexcept {
        for (auto& token: _tokens)
            token._impl->cancel();
    }

    void Disposer::dispose() {
        _is_disposed->cancel();
    }

    CancellationToken Disposer::cancellation_token() {
        return {_is_disposed};
    }

    DispatchAction::DispatchAction(ActionVariant&& action)
    : _action { std::move(action) }
    {}

    DispatchAction DispatchAction::plain(PlainAction&& action) {
        return {{std::move(action)}};
    }

    auto DispatchAction::managed(ManagedAction&& action) -> ManagedDispatch {
        auto disposer = Disposer{};
        auto dispatch_action = DispatchAction{{ManagedActionPair{
            std::move(action),
            disposer.cancellation_token()
        }}};
        return {
            std::move(dispatch_action),
            std::move(disposer)
        };
    }

    void DispatchAction::execute(const cancel_token_ptr& is_cancelled) {
        if (std::holds_alternative<PlainAction>(_action)) {
//            if (!is_cancelled->load(std::memory_order::relaxed))
            std::get<PlainAction>(_action)();
        } else if (std::holds_alternative<ManagedActionPair>(_action)) {
            auto [action, stored_token] = std::move(std::get<ManagedActionPair>(_action));
            auto tokens = vector<CancellationToken>{};
            if (is_cancelled)
                tokens.emplace_back(is_cancelled);
            tokens.emplace_back(std::move(stored_token));
            action(CancellationToken{ new CancellationTokenComposite { std::move(tokens) }});
        }
    }

    EnsureOnExecutor::OnAwaitable::OnAwaitable(
        Executor* executor,
        bool is_deferred
        $yaz_debug(, const char* descr)
    )
    : _executor { executor }
    , _is_deferred { is_deferred }
    $yaz_debug(, _descr { descr })
    {}

    bool EnsureOnExecutor::OnAwaitable::await_ready() const noexcept { return _executor == nullptr; }

    void EnsureOnExecutor::OnAwaitable::await_suspend(std::experimental::coroutine_handle<> awaiter) {
        if (!_executor) {
            awaiter.resume();
            return;
        }
        concurrency::unique_function<void()> resume_fn = [awaiter]() mutable {
            if (awaiter && !awaiter.done())
                awaiter.resume();
        };
        if (_is_deferred) {
            _executor->defer_impl(std::move(resume_fn) $yaz_debug(, _descr));
        } else {
            _executor->dispatch_impl(std::move(resume_fn) $yaz_debug(, _descr));
        }
    }

    void EnsureOnExecutor::OnAwaitable::await_resume() const noexcept {}

    bool EnsureOnExecutor::is_on() noexcept {
        return false;
    }

    auto EnsureOnExecutor::ensure_on($yaz_debug(const char* descr)) -> OnAwaitable {
        if (is_on())
            return {};
        return on($yaz_debug(descr));
    }

    auto Executor::on($yaz_debug(const char* descr)) -> OnAwaitable {
        return {this, false $yaz_debug(, descr)};
    }

    auto Executor::on_deferred($yaz_debug(const char* descr)) -> OnAwaitable {
        return {this, true $yaz_debug(, descr)};
    }

    void Executor::add(unique_function<void ()> fn) {
        dispatch_impl(std::move(fn) YAZ_PRE_COMMA_LOCATION_STR);
    }

    bool ThreadIdHolder::is_on() noexcept {
        return thread_id() == thread_idx();
    }

    void ThreadExecutor::stop_joined() {
        stop();
        wait();
        $breakpoint_hint
    }

    bool SingleThreadExecutor::poll_all_dispatched() {
        return _dispatch_stack.sweep_ordered([this](DispatchAction&& action){
            action.execute(_need_cancel);
            poll_all_deferred();
        });
    }

    void SingleThreadExecutor::poll_all_deferred() {
        _deferred_stack.sweep([](auto&& fn) { fn(); });
    }

    void SingleThreadExecutor::thread_loop() {
        _thread_id = thread_idx();
        {
            CVWaiter baton{_wait_cv};
            while (!_need_cancel->is_cancelled())
                baton.check(poll_all_dispatched());
        }
        while (poll_all_dispatched());
    }

    SingleThreadExecutor::SingleThreadExecutor()
    {}

    SingleThreadExecutor::~SingleThreadExecutor() {
        stop();
        wait();
    }

    const std::optional<uint64_t>& SingleThreadExecutor::thread_id() const {
        return _thread_id;
    }

    Disposer SingleThreadExecutor::dispatch_impl(
        unique_function<void(CancellationToken&&)>&& clbk
        $yaz_debug(, const char* descr)
    ) {
        auto [action,disposer] = DispatchAction::managed(std::move(clbk));
       _dispatch_stack.push(std::move(action));
       _wait_cv.notify_one();
       return disposer;
    }
    void SingleThreadExecutor::dispatch_impl(
        unique_function<void()>&& clbk
         $yaz_debug(, const char* descr)
    ) {
        _dispatch_stack.push(DispatchAction::plain(std::move(clbk)));
        _wait_cv.notify_one();
    }

    void SingleThreadExecutor::defer_impl(
        unique_function<void()>&& clbk
         $yaz_debug(, const char* descr)
    ) {
        _deferred_stack.push(std::move(clbk));
        _wait_cv.notify_one();
    }

    Result<void> SingleThreadExecutor::start() {
        if (_thread && _thread->joinable())
            return yaz_fail<string>("already started");
        _need_cancel = new CancellationTokenAtomic;
        _thread = std::make_shared<std::thread>(std::bind(&SingleThreadExecutor::thread_loop, this));
        return yaz_ok();
    }

    void SingleThreadExecutor::stop() {
        if (_need_cancel) _need_cancel->cancel();
        _wait_cv.notify_one();
    }

    void SingleThreadExecutor::wait() {
        if (_thread && _thread->joinable()) _thread->join();
        _thread.reset();
        _need_cancel.reset();
        _thread_id = {};
    }

    intrusive_ptr<SingleThreadExecutor> create_single_thread_executor(bool start) {
        intrusive_ptr result = new SingleThreadExecutor{};
        if (start && !result->start()) return nullptr;
        return result;
    }

    const executor_ptr_t IExecutorProvider::s_null_executor = nullptr;

    executor_ptr_t s_default_ex;
    ::folly::Executor* default_executor_for_folly() {
        return s_default_ex.get();
    }

    void mark_default_executor(const executor_ptr_t& ex) {
        s_default_ex = ex;
        ::folly::default_hazptr_domain().set_executor(default_executor_for_folly);
    }

} // end of ::yazik::concurrency namespace
} // end of ::yazik namespace