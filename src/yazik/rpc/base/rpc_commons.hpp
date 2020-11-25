#pragma once

#include <yazik/rpc/base/rpc_status.hpp>
#include <yazik/utility/concepts.hpp>
#include <yazik/concurrency/channel.hpp>
#include <yazik/concurrency/generator.hpp>
#include <yazik/concurrency/scheduler.hpp>

namespace yazik::rpc {

    template<typename T = void>
    using RpcResult = Result<T, RpcStatus>;

    template<typename T>
    using RpcResultIPtr = ResultIPtr<T, RpcStatus>;

    template<typename T>
    using RpcResultSPtr = ResultSPtr<T, RpcStatus>;

    template<typename T = void>
    using RpcTask = Task<T, RpcStatus>;

     template<typename T>
    using RpcTaskIPtr = TaskIPtr<T, RpcStatus>;

    template<typename T>
    using RpcGenerator = Generator<T, RpcStatus>;

    template<typename T>
    using RpcChannel = Channel<T, RpcStatus>;

    template<typename T>
    concept c_request_context = requires (T* a) {
        { T::tag };
        { T::Tag::service_name.c_str() } -> concepts::c_same_as<const char*>;
        { T::Tag::method_name.c_str() } -> concepts::c_same_as<const char*>;
        { a->scheduler } -> concepts::c_just<const concurrency::scheduler_ptr_t&>;
        { a->response.done() } -> concepts::c_same_as<typename T::Tag>;
    };

    template<
        typename Fn,
        typename Req,
        typename Res,
        typename Ctx
    > concept c_unary_sync_unit = requires (
        Fn fn,
        Ctx& ctx,
        Req req
    ) {
        { fn(ctx, std::move(req)) } -> concepts::c_same_as<RpcResult<typename Ctx::Tag>>;
    } && c_request_context<Ctx>;

    template<
        typename Fn,
        typename Req,
        typename Res,
        typename Ctx
    > concept c_unary_async_unit = requires (
        Fn fn,
        Ctx& ctx,
        Req req
    ) {
        { fn(ctx, std::move(req)) } -> concepts::c_same_as<RpcTask<typename Ctx::Tag>>;
    } && c_request_context<Ctx>;

    template<
        typename Fn,
        typename Req,
        typename Res,
        typename Ctx
    > concept c_stream_reader_unit = requires (
        Fn fn,
        Ctx& ctx,
        RpcChannel<Req> stream
    ) {
        { fn(ctx, std::move(stream)) } -> concepts::c_same_as<RpcTask<typename Ctx::Tag>>;
    } && c_request_context<Ctx>;

    template<
        typename Fn,
        typename Req,
        typename Res,
        typename Ctx
    > concept c_stream_writer_unit = requires (
        Fn fn,
        Ctx& ctx,
        Req req
    ) {
        { fn(ctx, std::move(req)) } -> concepts::c_same_as<RpcChannel<typename Ctx::Tag>>;
    } && c_request_context<Ctx>;

    template<
        typename Fn,
        typename Req,
        typename Res,
        typename Ctx
    > concept c_rpc_unit = c_unary_sync_unit<Fn, Req, Res, Ctx>
                      || c_unary_async_unit<Fn, Req, Res, Ctx>
                      || c_stream_reader_unit<Fn, Req, Res, Ctx>
                      || c_stream_writer_unit<Fn, Req, Res, Ctx>;

} // end of ::yazik::rpc namespace