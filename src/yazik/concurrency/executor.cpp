#include "executor.hpp"
#include "event.hpp"

#include <folly/system/ThreadId.h>

namespace yazik {
namespace concurrency {

    uint64_t thread_idx() {
        return ::folly::getCurrentThreadID();
    }

    CancellationToken::CancellationToken(std::unique_ptr<ICancellationTokenImpl>&& impl)
    : _impl { std::move(impl) }
    {}

    bool CancellationToken::is_cancelled() const noexcept {
        return _impl && _impl->is_cancelled();
    }

    CancellationTokenAtomic::CancellationTokenAtomic(
        std::shared_ptr<std::atomic_bool> is_cancelled
    )
    : _is_cancelled { std::move(is_cancelled) }
    {}

    bool CancellationTokenAtomic::is_cancelled() const noexcept {
        return _is_cancelled->load();
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

    void Disposer::dispose() {
        _is_disposed->store(true);
    }

    CancellationToken Disposer::cancellation_token() {
        return {std::make_unique<CancellationTokenAtomic>(_is_disposed)};
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

    void DispatchAction::execute(const std::shared_ptr<std::atomic_bool>& is_cancelled) {
        if (std::holds_alternative<PlainAction>(_action)) {
//            if (!is_cancelled->load(std::memory_order::relaxed))
            std::get<PlainAction>(_action)();
        } else if (std::holds_alternative<ManagedActionPair>(_action)) {
            auto [action, stored_token] = std::move(std::get<ManagedActionPair>(_action));
            auto tokens = vector<CancellationToken>{};
            if (is_cancelled)
                tokens.emplace_back(std::make_unique<CancellationTokenAtomic>(is_cancelled));
            tokens.emplace_back(std::move(stored_token));
            action(CancellationToken{std::make_unique<CancellationTokenComposite>(std::move(tokens))});
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

    auto EnsureOnExecutor::ensure_on($yaz_debug(const char* descr)) -> OnAwaitable {
        return on($yaz_debug(descr));
    }

    auto Executor::on($yaz_debug(const char* descr)) -> OnAwaitable {
        return {this, false $yaz_debug(, descr)};
    }

    auto Executor::on_deferred($yaz_debug(const char* descr)) -> OnAwaitable {
        return {this, true $yaz_debug(, descr)};
    }

    auto ThreadExecutor::ensure_on($yaz_debug(const char* descr)) -> OnAwaitable {
        if(thread_id() == thread_idx())
            return {};
        return on($yaz_debug(descr));
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
            while (!_need_cancel->load(std::memory_order_relaxed))
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
        _need_cancel = std::make_shared<std::atomic_bool>(false);
        _thread = std::make_shared<std::thread>(std::bind(&SingleThreadExecutor::thread_loop, this));
        return yaz_ok();
    }

    void SingleThreadExecutor::stop() {
        if (_need_cancel) _need_cancel->store(true);
        _wait_cv.notify_one();
    }

    void SingleThreadExecutor::wait() {
        if (_thread && _thread->joinable()) _thread->join();
        _thread.reset();
        _need_cancel.reset();
        _thread_id = {};
    }

    utility::intrusive_ptr<SingleThreadExecutor> create_single_thread_executor(bool start) {
        utility::intrusive_ptr result = new SingleThreadExecutor{};
        if (start && !result->start()) return nullptr;
        return result;
    }

    const executor_ptr_t IExecutorProvider::s_null_executor = nullptr;

} // end of ::yazik::concurrency namespace
} // end of ::yazik namespace