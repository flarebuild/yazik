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

    bool Runtime::has_ops_since_last_check() noexcept {
        return ranges::any_of(_workers, [](auto& w) {
            return w->scheduler()->has_ops_since_last_check();
        });
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
        _server->Shutdown();
        $breakpoint_hint
    }

    Future<> Runtime::wait() {
        for (auto& worker: _workers)
            worker->wait();
        _server->Wait();
        co_return;
    }



} // end of yazik::rpc::grpc namespace