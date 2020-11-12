#pragma once

#include <yazik/compiler/support/rpc_support.hpp>
#include <yazik/concurrency/task.hpp>
#include <yazik/rpc/grpc/grpc_runtime_worker.hpp>
#include <yazik/rpc/grpc/grpc_tag_stepper.hpp>
#include <yazik/rpc/grpc/grpc_utility.hpp>

namespace yazik::compiler::grpc_support {

    template<
        typename Entity,
        typename EntityPbSpec,
        typename PbRequest,
        typename PbResponse
    > rpc::RpcChannel<Entity> create_request_stream(
        ::yazik::rpc::grpc::GrpcTagStepper& stepper,
        ::grpc::ServerAsyncReader<PbResponse, PbRequest>& reader
        $yaz_debug(, const char* tag)
    ) {
        for (;;) {
            PbRequest request_msg{};
            reader.Read(&request_msg, stepper.tag());
            if (!(co_await stepper.step($yaz_debug(tag)))) {
                break;
            }
            co_yield EntityPbSpec::wrap(request_msg);
        }
        co_return;
    }

} // end of ::yazik::compiler::grpc_support namespace