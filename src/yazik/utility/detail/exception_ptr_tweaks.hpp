#pragma once

#include <yazik/utility/detail/result_detail.hpp>

namespace yazik::detail {

    template <typename Error, typename Fn>
    Error extract_error(Fn&& fn) try {
        fn();
        return {};
    } catch (std::exception_ptr e) {
        return extract_error<Error>(std::bind(std::rethrow_exception, e));
    } catch (const Error& e) {
        return e;
    } catch (const ErrorWrapper<Error>& e) {
        return e;
    } catch (const std::exception& e) {
        return detail::result_error_from_str<Error>(e.what());
    } catch (...) {
        return detail::result_error_from_str<Error>(
            "unhandled_exception: unknown exception"
        );
    }

    inline ::boost::exception_ptr convert_std_ex_to_boost(std::exception_ptr ex) try {
        throw boost::enable_current_exception(ex);
    } catch (...) {
        return boost::current_exception();
    }


} // end of ::yazik::detail namespace