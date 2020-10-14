#include "grpc_runtime.hpp"
#include <yazik/rpc/grpc/grpc_runtime_worker.hpp>

namespace yazik::rpc::grpc {

    std::atomic_size_t Runtime::s_service_id { 0 };

    Runtime::Runtime() {
        _builder.emplace();
    }

    ::grpc::ServerBuilder& Runtime::builder() {
        return _builder.value();
    }

    const std::shared_ptr<RuntimeWorker>& Runtime::add_worker() {
        auto worker = std::shared_ptr<RuntimeWorker>(new RuntimeWorker{
            this,
            _builder->AddCompletionQueue()
        });
        _workers.push_back(std::move(worker));
        return _workers.back();
    }

    Future<> Runtime::start() {
        _server = _builder->BuildAndStart();
        _builder.reset();

        for (auto& worker: _workers)
            co_await worker->start();
        co_return;
    }

    void Runtime::stop() {
        for (auto& worker: _workers)
            worker->stop();
    }

    Future<> Runtime::wait() {
//        _server->Wait();
        for (auto& worker: _workers)
            co_await worker->wait();

        _server->Shutdown();
        co_return;
    }



} // end of yazik::rpc::grpc namespace