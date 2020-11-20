#pragma once

#include <functional>
#include <variant>

#include <folly/Function.h>

#include <yazik/utility/detail/promise_base.hpp>
#include <yazik/utility/type_traits.hpp>
#include <yazik/concurrency/task.hpp>
#include <yazik/concurrency/event.hpp>
#include <yazik/concurrency/stack.hpp>
#include <yazik/concurrency/task.hpp>

namespace yazik::concurrency {

    template <typename... Signatures>
    using unique_function = ::folly::Function<Signatures...>;

    uint64_t thread_idx();

    struct ICancellationTokenImpl {
        virtual bool is_cancelled() const noexcept = 0;
        virtual ~ICancellationTokenImpl() {};
    };

    class CancellationTokenComposite;
    class CancellationToken {
        std::unique_ptr<ICancellationTokenImpl> _impl;
        friend class CancellationTokenComposite;
    public:
        CancellationToken() = default;
        CancellationToken(std::unique_ptr<ICancellationTokenImpl>&& impl);
        bool is_cancelled() const noexcept;
    };

    class CancellationTokenAtomic
    : public ICancellationTokenImpl {
        std::shared_ptr<std::atomic_bool> _is_cancelled;
    public:
        CancellationTokenAtomic(std::shared_ptr<std::atomic_bool> is_cancelled);
        bool is_cancelled() const noexcept override;
    };

    class CancellationTokenComposite
    : public ICancellationTokenImpl {
        const vector<CancellationToken> _tokens;
    public:
        CancellationTokenComposite(vector<CancellationToken>&& tokens);
        bool is_cancelled() const noexcept override;
    };

    class Disposer {
        std::shared_ptr<std::atomic_bool> _is_disposed
            = std::make_shared<std::atomic_bool>(false);
    public:
        void dispose();
        CancellationToken cancellation_token();
    };

    struct Executor;
    struct EnsureOnExecutor {
        class OnAwaitable {
            Executor* _executor = nullptr;
            bool _is_deferred;
             $yaz_debug(const char* _descr;)
        public:
            OnAwaitable() = default;
            OnAwaitable(Executor* executor, bool is_deferred  $yaz_debug(, const char* descr));
            bool await_ready() const noexcept;

            void await_suspend(
                std::experimental::coroutine_handle<> awaiter
            );

            template <typename Promise>
            void await_suspend(std::experimental::coroutine_handle<Promise> awaiter);
            void await_resume() const noexcept;
        };

        virtual bool is_on() noexcept;
        virtual OnAwaitable on($yaz_debug(const char* descr)) = 0;
        OnAwaitable ensure_on($yaz_debug(const char* descr));
        virtual OnAwaitable on_deferred($yaz_debug(const char* descr)) = 0;
    };

    struct Executor
    : virtual EnsureOnExecutor
    , virtual utility::ref_counted {
        virtual Disposer dispatch_impl(
            unique_function<void(CancellationToken&&)>&& clbk
            $yaz_debug(, const char* descr)
        ) = 0;
        virtual void dispatch_impl(
            unique_function<void()>&& clbk
            $yaz_debug(, const char* descr)
        ) = 0;
        virtual void defer_impl(
            unique_function<void()>&& clbk
            $yaz_debug(, const char* descr)
        ) = 0;

        template <typename Fn>
        auto dispatch(Fn&& fn $yaz_debug(, const char* descr)) {
            if constexpr (
                utility::is_callable_with<Fn, CancellationToken&&>::value
                && !utility::is_callable_with<Fn>::value
            ) {
                return dispatch_impl(
                    [_l_forward(fn)](CancellationToken&& t) mutable { fn(std::move(t)); }
                    $yaz_debug(, descr)
                );
            } else {
                dispatch_impl(
                    [_l_forward(fn)]() mutable { fn(); }
                    $yaz_debug(, descr)
                );
                return;
            }
        }

        template <typename Fn>
        Task<typename std::result_of_t<Fn()>::value_type> dispatch_future(Fn&& fn $yaz_debug(, const char* descr)) {
            co_await ensure_on($yaz_debug(descr));
            auto task = fn();
            co_return co_await task;
        }

        template <typename Fn>
        Task<typename std::result_of_t<Fn()>> dispatch_task(Fn&& fn $yaz_debug(, const char* descr)) {
            return dispatch_future(
                [_l_forward(fn)] () mutable -> Task<typename std::result_of_t<Fn()>> {
                    co_return fn();
                }
                $yaz_debug(, descr)
            );
        }

        template <typename Fn>
        inline void defer(Fn&& fn $yaz_debug(,const char* descr)) {
            defer_impl([ _l_forward(fn)]() mutable { fn(); } $yaz_debug(, descr));
        }

        virtual ~Executor() {}

        OnAwaitable on($yaz_debug(const char* descr)) override;
        OnAwaitable on_deferred($yaz_debug(const char* descr)) override;
    };

