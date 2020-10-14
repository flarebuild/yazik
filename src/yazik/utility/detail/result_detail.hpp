#pragma once

#include <boost/exception_ptr.hpp>
#include <folly/ExceptionWrapper.h>

#include <yazik/utility/unit.hpp>
#include <yazik/utility/utility_defs.hpp>
#include <yazik/utility/concepts.hpp>
#include <yazik/utility/type_traits.hpp>

namespace yazik::detail {

    template<typename Error>
    class ErrorWrapper {
        Error _error;
    public:
        ErrorWrapper() = default;
        ErrorWrapper(Error&& error)
        : _error { std::forward<Error>(error) }
        {}
        operator const Error&() const {
            return _error;
        }
    };

    template <typename T, typename Error>
    using Result = ::folly::Expected<
        std::conditional_t<std::is_void_v<T>, utility::unit_t, T>,
        Error
    >;

    struct UnxepectedStringTag {};

    template <typename Error>
    concept c_is_legal_result_error = concepts::c_just<Error, string> || ( 
            std::is_constructible_v<Error, UnxepectedStringTag, string>
            && requires (const Error* a) {
                { a->to_error_string() } -> concepts::c_just<string>;
            }
        );

    template <typename Error>
    inline Error result_error_from_str(const char* errmsg) {
        static_assert(
            c_is_legal_result_error<Error>,
            "Illegal result error type"
        );
        if constexpr (std::is_same_v<Error, string>)
            return string{errmsg};
        else
            return Error{detail::UnxepectedStringTag{}, errmsg};
    }

    template <typename Error>
    inline Error result_error_from_str(string&& errmsg) {
        static_assert(
            c_is_legal_result_error<Error>,
            "Illegal result error type"
        );
        if constexpr (std::is_same_v<Error, string>)
            return string{std::move(errmsg)};
        else
            return Error{detail::UnxepectedStringTag{}, std::move(errmsg)};
    }

    template <typename Error>
    inline Error current_exception_to_error() {
        if (auto exception_ptr = std::current_exception())
            try {
                std::rethrow_exception(exception_ptr);
            } catch (const Error& e) {
                return e;
            } catch (const ErrorWrapper<Error>& e) {
                return e;
            } catch (const std::exception& e) {
                return result_error_from_str<Error>(
                    do_format("unhandled_exception: {}", e.what())
                );
            } catch (...) {
                return result_error_from_str<Error>(
                    "unhandled_exception: unknown exception"
                );
            }
        else
            return result_error_from_str<Error>(
                "unhandled_exception: unknown exception"
            );
    }

    template <typename Error, typename UError>
    struct ErrorMapper;

    template <typename Error, typename UError>
    struct ErrorMapper <ErrorWrapper<Error>, UError> {
        static inline UError map(const ErrorWrapper<Error>& e) noexcept {
            return ErrorMapper<Error, UError>::map(e);
        }
        static inline UError map(ErrorWrapper<Error>&& e) noexcept {
            return ErrorMapper<Error, UError>::map(std::forward<Error>(e._error));
        }
    };

    template <typename Error, typename UError>
    struct ErrorMapper {
        static inline UError map(const Error& e) noexcept {
            auto e_dup = Error { e };
            return map<UError>(std::move(e_dup));
        }

        static inline UError map(Error&& e) noexcept {
            if constexpr (std::is_same_v<Error, UError>)
                return std::forward<Error>(e);
            else if constexpr (std::is_constructible_v<UError, Error>)
                return UError { std::forward<Error>(e) };
            else if constexpr (std::is_same_v<UError, string>)
                return e.to_error_string();
            else if constexpr (std::is_same_v<Error, string>)
                return UError{
                    UnxepectedStringTag{},
                    std::forward<Error>(e)
                };
            else if constexpr (std::is_same_v<Error, ::folly::exception_wrapper>)
                return ErrorMapper<string, UError>::map(e.what());
            else
                static_assert(
                    utility::c_any_of<
                        std::is_same<UError, string>,
                        std::is_same<Error, string>
                    >::value,
                    "unknown error transformation"
                );
        }
    };

    template <typename Error = string>
    inline auto yaz_ok() noexcept {
        return ::folly::makeExpected<Error, utility::unit_t>({});
    }

    template<typename SuccessT, typename Error = string>
    inline auto yaz_ok(SuccessT&& value) noexcept {
        return ::folly::makeExpected<Error, SuccessT>(
            std::forward<SuccessT>(value)
        );
    }

    template<typename Error = string>
    inline auto yaz_fail(const Error& err) noexcept {
        if constexpr (std::is_same_v<std::string, Error>)
            return ::folly::makeUnexpected<string>(Error { err });
        else if constexpr (utility::is_c_str<Error>::value)
            return ::folly::makeUnexpected<string>(string { err });
        else
            return ::folly::makeUnexpected<Error>(Error { err });
    }

    template<typename Error = string>
    inline auto yaz_fail(Error&& err) noexcept {
        return ::folly::makeUnexpected<Error>(
            std::forward<Error>(err)
        );
    }

    template<typename... Args>
    inline auto yaz_fail_fmt(Args... args) {
        return yaz_fail<string>(do_format(
            std::forward<Args>(args)...
        ));
    }

} // end of ::yazik::detail namespace