#include <yazik/testlib/yaz_test_macros.hpp>
#include <yazik/utility/utility_defs.hpp>
#include <yazik/rpc/base/rpc_commons.hpp>

using namespace yazik;

rpc::RpcTask<> broken_task() {
    return rpc::RpcStatus::internal("fial").as_broken_task();
}

rpc::RpcTask<> ok_task() {
    return yaz_ok_t<rpc::RpcStatus>();
}

template <typename Fn>
rpc::RpcTask<> eval(Fn&& fn) {
    co_await fn();
}

YAZ_F_TEST_CASE("status as_broken test", "[rpc/status]") {
    REQUIRE_FALSE(co_await eval(broken_task).wrapped());
    REQUIRE(co_await eval(ok_task).wrapped());
}