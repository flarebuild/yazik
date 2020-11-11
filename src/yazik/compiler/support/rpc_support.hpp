#pragma once

#include <boost/di.hpp>
#include <folly/FixedString.h>

#include <yazik/compiler/support/support.hpp>
#include <yazik/rpc/base/rpc_commons.hpp>

namespace yazik::compiler::rpc_support {

    enum class RpcUnitType {
        UnarySync,
        UnaryAsync,
        ServerStreaming,
        ClientStreaming
    };

    template <
        typename Fn,
        typename Ctx
    > concept c_has_on_start = requires (
        Fn fn,
        Ctx& ctx
    ) {
        fn.on_start(ctx);
    };

    template <
        typename Fn,
        typename Ctx
    > concept c_has_on_finish = requires (
        Fn fn,
        Ctx& ctx
    ) {
        fn.on_finish(ctx, rpc::RpcStatus::cancelled());
    };

    template <typename Fn, typename Ctx, typename Req>
    concept c_identifiable_plain = requires(Fn fn, Ctx& ctx, Req& req) {
        { fn.identify(ctx, req) } -> concepts::c_same_as<void>;
    };

    template <typename Fn, typename Ctx, typename Req>
    concept c_identifiable_async = requires(Fn fn, Ctx& ctx,Req& req) {
        { fn.identify(ctx, req) } -> concepts::c_same_as<rpc::RpcTask<>>;
    };

    template <typename Fn, typename Ctx, typename Req>
    concept c_identifiable_sync = requires(Fn fn, Ctx& ctx,Req& req) {
        { fn.identify(ctx, req) } -> concepts::c_same_as<rpc::RpcResult<>>;
    };

    template <typename Fn, typename Ctx, typename Req>
    concept c_identifiable_result = c_identifiable_sync<Fn, Ctx, Req> || c_identifiable_async<Fn, Ctx, Req>;

    template <typename Fn, typename Ctx, typename Req>
    concept c_identifiable = requires(Fn fn) {
        { fn.identity() } -> concepts::c_just<string_view>;
    } && (c_identifiable_plain<Fn, Ctx, Req> || c_identifiable_result<Fn, Ctx, Req>);

} // end of ::yazik::compiler::rpc_support namespace