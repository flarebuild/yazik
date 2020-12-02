#pragma once

#include <yazik/rpc/base/rpc_status.hpp>
#include <yazik/utility/concepts.hpp>
#include <yazik/concurrency/channel.hpp>
#include <yazik/concurrency/generator.hpp>
#include <yazik/concurrency/scheduler.hpp>

namespace yazik::rpc {

    enum class RpcUnitType {
        UnarySync,
        UnaryAsync,
        ServerStreamingSync,
        ServerStreamingAsync,
        ClientStreamingSync,
        ClientStreamingAsync
    };

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
    struct RespOk {};

    template<typename T, typename Resp>
    concept c_request_context = requires (T* a) {
        { T::tag };
        { T::Tag::service_name.c_str() } -> concepts::c_same_as<const char*>;
        { T::Tag::method_name.c_str() } -> concepts::c_same_as<const char*>;
        { a->scheduler } -> concepts::c_just<const concurrency::scheduler_ptr_t&>;
        { a->response.done() } -> concepts::c_same_as<RespOk<Resp>>;
    };

    template<typename Fn, typename Req, typename Res, typename Ctx>
    concept c_rpc_callable_op = requires (Fn fn, Ctx& ctx, Req req) {
        { fn(ctx, std::move(req)) } -> concepts::c_same_as<Res>;
    };
    template<typename Fn, typename Req, typename Res, typename Ctx>
    concept c_rpc_callable_call = requires (Fn fn, Ctx& ctx, Req req) {
        { fn.call(ctx, std::move(req)) } -> concepts::c_same_as<Res>;
    };

    template<typename Fn, typename Req, typename Res, typename Ctx>
    concept c_rpc_callable = c_rpc_callable_op<Fn, Req, Res, Ctx>
                          || c_rpc_callable_call<Fn, Req, Res, Ctx>;

    template<typename Fn, typename Req, typename Res, typename Ctx>
    concept c_unary_sync_unit = c_rpc_callable<Fn, Req, RpcResult<RespOk<Res>>, Ctx>
                             && c_request_context<Ctx, Res>;

    template<typename Fn, typename Req, typename Res, typename Ctx>
    concept c_unary_async_unit = c_rpc_callable<Fn, Req, RpcTask<RespOk<Res>>, Ctx>
                              && c_request_context<Ctx, Res>;

    template<typename Fn, typename Req, typename Res, typename Ctx>
    concept c_unary_unit = c_unary_sync_unit<Fn, Req, Res, Ctx>
                        || c_unary_async_unit<Fn, Req, Res, Ctx>;

    template<typename Fn, typename Req, typename Res, typename Ctx>
    concept c_stream_reader_sync_unit = c_rpc_callable<Fn, RpcGenerator<Req>, RpcResult<RespOk<Res>>, Ctx>
                                     && c_request_context<Ctx, Res>;

    template<typename Fn, typename Req, typename Res, typename Ctx>
    concept c_stream_reader_async_unit = c_rpc_callable<Fn, RpcChannel<Req>, RpcTask<RespOk<Res>>, Ctx>
                                      && c_request_context<Ctx, Res>;

    template<typename Fn, typename Req, typename Res, typename Ctx>
    concept c_stream_reader_unit = c_stream_reader_sync_unit<Fn, Req, Res, Ctx>
                                || c_stream_reader_async_unit<Fn, Req, Res, Ctx>;

    template<typename Fn, typename Req, typename Res,typename Ctx>
    concept c_stream_writer_sync_unit = c_rpc_callable<Fn, Req, RpcGenerator<RespOk<Res>>, Ctx>
                                     && c_request_context<Ctx, Res>;

    template<typename Fn, typename Req, typename Res,typename Ctx>
    concept c_stream_writer_async_unit = c_rpc_callable<Fn, Req, RpcChannel<RespOk<Res>>, Ctx>
                                      && c_request_context<Ctx, Res>;

    template<typename Fn, typename Req, typename Res,typename Ctx>
    concept c_stream_writer_unit = c_stream_writer_sync_unit<Fn, Req, Res, Ctx>
                                || c_stream_writer_async_unit<Fn, Req, Res, Ctx>;

    template<typename Fn, typename Req, typename Res, typename Ctx>
    concept c_rpc_unit = c_unary_unit<Fn, Req, Res, Ctx>
                      || c_stream_reader_unit<Fn, Req, Res, Ctx>
                      || c_stream_writer_unit<Fn, Req, Res, Ctx>;

} // end of ::yazik::rpc namespace