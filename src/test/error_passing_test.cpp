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
    REQUIRE((co_await eval(broken_task).wrapped()).hasError());
    REQUIRE((co_await eval(ok_task).wrapped()).hasValue());
    co_return;
}

rpc::RpcGenerator<bool> broken_generator() {
    return rpc::RpcStatus::internal("fial")
        .as_broken_generator<bool>();
}

rpc::RpcGenerator<bool> ok_generator() {
    co_yield true;
    co_return;
}

template <typename Fn>
rpc::RpcResult<> eval_gen(Fn&& fn) {
    auto gen = fn();
    for (auto _: gen) {}
    co_await gen.result();
    co_return;
}

TEST_CASE("status as_broken_generator test", "[rpc/status]") {
    REQUIRE(eval_gen(broken_generator).hasError());
    REQUIRE(eval_gen(ok_generator).hasValue());
}

rpc::RpcChannel<bool> broken_channel() {
    return rpc::RpcStatus::internal("fial")
        .as_broken_channel<bool>();
}

rpc::RpcChannel<bool> ok_channel() {
    co_yield true;
    co_return;
}

template <typename Fn>
rpc::RpcTask<> eval_channel(Fn&& fn) {
    auto gen = fn();
    for co_await(auto _: gen) {}
    co_return;
}

YAZ_F_TEST_CASE("status as_broken_channel test", "[rpc/status]") {
    auto channel_res = co_await eval_channel(broken_channel).wrapped();
    REQUIRE(channel_res.hasError());
    REQUIRE(channel_res.error().to_error_string() == "internal: fial");

    channel_res = co_await eval_channel(ok_channel).wrapped();
    REQUIRE(channel_res.hasValue());
    co_return;
}