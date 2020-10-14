#pragma once

#include <grpcpp/grpcpp.h>

#include <yazik/rpc/base/rpc_status.hpp>

#include <yazik/proto/message_formatter.hpp>

namespace yazik::rpc::grpc {

    ::grpc::Status grpc_from_rpc_status(const Result<void, RpcStatus>& sts);
    ::grpc::Status grpc_from_rpc_status(const RpcStatus& sts);
    RpcStatus rpc_from_grpc_status(const ::grpc::Status& sts);

} // end of ::yazik::rpc::grpc namespace