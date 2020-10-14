#pragma once

#include <grpcpp/grpcpp.h>
#include <yazik/utility/utility_defs.hpp>
#include <yazik/concurrency/future_result.hpp>
#include <grpcpp/impl/codegen/service_type.h>

namespace yazik::rpc::grpc {

    class RuntimeWorker;
    class Runtime {
        std::optional<::grpc::ServerBuilder> _builder;
        std::unique_ptr<::grpc::Server> _server;
        vector<std::shared_ptr<RuntimeWorker>> _workers;

        static std::atomic_size_t s_service_id;

        template<typename T>
        static std::size_t get_service_id() {
            static std::size_t s_id = ++s_service_id;
            return s_id;
        }

        std::mutex _registered_services_mutex;
        std::unordered_map<std::size_t, std::unique_ptr<::grpc::Service>> _registered_services;

    public:
        Runtime();

        ::grpc::ServerBuilder& builder();
        const std::shared_ptr<RuntimeWorker>& add_worker();

        template<typename Service>
        Service* service() {
            std::size_t id = get_service_id<Service>();
            auto it = _registered_services.find(id);
            if (it == _registered_services.end()) {
                auto lock = std::unique_lock { _registered_services_mutex };
                it = _registered_services.find(id);
                if (it == _registered_services.end()) {
                    auto* service = new Service;
                    it = _registered_services.emplace(id, service).first;
                    _builder->RegisterService(service);
                }
            }
            return (Service*)it->second.get();
        }

        Future<> start();
        void stop();
        Future<> wait();
    };

} // end of yazik::rpc::grpc namespace