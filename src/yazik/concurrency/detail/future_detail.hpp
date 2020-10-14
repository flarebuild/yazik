#pragma once

#include <portable_concurrency/future>

#include <yazik/utility/detail/exception_ptr_tweaks.hpp>

namespace yazik::detail {

    template<typename T, typename Error>
    inline Result<T, Error> unwrap_future(::pc::future<T>& f) try {
        if constexpr (std::is_same_v<T, void>) {
            f.get();
            return yaz_ok<Error>();
        } else {
            return f.get();
        }
    } catch (std::exception_ptr e) {
        return yaz_fail<Error>(extract_error<Error>(std::bind(std::rethrow_exception, e)));
    } catch (const Error& e) {
        return yaz_fail<Error>(e);
    } catch (const ErrorWrapper<Error>& e) {
        return yaz_fail<Error>(e);
    } catch (const std::exception& e) {
        return yaz_fail<Error>(detail::result_error_from_str<Error>(e.what()));
    } catch (...) {
        return yaz_fail<Error>(detail::result_error_from_str<Error>(
            "unhandled_exception: unknown exception"
        ));
    }

} // end of ::yazik::detail namespace