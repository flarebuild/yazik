#pragma once

#include <yazik/compiler/support/rpc_support.hpp>
#include <yazik/concurrency/task.hpp>
#include <yazik/rpc/grpc/grpc_runtime_worker.hpp>
#include <yazik/rpc/grpc/grpc_tag_stepper.hpp>
#include <yazik/rpc/grpc/grpc_utility.hpp>

namespace yazik::compiler::grpc_support {

    template<typename Ctx, typename Unit>
    class PayloadBase {
    protected:
        static constexpr auto s_handle_id = Ctx::Tag::service_name + "::" + Ctx::Tag::method_name;

        rpc::grpc::server_queue_thread_scheduler_ptr_t _scheduler;
        ::yazik::rpc::grpc::GrpcTagStepper _stepper;
        ::grpc::ServerContext _grpc_ctx;
        typename Ctx::output_pb_t _response_pb;
        typename Ctx::resp_ok_t _resp_ok_tag;
        Ctx _ctx;

        PayloadBase(const rpc::grpc::server_queue_thread_scheduler_ptr_t& scheduler)
        : _scheduler { scheduler }
        , _stepper { _scheduler.get() $yaz_debug(, s_handle_id.c_str()) }
        , _ctx {
            _scheduler,
            Ctx::output_pb_builder_spec_t::wrap(_response_pb)
                .template layered<typename Ctx::resp_ok_t>(&_resp_ok_tag)
        }
        {}

    public:
        rpc::RpcTask<> step() noexcept {
            co_await _stepper.step($yaz_debug(s_handle_id.c_str()));
        }

        inline void on_start(Unit& unit) {
            if constexpr (::yazik::compiler::rpc_support::c_has_on_start<Unit, Ctx>)
                unit.on_start(_ctx);
        }

        inline void on_finish(Unit& unit, rpc::RpcStatus sts) {
            if constexpr (::yazik::compiler::rpc_support::c_has_on_finish<Unit, Ctx>)
                unit.on_finish(_ctx, sts);
        }

        rpc::RpcTask<> step_checked(Unit& unit) noexcept {
            if (co_await _stepper.step($yaz_debug(s_handle_id.c_str())))
                co_return;

            auto sts = rpc::RpcStatus::cancelled();
            on_finish(unit, sts);
            co_await sts.as_broken_task();
        }

        rpc::RpcTask<> identify(Unit& unit, auto& request) noexcept {
            if constexpr (rpc_support::c_identifiable_plain<Unit, Ctx, std::decay_t<decltype(request)>>) {
                unit.identify(_ctx, request);
            } else if constexpr (rpc_support::c_identifiable_result<Unit, Ctx, std::decay_t<decltype(request)>>) {
                co_await unit.identify(_ctx, request);
            }
            co_return;
        }

        inline auto call_unit(Unit& unit, auto&& request) noexcept {
            if constexpr(requires () { unit(_ctx, std::move(request)); }) {
                return unit(_ctx, std::move(request));
            } else if constexpr(requires () { unit.call(_ctx, std::move(request)); }) {
                return unit.call(_ctx, std::move(request));
            }
        }
    };

