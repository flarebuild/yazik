#include <yazik/utility/lambda_continuation.hpp>

namespace yazik {
namespace utility {

    OnceImpl::OnceImpl(std::once_flag& once_flag): _once_flag{once_flag} {}
    Result<void> OnceImpl::apply(continuation_t&& continuation) {
        Result<void> result = yaz_ok();
        std::call_once(
            _once_flag,
            [&result, _l_move(continuation)] {
                result = continuation();
            }
        );
        return result;
    }


} // end of ::yazik::utility namespace
} // end of ::yazik namespace