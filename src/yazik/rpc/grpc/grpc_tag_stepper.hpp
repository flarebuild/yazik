#pragma once

#include <grpcpp/alarm.h>

#include <yazik/rpc/base/rpc_status.hpp>

#include <yazik/rpc/grpc/grpc_scheduler.hpp>
#include <yazik/rpc/grpc/grpc_utility.hpp>

namespace yazik::rpc::grpc {

    class GrpcTagStepper {
        GrpcQueueScheduler* _scheduler;
        GrpcQueueTag _tag;

        ::grpc::Alarm _alarm;

        GrpcQueueTag::GrpcQueueTagOp step_impl(GrpcQueueTag& tag $yaz_debug(, const char* descr));

    public:

        GrpcTagStepper(GrpcQueueScheduler* scheduler $yaz_debug(, const char* descr));

        void* tag();
        GrpcQueueTag::GrpcQueueTagOp step($yaz_debug(const char* tag));
        GrpcQueueTag::GrpcQueueTagOp step_ensure_queue_back($yaz_debug(const char* tag));
        ::grpc::CompletionQueue* queue();
        GrpcQueueScheduler* scheduler();
    };

    class GrpcServerTagStepper
    : public GrpcTagStepper {
    public:
        using GrpcTagStepper::GrpcTagStepper;
        ::grpc::ServerCompletionQueue* server_queue();
    };

} // end of ::yazik::rpc::grpc namespace