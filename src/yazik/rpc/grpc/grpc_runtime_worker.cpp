#include "grpc_runtime_worker.hpp"

namespace yazik::rpc::grpc {
    
    RuntimeWorker::RuntimeWorker(
        Runtime* runtime,
        std::unique_ptr<::grpc::ServerCompletionQueue>&& cq
    )
    : _runtime { runtime }
    , _scheduler { new rpc::grpc::GrpcServerQueueThreadScheduler { std::move(cq) } }
    {}

    Future<> RuntimeWorker::start() {
        for (auto& unit: _units)
            unit();
        co_await _scheduler->start();
        co_await _scheduler->on(YAZ_LOCATION_STR);
        co_return;
    }

    void RuntimeWorker::stop() {
        _scheduler->stop();
    }

    Future<> RuntimeWorker::wait() {
        return _scheduler->on_stop();
    }

    concurrency::scheduler_ptr_t RuntimeWorker::scheduler() noexcept {
        return _scheduler;
    }

} // end of yazik::rpc::grpc namespace