    template <typename Promise>
    void EnsureOnExecutor::OnAwaitable::await_suspend(
        std::experimental::coroutine_handle<Promise> awaiter
    ) {
        if (!_executor) {
            if constexpr (std::is_base_of_v<promises::BasePromise, Promise>)
                promises::propagate(awaiter);
            else
                awaiter.resume();
            return;
        }
        concurrency::unique_function<void()> resume_fn = [awaiter]() mutable {
            if (awaiter) {
                if constexpr (std::is_base_of_v<promises::BasePromise, Promise>) {
                    promises::propagate(awaiter);
                } else {
                    awaiter.resume();
                }
            }
        };
        if (_is_deferred) {
            _executor->defer_impl(std::move(resume_fn) $yaz_debug(, _descr));
        } else {
            _executor->dispatch_impl(std::move(resume_fn) $yaz_debug(, _descr));
        }
    }

    class DispatchAction {
    public:
        using PlainAction = unique_function<void()>;
        using ManagedAction = unique_function<void(CancellationToken&&)>;
        using ManagedActionPair = std::pair<
            ManagedAction,
            CancellationToken
        >;
        using ManagedDispatch = std::pair<
            DispatchAction,
            Disposer
        >;
        using ManagedDispatchPtr = std::pair<
            DispatchAction*,
            Disposer
        >;
    private:
        using ActionVariant = std::variant<
            PlainAction,
            ManagedActionPair
        >;
        ActionVariant _action;

    protected:
        DispatchAction(ActionVariant&& action);
    public:
        DispatchAction() = default;
        static DispatchAction plain(PlainAction&& action);
        static ManagedDispatch managed(ManagedAction&& action);
        void execute(const std::shared_ptr<std::atomic_bool>& is_cancelled);
    };

    struct ThreadIdHolder: virtual EnsureOnExecutor {
        virtual const std::optional<uint64_t>& thread_id() const = 0;
        bool is_on() noexcept override;
    };

    struct ThreadExecutor
    : virtual ThreadIdHolder
    , virtual utility::ref_counted {

        virtual Result<void> start() = 0;
        virtual void stop() = 0;
        virtual void wait() = 0;
        void stop_joined();
    };

    class SingleThreadExecutor
    : public Executor
    , public ThreadExecutor {
        std::shared_ptr<std::atomic_bool> _need_cancel;
        std::shared_ptr<std::thread> _thread;
        std::optional<uint64_t> _thread_id;

        Stack<concurrency::unique_function<void()>> _deferred_stack;

        std::condition_variable _wait_cv;
        Stack<DispatchAction>  _dispatch_stack;

        bool poll_all_dispatched();
        void poll_all_deferred();
        void thread_loop();

    public:

        SingleThreadExecutor();
        virtual ~SingleThreadExecutor();

        const std::optional<uint64_t>& thread_id() const override;

        Disposer dispatch_impl(
            unique_function<void(CancellationToken&&)>&& clbk
             $yaz_debug(, const char* descr)
        ) override;
        void dispatch_impl(
            unique_function<void()>&& clbk
             $yaz_debug(, const char* descr)
        ) override;
        void defer_impl(
            unique_function<void()>&& clbk
             $yaz_debug(, const char* descr)
        ) override;
        Result<void> start() override;
        void stop() override;
        void wait() override;
    };

    intrusive_ptr<SingleThreadExecutor> create_single_thread_executor(bool start = true);

    using executor_ptr_t = intrusive_ptr<concurrency::Executor>;
    using thread_executor_ptr_t = intrusive_ptr<concurrency::ThreadExecutor>;

