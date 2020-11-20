#include "coro_scope.hpp"

namespace yazik::concurrency {

    Task<> CoroScope::join(concurrency::Executor* ex) {
        co_await _barrier.wait_until_published(_size, ex);
        co_return;
    }

    CoroDefer::CoroDefer(concurrency::executor_ptr_t ex)
    : _ex { std::move(ex) }
    {}

    CoroDefer::CoroDefer(CoroDefer&& other) {
        _ex = std::move(other._ex);
        _fn = std::move(other._fn);
        other._ex = nullptr;
        other._fn = nullptr;
    }

    CoroDefer::~CoroDefer() {
        [[maybe_unused]] auto _ = defer();
    }

    Future<> CoroDefer::defer() {
        if (!_fn) co_return;

        auto fn = std::move(_fn);
        co_await _ex->ensure_on(YAZ_LOCATION_STR);
        fn();
        co_return;
    }

    CoroDefer CoroDefer::apply(continuation_t&& continuation) {
        _fn = std::move(continuation);
        return std::move(*this);
    }
} // end of ::yazik::concurrency namespace