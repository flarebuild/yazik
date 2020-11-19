#pragma once

#include <functional>
#include <utility>
#include <type_traits>
#include <mutex>

#include <yazik/utility/macro.hpp>
#include <yazik/utility/result.hpp>

namespace yazik {
namespace utility {
    template <typename Impl>
    struct LambdaContinuationMark: Impl {
        using Impl::Impl;
        ~LambdaContinuationMark() { this->~Impl(); }
    };

    class DeferImpl {
        using continuation_t = std::function<void ()>;
        continuation_t _continuation;
    public:
        DeferImpl(DeferImpl&& other) {
            _continuation = std::move(other._continuation);
            other._continuation = nullptr;
        }

        DeferImpl() = default;

        DeferImpl apply(continuation_t&& continuation) {
            _continuation = std::move(continuation);
            return std::move(*this);
        }

        ~DeferImpl() {
            if (_continuation)
                _continuation();
        }
    };

    template<typename T>
    class DoImpl {
        using continuation_t = std::function<void (T)>;
        T _value;
    public:
        DoImpl(T value): _value{value} {}
        void apply(continuation_t&& continuation) { continuation(std::forward<T>(_value)); }
    };

    template<typename T>
    class WithImpl {
        using continuation_t = std::function<void (T&)>;
        T _value;
    public:
        WithImpl(T&& value): _value{std::move(value)} {}
        T apply(continuation_t&& continuation) {
            T result = std::move(_value);
            continuation(result);
            return std::move(result);
        }
    };

    class OnceImpl {
        using continuation_t = std::function<Result<void>()>;
        std::once_flag& _once_flag;
    public:
        OnceImpl(std::once_flag& once_flag);
        Result<void> apply(continuation_t&& continuation);
    };

    template<typename Impl, typename Fn>
    auto operator <<= (LambdaContinuationMark<Impl>&& impl, Fn fn) {
        using ret_t = decltype(impl.apply(std::move(fn)));
        if constexpr (std::is_void_v<ret_t>)
            return impl.apply(std::move(fn));
        else
            return std::forward<ret_t>(impl.apply(std::move(fn)));
    }
} // end of ::yazik::utility namespace
} // end of ::yazik namespace

#define yaz_defer \
    auto YAZ_CONCAT(__DEFER_, __LINE__ ) \
        = ::yazik::utility::LambdaContinuationMark<::yazik::utility::DeferImpl>{} <<= [&]()

#define yaz_once(flag) \
    ::yazik::utility::LambdaContinuationMark<::yazik::utility::Once>{} <<= [&]

#define yaz_do(name, ...) \
    ::yazik::utility::LambdaContinuationMark<::yazik::utility::DoImpl<decltype(__VA_ARGS__)>>{__VA_ARGS__} \
        <<= [&](decltype(__VA_ARGS__) name)

#define yaz_with(name, ...) \
    ::yazik::utility::LambdaContinuationMark<::yazik::utility::WithImpl<decltype(__VA_ARGS__)>>{__VA_ARGS__} \
        <<= [&](decltype(__VA_ARGS__)& name)