    struct IExecutorProvider {
        static const executor_ptr_t s_null_executor;
        virtual const executor_ptr_t& get_executor() const noexcept = 0;
        virtual vector<executor_ptr_t> all_executors() const noexcept = 0;
    };

    struct IThreadExecutorProvider
    : virtual IExecutorProvider {
        virtual Result<> start_executor() = 0;
        virtual Result<> stop_executor() = 0;
        virtual Result<> wait_executor() = 0;
    };

    template <typename ExecutorImpl>
    class SingleThreadExecutorProvider
    : public virtual IThreadExecutorProvider {
        ThreadExecutor* _thread_ex;
        executor_ptr_t _executor;

        inline ThreadExecutor* thread_ex() {
            return _thread_ex;
        }

        static_assert(std::is_base_of_v<Executor, ExecutorImpl>, "");
        static_assert(std::is_base_of_v<ThreadExecutor, ExecutorImpl>, "");

    public:

        SingleThreadExecutorProvider() = default;

        SingleThreadExecutorProvider(std::shared_ptr<ExecutorImpl>&& executor) {
            set_executor(std::forward<std::shared_ptr<ExecutorImpl>>(executor));
        }

        void set_executor(std::shared_ptr<ExecutorImpl>&& executor) {
            _thread_ex = executor.get();
            _executor = std::forward<std::shared_ptr<ExecutorImpl>>(executor);
        }

        const executor_ptr_t& get_executor() const noexcept override {
            return _executor;
        }

        vector<executor_ptr_t> all_executors() const noexcept override {
            if (!_executor) return {};
            return { _executor };
        }

        Result<> start_executor() override {
            if (!_executor) co_await yaz_fail<string>("no executor");
            co_await thread_ex()->start();
            co_return;
        }

        Result<> stop_executor() override {
            if (!_executor) co_await yaz_fail<string>("no executor");
            thread_ex()->stop();
            co_return;
        }

        Result<> wait_executor() override {
            if (!_executor) co_await yaz_fail<string>("no executor");
            thread_ex()->wait();
            co_return;
        }
    };

    template <typename ExecutorImpl>
    class ThreadPoolExecutorProvider
    : public virtual IThreadExecutorProvider {
        vector<executor_ptr_t> _executors;
        vector<ThreadExecutor*> _thread_executors;
        mutable std::atomic_uint _current_executor {0};

        static_assert(std::is_base_of_v<Executor, ExecutorImpl>, "");
        static_assert(std::is_base_of_v<ThreadExecutor, ExecutorImpl>, "");

    public:

        ThreadPoolExecutorProvider() = default;

        ThreadPoolExecutorProvider(
            unsigned count,
            std::function<std::shared_ptr<ExecutorImpl>()>&& fabric
        ) {
            set_executors(count, std::forward<std::function<std::shared_ptr<ExecutorImpl>()>>(fabric));
        }

        void set_executors(
            unsigned count,
            std::function<std::shared_ptr<ExecutorImpl>()>&& fabric
        ) {
            for (unsigned i = 0; i < count; ++i) {
                auto ex = fabric();
                _thread_executors.push_back(ex.get());
                _executors.push_back(std::move(ex));
            }
        }

        const executor_ptr_t& get_executor() const noexcept override {
            if (_executors.empty()) return IThreadExecutorProvider::s_null_executor;
            unsigned current_executor = _current_executor.fetch_add(1);
            unsigned thread_num = current_executor % _executors.size();
            return _executors[thread_num];
        }

        vector<executor_ptr_t> all_executors() const noexcept override {
            return _executors;
        }

        Result<> start_executor() override {
            if (_executors.empty()) co_await yaz_fail<string>("no executor");
            for (auto& thr_ex: _thread_executors)
                co_await thr_ex->start();
            co_return;
        }

        Result<> stop_executor() override {
            if (_executors.empty()) co_await yaz_fail<string>("no executor");
            for (auto& thr_ex: _thread_executors)
                thr_ex->stop();
            co_return;
        }

        Result<> wait_executor() override {
            if (_executors.empty()) co_await yaz_fail<string>("no executor");
            for (auto& thr_ex: _thread_executors)
                 thr_ex->wait();
            co_return;
        }
    };

} // end of ::yazik::concurrency namespace