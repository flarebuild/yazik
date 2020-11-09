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

} // end of ::yazik::compiler::rpc_support namespace