#include "coro_scope.hpp"

namespace yazik::concurrency {

    Task<> CoroScope::join(concurrency::Executor* ex) {
        co_await _barrier.wait_until_published(_size, ex);
        co_return;
    }

} // end of ::yazik::concurrency namespace