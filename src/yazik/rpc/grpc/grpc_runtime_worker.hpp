#pragma once

#include <yazik/utility/macro.hpp>
#include <yazik/rpc/base/rpc_commons.hpp>
#include <yazik/rpc/grpc/grpc_scheduler.hpp>
#include <yazik/rpc/grpc/grpc_runtime.hpp>

namespace yazik::rpc::grpc {

    class RuntimeWorker {
        Runtime* _runtime;
        server_queue_thread_scheduler_ptr_t _scheduler;
        vector<unique_function<void()>> _units;

        RuntimeWorker(
            Runtime* runtime,
            std::unique_ptr<::grpc::ServerCompletionQueue>&& cq
        );

        friend class Runtime;

        Future<> start();
        void stop();
        void wait();

    public:

        queue_scheduler_ptr_t scheduler() noexcept;

        template <
            template <class> typename Handle,
            typename RpcUnit
        > void spawn(RpcUnit unit) {
            using handle_t = Handle<RpcUnit>;
            auto* service = _runtime->service<typename handle_t::service_t>();
            _units.push_back([this, _l_move(unit), service]{
                handle_t::spawn(
                    std::move(unit),
                    service,
                    _scheduler
                );
            });
        }

        template<typename Handle>
        void spawn_handle(Handle&& handle) {
            auto* service = _runtime->service<typename Handle::service_t>();
            _units.push_back([this, unit = std::move(handle.unit), service]{
                Handle::spawn(
                    std::move(unit),
                    service,
                    _scheduler
                );
            });
        }

    };
    
} // end of yazik::rpc::grpc namespace