    template<typename Ctx, typename Unit>
    class UnaryPayloadBase
    : public PayloadBase<Ctx, Unit> {
        using Base = PayloadBase<Ctx, Unit>;
    public:
        typename Ctx::input_pb_t _request_pb;
        ::grpc::ServerAsyncResponseWriter<typename Ctx::output_pb_t> _responder;

        UnaryPayloadBase(const rpc::grpc::server_queue_thread_scheduler_ptr_t& scheduler)
        : Base { scheduler }
        , _responder { &this->_grpc_ctx }
        {}

        using request_fn_t = void (Ctx::grpc_service_t::*)(
            ::grpc::ServerContext*,
            typename Ctx::input_pb_t*,
            ::grpc::ServerAsyncResponseWriter<typename Ctx::output_pb_t>*,
            ::grpc::CompletionQueue*,
            ::grpc::ServerCompletionQueue*,
            void*
        );

        rpc::RpcTask<> request(
            typename Ctx::grpc_service_t* service,
            request_fn_t request_fn
        ) noexcept {
            co_await Base::_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));
            (service->*request_fn)(
                &this->_grpc_ctx,
                &_request_pb,
                &_responder,
                this->_scheduler->server_queue(),
                this->_scheduler->server_queue(),
                this->_stepper.tag()
            );
            co_await Base::step();
            co_return;
        }

        rpc::RpcTask<> finish(Unit& unit, rpc::RpcStatus sts) noexcept {
            Base::on_finish(unit, sts);
            co_await Base::_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));
            if (!sts.is_ok()) {
                _responder.FinishWithError(
                    rpc::grpc::grpc_from_rpc_status(sts),
                    Base::_stepper.tag()
                );
            } else {
                _responder.Finish(
                    Base::_response_pb,
                    ::grpc::Status::OK,
                    Base::_stepper.tag()
                );
            }
            co_await Base::step();
            co_return;
        }

        rpc::RpcTask<> pre_run(Unit& unit, auto& request) noexcept {
            auto identify_res = co_await Base::identify(unit, request).wrapped();
            if (!identify_res) {
                co_await finish(unit, identify_res.error());
                co_await identify_res;
            } else {
                co_await Base::_scheduler->ensure_on($yaz_debug(
                    Base::s_handle_id.c_str()
                ));
            }
            Base::on_start(unit);
            co_return;
        }
    };

    template<typename Ctx, typename Unit>
    class UnaryPayloadSync
    : public UnaryPayloadBase<Ctx, Unit> {
        using Base = UnaryPayloadBase<Ctx, Unit>;
    public:
        using Base::Base;

        rpc::RpcTask<> run(Unit& unit) noexcept {
            auto request = Ctx::input_pb_spec_t::wrap(Base::_request_pb);
            co_await Base::pre_run(unit, request);
            auto unit_res = Base::call_unit(unit, std::move(request));
            if (!unit_res) {
                co_await Base::finish(unit, unit_res.error());
                co_await unit_res;
            } else {
                co_await Base::finish(unit, rpc::RpcStatus::ok());
            }
            co_return;
        }
    };

    template<typename Ctx, typename Unit>
    class UnaryPayloadAsync
    : public UnaryPayloadBase<Ctx, Unit> {
        using Base = UnaryPayloadBase<Ctx, Unit>;
    public:
        using Base::Base;

        rpc::RpcTask<> run(Unit& unit) noexcept {
            auto request = Ctx::input_pb_spec_t::wrap(Base::_request_pb);
            co_await Base::pre_run(unit, request);
            auto unit_res = co_await Base::call_unit(unit, std::move(request)).wrapped();
            if (!unit_res) {
                co_await Base::finish(unit, unit_res.error());
                co_await unit_res;
            } else {
                co_await Base::finish(unit, rpc::RpcStatus::ok());
            }
            co_return;
        }
    };

    template<typename Ctx, typename Unit>
    class ServerStreamingPayloadBase
    : public PayloadBase<Ctx, Unit> {
        using Base = PayloadBase<Ctx, Unit>;
    protected:
        typename Ctx::input_pb_t _request_pb;
        ::grpc::ServerAsyncWriter<typename Ctx::output_pb_t> _responder;

    public:
        ServerStreamingPayloadBase(
            const rpc::grpc::server_queue_thread_scheduler_ptr_t& scheduler
        )
        : Base { scheduler }
        , _responder { &this->_grpc_ctx }
        {}

        using request_fn_t = void (Ctx::grpc_service_t::*)(
            ::grpc::ServerContext*,
            typename Ctx::input_pb_t*,
            ::grpc::ServerAsyncWriter<typename Ctx::output_pb_t>*,
            ::grpc::CompletionQueue*,
            ::grpc::ServerCompletionQueue*,
            void*
        );

        rpc::RpcTask<> request(
            typename Ctx::grpc_service_t* service,
            request_fn_t request_fn
        ) noexcept {
            co_await Base::_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));
            (service->*request_fn)(
                &this->_grpc_ctx,
                &_request_pb,
                &_responder,
                this->_scheduler->server_queue(),
                this->_scheduler->server_queue(),
                this->_stepper.tag()
            );
            co_await Base::step();
            co_return;
        }

        rpc::RpcTask<> finish(Unit& unit, rpc::RpcStatus sts, bool write_last) {
            Base::on_finish(unit, sts);
            co_await Base::_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));
            if (!sts.is_ok()) {
                _responder.Finish(
                    rpc::grpc::grpc_from_rpc_status(sts),
                    Base::_stepper.tag()
                );
            } else if (write_last) {
                _responder.WriteAndFinish(
                    Base::_response_pb,
                    ::grpc::WriteOptions{},
                    ::grpc::Status::OK,
                    Base::_stepper.tag()
                );
            } else {
                _responder.Finish(
                    ::grpc::Status::OK,
                    Base::_stepper.tag()
                );
            }
            co_await Base::step();
            co_return;
        }

        rpc::RpcTask<> pre_run(Unit& unit, auto& request) noexcept {
            auto identify_res = co_await Base::identify(unit, request).wrapped();
            if (!identify_res) {
                co_await finish(unit, identify_res.error(), false);
                co_await identify_res;
            } else {
                co_await Base::_scheduler->ensure_on($yaz_debug(
                    Base::s_handle_id.c_str()
                ));
            }
            Base::on_start(unit);
            co_return;
        }
    };

    template<typename Ctx, typename Unit>
    class ServerStreamingPayloadSync
    : public ServerStreamingPayloadBase<Ctx, Unit> {
        using Base = ServerStreamingPayloadBase<Ctx, Unit>;
    public:
        using Base::Base;

        rpc::RpcTask<> run(Unit& unit) noexcept {
            auto request = Ctx::input_pb_spec_t::wrap(Base::_request_pb);
            co_await Base::pre_run(unit, request);
            auto sync_stream = Base::call_unit(unit, std::move(request));
            for (auto&& _: sync_stream) {
                Base::_responder.Write(Base::_response_pb, Base::_stepper.tag());
                co_await Base::step_checked(unit);
            }
            auto sync_stream_res = sync_stream.result();
            if (!sync_stream_res) {
                co_await Base::finish(unit, sync_stream_res.error(), false);
                co_await sync_stream_res;
            } else {
                co_await Base::finish(unit, rpc::RpcStatus::ok(), false);
            }
            co_return;
        }
    };

    template<typename Ctx, typename Unit>
    class ServerStreamingPayloadAsync
    : public ServerStreamingPayloadBase<Ctx, Unit> {
        using Base = ServerStreamingPayloadBase<Ctx, Unit>;

        rpc::RpcTask<> process_async_stream(Unit& unit, auto&& async_stream) noexcept {
            for co_await(auto&& _: async_stream) {
                co_await Base::_scheduler->ensure_on($yaz_debug(
                    Base::s_handle_id.c_str()
                ));
                Base::_responder.Write(Base::_response_pb, Base::_stepper.tag());
                co_await Base::step_checked(unit);
            }
            if (!async_stream.status().is_ok())
                co_await async_stream.status().as_broken_task();

            co_return;
        }

    public:
        using Base::Base;

        rpc::RpcTask<> run(Unit& unit) noexcept {
            auto request = Ctx::input_pb_spec_t::wrap(Base::_request_pb);
            co_await Base::pre_run(unit, request);
            auto async_stream = Base::call_unit(unit, std::move(request));
            if (auto result = async_stream.one_result_only()) {
                if (*result) {
                    co_await Base::finish(unit, rpc::RpcStatus::ok(), true);
                } else {
                    co_await Base::finish(unit, result->error(), false);
                }
            } else {
                auto process_stream_res = co_await process_async_stream(unit, std::move(async_stream)).wrapped();
                if (!process_stream_res) {
                    co_await Base::finish(unit, process_stream_res.error(), false);
                } else {
                    co_await Base::finish(unit, rpc::RpcStatus::ok(), false);
                }
            }
            co_return;
        }
    };

    template<typename Ctx, typename Unit>
    class ClientStreamingPayloadBase
    : public PayloadBase<Ctx, Unit> {
        using Base = PayloadBase<Ctx, Unit>;
    protected:
        typename Ctx::input_pb_t _request;
        ::grpc::ServerAsyncReader<typename Ctx::output_pb_t, typename Ctx::input_pb_t> _responder;
        std::atomic_bool _finished { false };

    public:
        ClientStreamingPayloadBase(
            const rpc::grpc::server_queue_thread_scheduler_ptr_t& scheduler
        )
        : Base { scheduler }
        , _responder { &this->_grpc_ctx }
        {}

        using request_fn_t = void (Ctx::grpc_service_t::*)(
            ::grpc::ServerContext*,
            ::grpc::ServerAsyncReader<typename Ctx::output_pb_t, typename Ctx::input_pb_t>*,
            ::grpc::CompletionQueue*,
            ::grpc::ServerCompletionQueue*,
            void*
        );

        rpc::RpcTask<> request(
            typename Ctx::grpc_service_t* service,
            request_fn_t request_fn
        ) noexcept {
            co_await Base::_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));
            (service->*request_fn)(
                &this->_grpc_ctx,
                &_responder,
                this->_scheduler->server_queue(),
                this->_scheduler->server_queue(),
                this->_stepper.tag()
            );
            co_await Base::step();
            co_return;
        }

        rpc::RpcTask<> finish(Unit& unit, rpc::RpcStatus sts) {
            _finished = true;
            Base::on_finish(unit, sts);
            co_await Base::_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));
            if (!sts.is_ok()) {
                _responder.FinishWithError(
                    rpc::grpc::grpc_from_rpc_status(sts),
                    Base::_stepper.tag()
                );
            } else {
                _responder.Finish(
                    Base::_response_pb,
                    ::grpc::Status::OK,
                    Base::_stepper.tag()
                );
            }
            co_await Base::step();
            co_return;
        }

        rpc::RpcTask<> pre_run(Unit& unit, auto& request) noexcept {
            auto identify_res = co_await Base::identify(unit, request).wrapped();
            if (!identify_res) {
                co_await finish(unit, identify_res.error());
                co_await identify_res;
            } else {
                co_await Base::_scheduler->ensure_on($yaz_debug(
                    Base::s_handle_id.c_str()
                ));
            }
            Base::on_start(unit);
            co_return;
        }

        rpc::RpcChannel<typename Ctx::input_ref_t> create_request_stream(Unit& unit) {
            while (!_finished) {
                co_await Base::_scheduler->ensure_on($yaz_debug(
                    Base::s_handle_id.c_str()
                ));
                typename Ctx::input_pb_t _request_pb;
                _responder.Read(&_request_pb, Base::_stepper.tag());
                co_await Base::step_checked(unit);
                co_yield Ctx::input_pb_spec_t::wrap(_request_pb);
            }
            co_return;
        }
    };

    template<typename Ctx, typename Unit>
    class ClientStreamingPayloadSync
    : public ClientStreamingPayloadBase<Ctx, Unit> {
        using Base = ClientStreamingPayloadBase<Ctx, Unit>;

        rpc::RpcTask<vector<typename Ctx::input_pb_t>> stream_to_vector(
            rpc::RpcChannel<typename Ctx::input_ref_t>&& request
        ) {
            vector<typename Ctx::input_pb_t> result;
            for co_await(auto ref: request) {
                request.emplace_back();
                Ctx::input_pb_builder_spec_t::wrap(request.back())
                    .move_initialize_from(std::move(ref));
            }
            if (!request.status().is_ok())
                co_await request.status().as_broken_task();

            co_return std::move(result);
        }

        rpc::RpcGenerator<typename Ctx::input_ref_t> vector_to_generator(
            vector<typename Ctx::input_pb_t>&& data
        ) {
            for (auto& req: data)
                co_yield Ctx::input_pb_spec_t::wrap(req);
            co_return;
        }

    public:
        using Base::Base;

        rpc::RpcTask<> run(Unit& unit) noexcept {
            auto request = this->create_request_stream(unit);
            co_await Base::pre_run(unit, request);
            auto stream_to_vector_res = stream_to_vector(std::move(request)).wrapped();
            if (!stream_to_vector_res) {
                co_await Base::finish(unit, stream_to_vector_res.error());
                co_await stream_to_vector_res;
            }
            auto unit_res = co_await Base::call_unit(
                unit,
                vector_to_generator(std::move(stream_to_vector_res.value()))
            )();
            if (!unit_res) {
                co_await Base::finish(unit, unit_res.error());
                co_await unit_res;
            } else {
                co_await Base::finish(unit, rpc::RpcStatus::ok());
            }
            co_return;
        }
    };

    template<typename Ctx, typename Unit>
    class ClientStreamingPayloadAsync
    : public ClientStreamingPayloadBase<Ctx, Unit> {
        using Base = ClientStreamingPayloadBase<Ctx, Unit>;
    public:
        using Base::Base;

        rpc::RpcTask<> run(Unit& unit) noexcept {
            auto request = this->create_request_stream(unit);
            co_await Base::pre_run(unit, request);
            auto unit_res = co_await Base::call_unit(unit, std::move(request)).wrapped();
            if (!unit_res) {
                co_await Base::finish(unit, unit_res.error());
                co_await unit_res;
            } else {
                co_await Base::finish(unit, rpc::RpcStatus::ok());
            }
            co_return;
        }
    };

} // end of ::yazik::compiler::grpc_support namespace