#include "grpc_tag_stepper.hpp"

namespace yazik::rpc::grpc {

    GrpcTagStepper::GrpcTagStepper(GrpcQueueScheduler* scheduler $yaz_debug(, const char* descr))
    : _scheduler { scheduler }
     $yaz_debug(, _tag { descr })
    {}

    void* GrpcTagStepper::tag() {
        return _tag.self_tagged();
    }

    GrpcQueueTag::GrpcQueueTagOp GrpcTagStepper::step_impl(GrpcQueueTag& tag $yaz_debug(, const char* descr)) {
        $yaz_debug( tag.set_descr(descr);)
        return _scheduler->wait_for_tag(tag);
    }

    GrpcQueueTag::GrpcQueueTagOp GrpcTagStepper::step($yaz_debug(const char* tag)) {
        return step_impl(_tag $yaz_debug(, tag));
    }

    GrpcQueueTag::GrpcQueueTagOp GrpcTagStepper::step_ensure_queue_back($yaz_debug(const char* tag)) {
        _alarm.Set(
            queue(),
            gpr_now(gpr_clock_type::GPR_CLOCK_REALTIME),
            _tag.self_tagged()
        );
        return step_impl(_tag $yaz_debug(, tag));
    }

    ::grpc::CompletionQueue* GrpcTagStepper::queue() {
        return _scheduler->queue();
    }

    GrpcQueueScheduler* GrpcTagStepper::scheduler() {
        return _scheduler;
    }

    ::grpc::ServerCompletionQueue* GrpcServerTagStepper::server_queue() {
        return static_cast<::grpc::ServerCompletionQueue*>(queue());
    }

} // end of ::yazik::rpc::grpc namespace