#pragma once

#include <yazik/concurrency/detail/future_detail.hpp>

#include <yazik/utility/detail/promise_base.hpp>
#include <yazik/utility/logging.hpp>
#include <yazik/utility/result.hpp>

namespace yazik {

    template <typename T, typename Error>
    class FutureBase {
    protected:
        mutable ::pc::future<T> _impl;

        std::exception_ptr exception() {
            return ::pc::detail::state_of<T>(*this)->exception();
        }

    public:
        using value_type = T;
        FutureBase(::pc::future<T>&& f)
        : _impl{ std::move(f) }
        {}
        FutureBase(FutureBase&& rhs) noexcept = default;
        FutureBase(const FutureBase&) = delete;
        FutureBase& operator=(const FutureBase&) = delete;
        FutureBase& operator=(FutureBase&& rhs) noexcept = default;
        ~FutureBase() = default;

        inline Result<T, Error> get() {
            return detail::unwrap_future<T, Error>(_impl);
        }

        inline void detach() {
            _impl.detach();
        }

        inline T get_unwrapped() {
            return _impl.get();
        }

        inline bool valid() const noexcept { return _impl.valid(); }
        inline bool is_ready() const { return _impl.is_ready(); }
        inline void wait() const { _impl.wait(); }

        template <typename Rep, typename Period>
        ::pc::future_status wait_for(const std::chrono::duration<Rep, Period>& rel_time) const {
            auto waiter = ::pc::timed_waiter { _impl };
            return waiter.template wait_for<Rep, Period>(rel_time);
        }

        inline bool await_ready() const noexcept {
            return is_ready();
        }

        inline void await_suspend(std::experimental::coroutine_handle<> handle) {
            _impl.await_suspend(std::move(handle));
        }
    };

    template <typename T, typename Error>
    class FutureResult
    : public FutureBase <T, Error> {
        using Super = FutureBase<T, Error>;
    public:

        using typename Super::value_type;
        using Super::Super;
        using Super::operator=;

        inline Result<T, Error> await_resume() {
            return Super::get();
        }
    };

    template <typename T = void, typename Error = string>
    class [[nodiscard]] Future
    : public FutureBase <T, Error> {
        using Super = FutureBase <T, Error>;
    public:

        using typename Super::value_type;
        using Super::Super;
        using Super::operator=;
        using promise_type =  promises::FuturePromise<T, Error>;

        FutureResult<T, Error> wrapped() noexcept {
            return { std::move(Super::_impl) };
        }

        inline T await_resume() {
            return Super::_impl.get();
        }

    };

namespace promises {

    template <
        typename T,
        typename Error,
        template <typename, typename> typename FutureType
    > class FuturePromiseBase {
    protected:
        ::pc::promise<T> _impl;
    public:

        inline void set_exception(std::exception_ptr e) noexcept {
            _impl.set_exception(e);
        }

        Future<T, Error> get_return_object() {
            return _impl.get_future();
        }

        void unhandled_exception() {
            set_exception(std::current_exception());
        }

        auto initial_suspend() const noexcept {
            return std::experimental::suspend_never{};
        }
        auto final_suspend() const noexcept {
            return std::experimental::suspend_never{};
        }
    };

    template <typename T = void, typename Error = string>
    struct FuturePromise
    : FuturePromiseBase<T, Error, Future> {
        using FuturePromiseBase<T, Error, Future>::FuturePromiseBase;

        inline void set_value(const T& val) { this->_impl.set_value(val); }
        inline void set_value(T&& val) { this->_impl.set_value(std::move(val)); }

        void return_value(const T& val) { set_value(val); }
        void return_value(T&& val) { set_value(std::move(val)); }
    };

    template <typename Error>
    struct FuturePromise<void, Error>
    : FuturePromiseBase<void, Error, Future> {
        using FuturePromiseBase<void, Error, Future>::FuturePromiseBase;

        inline void set_value() { this->_impl.set_value(); }
        void return_void() { set_value(); }
    };
}

    template <typename T, typename Error = string>
    using FutureSPtr = Future<std::shared_ptr<T>, Error>;

    template <typename Error = string>
    Future<void, Error> yaz_ok_f() noexcept {
        return ::pc::make_ready_future();
    }

    template <typename T, typename Error = string>
    Future<T, Error> yaz_ok_f(T&& value) noexcept {
        return ::pc::make_ready_future(std::forward<T>(value));
    }

    template <typename T = void, typename Error = string>
    Future<T, Error> yaz_fail_f(Error&& e) noexcept {
        return ::pc::make_exceptional_future<T>(std::forward<Error>(e));
    }

    template <typename T, typename Error>
    class PromiseBase {
    protected:
        ::pc::promise<T> _impl;
    public:
        PromiseBase() = default;

        PromiseBase(std::weak_ptr<pc::detail::shared_state<T>>&& state)
        : _impl(std::move(state))
        {}

        PromiseBase(PromiseBase&&) noexcept = default;
        PromiseBase(const PromiseBase&) = delete;

        PromiseBase& operator=(const PromiseBase&) = delete;
        PromiseBase& operator=(PromiseBase&& rhs) noexcept = default;

        ~PromiseBase() = default;

        inline Future<T, Error> get_future() {
            return _impl.get_future();
        }

        inline void set_exception(std::exception_ptr e) noexcept {
            _impl.set_exception(e);
        }
    };

    template <typename T = void, typename Error = string>
    class Promise;

    template <typename Error>
    class Promise<void, Error>
    : public PromiseBase<void, Error> {
        using Base = PromiseBase<void, Error>;
    public:
        using Base::Base;
        using Base::operator =;

        void set_value() {
            Base::_impl.set_value();
        }
    };

    template <typename T, typename Error>
    class Promise
    : public PromiseBase<T, Error> {
        using Base = PromiseBase<T, Error>;
    public:
        using Base::Base;
        using Base::operator =;

        void set_value(T&& value) {
            Base::_impl.set_value(std::forward<T>(value));
        }
    };

} // end of ::yazik namespace
