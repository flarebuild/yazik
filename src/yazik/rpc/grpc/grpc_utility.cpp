#include "grpc_utility.hpp"

namespace yazik::rpc::grpc {

    ::grpc::Status grpc_from_rpc_status(const Result<void, RpcStatus>& sts) {
        if (sts) return ::grpc::Status::OK;
        return grpc_from_rpc_status(sts.error());
    }

    ::grpc::Status grpc_from_rpc_status(const RpcStatus& sts) {
        switch (sts.value()) {
        case RpcStatus::Status::Ok:
            return ::grpc::Status { ::grpc::StatusCode::OK, sts.to_error_string().toStdString() };
        case RpcStatus::Status::Cancelled:
            return ::grpc::Status { ::grpc::StatusCode::CANCELLED, sts.to_error_string().toStdString() };
        case RpcStatus::Status::AlreadyExist:
            return ::grpc::Status { ::grpc::StatusCode::ALREADY_EXISTS, sts.to_error_string().toStdString() };
        case RpcStatus::Status::InvalidArgument:
            return ::grpc::Status { ::grpc::StatusCode::INVALID_ARGUMENT, sts.to_error_string().toStdString() };
        case RpcStatus::Status::TimeOut:
            return ::grpc::Status { ::grpc::StatusCode::DEADLINE_EXCEEDED, sts.to_error_string().toStdString() };
        case RpcStatus::Status::NotFound:
            return ::grpc::Status { ::grpc::StatusCode::NOT_FOUND, sts.to_error_string().toStdString() };
        case RpcStatus::Status::NonAuthorized:
            return ::grpc::Status { ::grpc::StatusCode::UNAUTHENTICATED, sts.to_error_string().toStdString() };
        case RpcStatus::Status::ResourceExhausted:
            return ::grpc::Status { ::grpc::StatusCode::RESOURCE_EXHAUSTED, sts.to_error_string().toStdString() };
        case RpcStatus::Status::FailedPrecondition:
            return ::grpc::Status { ::grpc::StatusCode::FAILED_PRECONDITION, sts.to_error_string().toStdString() };
        case RpcStatus::Status::DataCorrupted:
            return ::grpc::Status { ::grpc::StatusCode::DATA_LOSS, sts.to_error_string().toStdString() };
        case RpcStatus::Status::Internal:
            return ::grpc::Status { ::grpc::StatusCode::INTERNAL, sts.to_error_string().toStdString() };
        case RpcStatus::Status::OutOfRange:
            return ::grpc::Status { ::grpc::StatusCode::OUT_OF_RANGE, sts.to_error_string().toStdString() };
        case RpcStatus::Status::Unimplemented:
            return ::grpc::Status { ::grpc::StatusCode::UNIMPLEMENTED, sts.to_error_string().toStdString() };
        case RpcStatus::Status::Unexpected:
            return ::grpc::Status { ::grpc::StatusCode::UNKNOWN, sts.to_error_string().toStdString() };
        }
    }

    RpcStatus rpc_from_grpc_status(const ::grpc::Status& sts) {
        switch (sts.error_code()) {
        case ::grpc::StatusCode::OK:
            return RpcStatus::ok();
        case ::grpc::StatusCode::CANCELLED:
            return RpcStatus{ RpcStatusEnum::Cancelled, sts.error_message() };
        case ::grpc::StatusCode::INVALID_ARGUMENT:
            return RpcStatus{ RpcStatusEnum::InvalidArgument, sts.error_message() };
        case ::grpc::StatusCode::DEADLINE_EXCEEDED:
            return RpcStatus{ RpcStatusEnum::TimeOut, sts.error_message() };
        case ::grpc::StatusCode::ALREADY_EXISTS:
            return RpcStatus{ RpcStatusEnum::AlreadyExist, sts.error_message() };
        case ::grpc::StatusCode::NOT_FOUND:
            return RpcStatus{ RpcStatusEnum::NotFound, sts.error_message() };
        case ::grpc::StatusCode::UNAUTHENTICATED:
            return RpcStatus{ RpcStatusEnum::NonAuthorized, sts.error_message() };
        case ::grpc::StatusCode::RESOURCE_EXHAUSTED:
            return RpcStatus{ RpcStatusEnum::ResourceExhausted, sts.error_message() };
        case ::grpc::StatusCode::FAILED_PRECONDITION:
            return RpcStatus{ RpcStatusEnum::FailedPrecondition, sts.error_message() };
        case ::grpc::StatusCode::DATA_LOSS:
            return RpcStatus{ RpcStatusEnum::DataCorrupted, sts.error_message() };
        case ::grpc::StatusCode::INTERNAL:
            return RpcStatus{ RpcStatusEnum::Internal, sts.error_message() };
        case ::grpc::StatusCode::OUT_OF_RANGE:
            return RpcStatus{ RpcStatusEnum::OutOfRange, sts.error_message() };
        case ::grpc::StatusCode::UNIMPLEMENTED:
            return RpcStatus{ RpcStatusEnum::Unimplemented, sts.error_message() };
        default:
            return RpcStatus{ RpcStatusEnum::Unexpected, sts.error_message() };
        }
    }

} // end of ::yazik::rpc::grpc namespace