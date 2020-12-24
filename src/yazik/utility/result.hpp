#pragma once

#include <string>
#include <stdexcept>

#include <functional>

#include <yazik/utility/detail/result_detail.hpp>
#include <yazik/utility/detail/promise_base.hpp>
#include <yazik/utility/detail/return_object_holder.hpp>
#include <yazik/utility/detail/result_error_enum_based.hpp>
#include <yazik/utility/ref_counted.hpp>
#include <yazik/utility/intrusive_ptr.hpp>

#include <yazik/utility/logging.hpp>
#include <yazik/utility/macro.hpp>

namespace yazik {

    template <typename T = void, typename Error = string>
    using Result = detail::Result<T, Error>;

    template <typename T, typename Error = string>
    using ResultRef = detail::Result<std::reference_wrapper<T>, Error>;

    template <typename T, typename Error = string>
    using ResultSPtr = Result<std::shared_ptr<T>, Error>;

    template <typename T, typename Error = string>
    using ResultIPtr = Result<intrusive_ptr<T>, Error>;

    template <typename T, typename Error = string>
    using ResultOpt = Result<std::optional<T>, Error>;

    using detail::yaz_ok;
    using detail::yaz_fail;
    using detail::yaz_fail_fmt;

    template<typename... Args>
    inline auto runtime_fail(Args... args) {
        return std::runtime_error(do_sformat(
            std::forward<Args>(args)...
        ));
    }

namespace promises {

    template <typename T, typename Error>
    class ResultAwaitable;

    template <typename T, typename Error>
    class ResultPromiseBase
    : public ErrorPropagationPromise<Error> {
    protected:
        detail::ReturnObjectHolder<Result<T, Error>>* _data;
    public:
        auto get_return_object() { return make_return_object_holder(_data); }
        auto initial_suspend() { return std::experimental::suspend_never{}; }
        auto final_suspend() noexcept { return std::experimental::suspend_never{}; }

        template <typename U, typename UError>
        auto await_transform(::folly::Expected<U, UError> value) {
            return std::move(value);
        }

        template <typename UError>
        auto await_transform(::folly::Unexpected<UError> err) {
            return std::move(err);
        }

        void set_error(Error&& error) {
            _data->emplace(Result<T, Error> { yaz_fail<Error>(
                std::forward<Error>(error)
            )});
        }

        std::experimental::coroutine_handle<>
        propagate_error_impl(Error&& error, std::experimental::coroutine_handle<>& self_h) override {
            set_error(std::forward<Error>(error));
            return nullptr;
        }

        void unhandled_exception() {
            set_error(detail::current_exception_to_error<Error>());
        }
    };

    template <typename Error>
    struct ResultPromise<void, Error>
    : ResultPromiseBase<void, Error> {
        void return_void() {
            this->_data->emplace(Result<void, Error>{ yaz_ok<Error>() });
        }
    };

    template <typename Error>
    struct ResultPromise<utility::unit_t, Error>
    : ResultPromiseBase<utility::unit_t, Error> {
        void return_void() {
            this->_data->emplace(Result<void, Error>{ yaz_ok<Error>() });
        }
    };

    template <typename T, typename Error>
    struct ResultPromise
    : ResultPromiseBase<T, Error> {
        void return_value(T x) {
            this->_data->emplace(Result<T, Error> { yaz_ok<T, Error>(std::move(x)) });
        }
    };

    template <typename T, typename Error>
    class ResultAwaitable {
        Result<T, Error> _result;
    public:
        ResultAwaitable(Result<T, Error>&& result)
        : _result { std::move(result) }
        {}

        ResultAwaitable(::folly::Unexpected<Error>&& err)
        : _result { std::move(err) }
        {}

        auto await_ready() {
            return _result.operator bool();
        }

        T await_resume() {
            if constexpr (std::is_same_v<T, void>) return;
            else return std::move(_result.value());
        }

        template <typename PromiseType>
        void await_suspend(std::experimental::coroutine_handle<PromiseType> h) {
            auto handle = promises::propagate_error(h, std::move(_result).error());
            if (handle) handle.resume();
        }

    };

} // end of promises namespace

    template <typename T, typename Error>
    inline auto operator co_await(::folly::Expected<T, Error> r) {
        return promises::ResultAwaitable<T, Error> { std::move(r) };
    }

    template <typename Error>
    inline auto operator co_await(::folly::Unexpected<Error> err) {
        return promises::ResultAwaitable<void, Error>{ std::move(err) };
    }


} // end of ::yazik namespace

// This makes Result<T> useable as a coroutine return type. Strictly, this
// specilaization should depend on a user-defined type, otherwise this is undefined
// behaviour. As this is purely for demonstration purposes, let's live dangerously.
namespace std {
namespace experimental {
    template<typename T, typename Error, typename... Args>
    struct coroutine_traits<::folly::Expected<T, Error>, Args...> {
        using promise_type = ::yazik::promises::ResultPromise<T, Error>;
    };
    template<typename Error, typename... Args>
    struct coroutine_traits<::folly::Unexpected<Error>, Args...> {
        using promise_type = ::yazik::promises::ResultPromise<void, Error>;
    };
} // end of ::std::experimental namespace
} // end of ::std namespace