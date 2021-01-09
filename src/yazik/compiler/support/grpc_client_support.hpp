#pragma once

#include <yazik/compiler/support/rpc_client_support.hpp>
#include <yazik/rpc/grpc/grpc_tag_stepper.hpp>
#include <yazik/rpc/grpc/grpc_utility.hpp>
#include <yazik/rpc/grpc/grpc_scheduler.hpp>

namespace yazik::compiler::grpc_support {

    template<typename Ctx>
    class ClientControlBase;

    template<typename Ctx>
    class ClientFactoryBase {
    protected:
        using stub_t = typename Ctx::stub_t;
        using input_pb_t = typename Ctx::input_pb_t;
        using output_pb_t = typename Ctx::output_pb_t;

        std::shared_ptr<stub_t> _stub;

        ClientFactoryBase(std::shared_ptr<stub_t> stub)
        : _stub { std::move(stub) }
        {}

        ::grpc::ClientContext* grpc_ctx_of(ClientControlBase<Ctx>*);
    };

    template<typename Ctx>
    class ClientControlBase: public utility::ref_counted {
    protected:
        static constexpr auto s_handle_id = Ctx::Tag::service_name + "::" + Ctx::Tag::method_name;

        rpc::grpc::queue_scheduler_ptr_t _scheduler;
        rpc::grpc::GrpcTagStepper _stepper;
        ::grpc::ClientContext _client_context;

        ClientControlBase(rpc::grpc::queue_scheduler_ptr_t scheduler)
        : _scheduler { std::move(scheduler) }
        , _stepper { _scheduler.get() $yaz_debug(, s_handle_id.c_str()) }
        {}

        rpc::RpcTask<bool> step() noexcept {
            co_return co_await _stepper.step($yaz_debug(s_handle_id.c_str()));
        }

        friend class ClientFactoryBase<Ctx>;
    };

    template<typename Ctx>
    ::grpc::ClientContext* ClientFactoryBase<Ctx>::grpc_ctx_of(ClientControlBase<Ctx>* client) {
        return &client->_client_context;
    }

    template<typename Ctx>
    class UnaryClient;
    template<typename Ctx>
    class UnaryClientControl;

    template<typename Ctx>
    class UnaryClientFactory: public ClientFactoryBase<Ctx> {
        using Base = ClientFactoryBase<Ctx>;
        using stub_t = typename Base::stub_t;
        using input_pb_t = typename Base::input_pb_t;
        using output_pb_t = typename Base::output_pb_t;

        using response_reader_ptr_t = std::unique_ptr<::grpc::ClientAsyncResponseReader<output_pb_t>>;
        using prepare_fn_t = response_reader_ptr_t (stub_t::*)(
            ::grpc::ClientContext*,
            const input_pb_t&,
            ::grpc::CompletionQueue*
        );

        prepare_fn_t _prepare_fn;

        friend class UnaryClient<Ctx>;
        friend class UnaryClientControl<Ctx>;
        intrusive_ptr<UnaryClientControl<Ctx>> control(const rpc::grpc::queue_scheduler_ptr_t& scheduler);

        response_reader_ptr_t create_reader(
            ::grpc::ClientContext* ctx,
            const input_pb_t& request,
            ::grpc::CompletionQueue* cq
        ) {
            return ((*this->_stub.get()).*_prepare_fn)(ctx, request, cq);
        }

    protected:

        UnaryClientFactory(
            std::shared_ptr<stub_t> stub,
            prepare_fn_t prepare_fn
        )
        : Base { std::move(stub) }
        , _prepare_fn { prepare_fn }
        {}

    public:

        UnaryClient<Ctx> client(const rpc::grpc::queue_scheduler_ptr_t& scheduler);

    };

    template<typename Ctx>
    class UnaryClientControl final: public ClientControlBase<Ctx> {
        using Self = UnaryClientControl<Ctx>;
        using Base = ClientControlBase<Ctx>;
        using input_pb_t = typename Ctx::input_pb_t;
        using input_builder_t = typename Ctx::input_ref_t::builder_t;
        using output_pb_t = typename Ctx::output_pb_t;
        using output_ref_t = typename Ctx::output_ref_t;

        using response_reader_ptr_t = std::unique_ptr<::grpc::ClientAsyncResponseReader<output_pb_t>>;

        input_pb_t _input;
        output_pb_t _output;
        response_reader_ptr_t _reader;

        friend class UnaryClientFactory<Ctx>;
        friend class UnaryClient<Ctx>;

        UnaryClientFactory<Ctx>* _factory;
        rpc::RpcTask<output_ref_t> _task;

        UnaryClientControl(
            rpc::grpc::queue_scheduler_ptr_t scheduler,
            UnaryClientFactory<Ctx>* factory
        )
        : Base { std::move(scheduler) }
        , _factory { factory }
        {}

        rpc::RpcTask<output_ref_t> do_task(intrusive_ptr<Self> guard) noexcept {
            co_await this->_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));

            _reader = _factory->create_reader(
                &this->_client_context,
                _input,
                this->_scheduler->queue()
            );

            _reader->StartCall();
            ::grpc::Status sts = ::grpc::Status::OK;
            _reader->Finish(&_output, &sts, this->_stepper.tag());
            if (!co_await this->step())
                co_await rpc::RpcStatus::cancelled()
                    .as_unexpected();
            if (!sts.ok())
                co_await rpc::grpc::rpc_from_grpc_status(sts)
                    .as_unexpected();
            co_return Ctx::output_pb_spec_t::wrap(_output);
        }

        using op_ret_t = typename input_builder_t::template layered_t<rpc::RpcTask<output_ref_t>>;

        op_ret_t run() {
            _task = do_task({this});
            return Ctx::input_pb_builder_spec_t::wrap(_input).layered(&_task);
        }

    };

    template<typename Ctx>
    intrusive_ptr<UnaryClientControl<Ctx>> UnaryClientFactory<Ctx>::control(
        const rpc::grpc::queue_scheduler_ptr_t& scheduler
    ) {
        intrusive_ptr client = new  UnaryClientControl<Ctx>{ scheduler, this };
        return client;
    }

    template<typename Ctx>
    class UnaryClient {
        Ctx _ctx;
        UnaryClientFactory<Ctx>* _factory;
        rpc::grpc::queue_scheduler_ptr_t _scheduler;

    public:

        UnaryClient(
            UnaryClientFactory<Ctx>* factory,
            rpc::grpc::queue_scheduler_ptr_t scheduler
        )
        : _factory { factory }
        , _scheduler { std::move(scheduler) }
        {}

        Ctx& ctx() noexcept { return _ctx; }

        auto operator ()() {
            return _factory->control(_scheduler)->run();
        }
    };

    template<typename Ctx>
    UnaryClient<Ctx> UnaryClientFactory<Ctx>::client(
        const rpc::grpc::queue_scheduler_ptr_t& scheduler
    ) {
        return { this, scheduler };
    }

    template<typename Ctx>
    class ServerStreamingClient;
    template<typename Ctx>
    class ServerStreamingClientControl;

    template<typename Ctx>
    class ServerStreamingClientFactory: public ClientFactoryBase<Ctx> {
        using Base = ClientFactoryBase<Ctx>;
        using stub_t = typename Base::stub_t;
        using input_pb_t = typename Base::input_pb_t;
        using output_pb_t = typename Base::output_pb_t;

        using response_reader_ptr_t = std::unique_ptr<::grpc::ClientAsyncReader<output_pb_t>>;
        using prepare_fn_t = response_reader_ptr_t (stub_t::*)(
            ::grpc::ClientContext*,
            const input_pb_t&,
            ::grpc::CompletionQueue*
        );


        prepare_fn_t _prepare_fn;

        friend class ServerStreamingClient<Ctx>;
        friend class ServerStreamingClientControl<Ctx>;
        intrusive_ptr<ServerStreamingClientControl<Ctx>> control(const rpc::grpc::queue_scheduler_ptr_t& scheduler);

        response_reader_ptr_t create_reader(
            ::grpc::ClientContext* ctx,
            const input_pb_t& request,
            ::grpc::CompletionQueue* cq
        ) {
            return ((*this->_stub.get()).*_prepare_fn)(ctx, request, cq);
        }

    protected:

        ServerStreamingClientFactory(
            std::shared_ptr<stub_t> stub,
            prepare_fn_t prepare_fn
        )
        : Base { std::move(stub) }
        , _prepare_fn { prepare_fn }
        {}

    public:

        ServerStreamingClient<Ctx> client(const rpc::grpc::queue_scheduler_ptr_t& scheduler);

    };

    template<typename Ctx>
    class ServerStreamingClientControl final: public ClientControlBase<Ctx> {
        using Self = ServerStreamingClientControl<Ctx>;
        using Base = ClientControlBase<Ctx>;
        using input_pb_t = typename Ctx::input_pb_t;
        using input_builder_t = typename Ctx::input_ref_t::builder_t;
        using output_pb_t = typename Ctx::output_pb_t;
        using output_ref_t = typename Ctx::output_ref_t;

        using response_reader_ptr_t = std::unique_ptr<::grpc::ClientAsyncReader<output_pb_t>>;

        input_pb_t _input;
        response_reader_ptr_t _reader;
        std::atomic_bool _is_finished { false };
        std::optional<::grpc::Status> _finish_sts;

        friend class ServerStreamingClientFactory<Ctx>;
        friend class ServerStreamingClient<Ctx>;

        ServerStreamingClientFactory<Ctx>* _factory;
        rpc::RpcChannel<output_ref_t> _channel;

        rpc::RpcChannel<output_ref_t> do_channel(intrusive_ptr<Self> guard) noexcept {
            yaz_defer { _is_finished = false; };

            co_await this->_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));

            _reader = _factory->create_reader(
                &this->_client_context,
                _input,
                this->_scheduler->queue()
            );

            _reader->StartCall(this->_stepper.tag());
            if (!co_await step_checked())
                co_await rpc::RpcStatus::cancelled()
                    .as_broken();

            while(!_is_finished) {
                co_await this->_scheduler->ensure_on($yaz_debug(
                    Base::s_handle_id.c_str()
                ));
                auto response = output_pb_t{};
                _reader->Read(&response, this->_stepper.tag());
                if (!co_await step_checked())
                    break;
                co_yield Ctx::output_pb_spec_t::wrap(response);
            }
            co_return;
        }

        ServerStreamingClientControl(
            rpc::grpc::queue_scheduler_ptr_t scheduler,
            ServerStreamingClientFactory<Ctx>* factory
        )
        : Base { std::move(scheduler) }
        , _factory { factory }
        , _channel { do_channel({this}) }
        {}


        using op_ret_t = typename input_builder_t::template layered_t<rpc::RpcChannel<output_ref_t>>;

        op_ret_t run() {
            return Ctx::input_pb_builder_spec_t::wrap(_input).layered(&_channel);
        }

        rpc::RpcTask<> read_finish_status() noexcept {
            if (_finish_sts) {
                if (!_finish_sts->ok())
                    co_await rpc::grpc::rpc_from_grpc_status(_finish_sts.value())
                        .as_unexpected();
                co_return;
            }

            co_await this->_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));
            _finish_sts = ::grpc::Status::OK;
            _reader->Finish(&_finish_sts.value(), this->_stepper.tag());
            if (!co_await this->step())
                co_await rpc::RpcStatus::cancelled()
                    .as_unexpected();

            if (!_finish_sts->ok())
                co_await rpc::grpc::rpc_from_grpc_status(_finish_sts.value())
                    .as_unexpected();

            co_return;
        }

        rpc::RpcTask<bool> step_checked() noexcept {
            bool need_continue = co_await this->step();
            if (!need_continue)
                co_await read_finish_status();
            co_return need_continue;
        }

        rpc::RpcTask<> finish() noexcept {
            if (_is_finished.exchange(true))
                co_await rpc::RpcStatus::internal("not started")
                    .as_broken();

            co_await read_finish_status();
            co_return;
        }

    };

    template<typename Ctx>
    intrusive_ptr<ServerStreamingClientControl<Ctx>> ServerStreamingClientFactory<Ctx>::control(
        const rpc::grpc::queue_scheduler_ptr_t& scheduler
    ) {
        return new ServerStreamingClientControl<Ctx>{ scheduler, this };
    }

    template<typename Ctx>
    class ServerStreamingClient {
        Ctx _ctx;
        ServerStreamingClientFactory<Ctx>* _factory;
        rpc::grpc::queue_scheduler_ptr_t _scheduler;
        intrusive_ptr<ServerStreamingClientControl<Ctx>> _cur_control;

    public:

        ServerStreamingClient(
            ServerStreamingClientFactory<Ctx>* factory,
            rpc::grpc::queue_scheduler_ptr_t scheduler
        )
        : _factory { factory }
        , _scheduler { std::move(scheduler) }
        {}

        Ctx& ctx() noexcept { return _ctx; }

        auto operator ()() {
            _cur_control =  _factory->control(_scheduler);
            return _cur_control->run();
        }

        rpc::RpcTask<> finish() noexcept {
            if (!_cur_control)
                return rpc::RpcStatus::internal("not started")
                    .as_broken_task();
            return _cur_control->finish();
        }
    };

    template<typename Ctx>
    ServerStreamingClient<Ctx> ServerStreamingClientFactory<Ctx>::client(
        const rpc::grpc::queue_scheduler_ptr_t& scheduler
    ) {
        return { this, scheduler };
    }

    template<typename Ctx>
    class ClientStreamingClient;
    template<typename Ctx>
    class ClientStreamingClientControl;

    template<typename Ctx>
    class ClientStreamingClientFactory: public ClientFactoryBase<Ctx> {
        using Base = ClientFactoryBase<Ctx>;
        using stub_t = typename Base::stub_t;
        using input_pb_t = typename Base::input_pb_t;
        using output_pb_t = typename Base::output_pb_t;

        using request_writer_t = std::unique_ptr<::grpc::ClientAsyncWriter<input_pb_t>>;
        using prepare_fn_t = request_writer_t (stub_t::*)(
            ::grpc::ClientContext*,
            output_pb_t*,
            ::grpc::CompletionQueue*
        );

        prepare_fn_t _prepare_fn;

        friend class ClientStreamingClient<Ctx>;
        intrusive_ptr<ClientStreamingClientControl<Ctx>> control(const rpc::grpc::queue_scheduler_ptr_t& scheduler);

    protected:

        ClientStreamingClientFactory(
            std::shared_ptr<stub_t> stub,
            prepare_fn_t prepare_fn
        )
        : Base { std::move(stub) }
        , _prepare_fn { prepare_fn }
        {}

    public:

        ClientStreamingClient<Ctx> client(const rpc::grpc::queue_scheduler_ptr_t& scheduler);

    };

    template<typename Ctx>
    class ClientStreamingClientControl final: public ClientControlBase<Ctx> {
        using Self = ClientStreamingClientControl<Ctx>;
        using Base = ClientControlBase<Ctx>;
        using input_pb_t = typename Ctx::input_pb_t;
        using input_builder_t = typename Ctx::input_ref_t::builder_t;
        using output_pb_t = typename Ctx::output_pb_t;
        using output_ref_t = typename Ctx::output_ref_t;

        using request_writer_t = std::unique_ptr<::grpc::ClientAsyncWriter<input_pb_t>>;

        input_pb_t _input;
        output_pb_t _output;
        request_writer_t _writer;
        bool _is_started { false };
        std::atomic_bool _is_finished { false };
        std::optional<::grpc::Status> _finish_sts;

        using Base::Base;

        virtual ~ClientStreamingClientControl() {
            $breakpoint_hint
        }

        friend class ClientStreamingClientFactory<Ctx>;
        friend class ClientStreamingClient<Ctx>;

        void set_writer(request_writer_t writer) noexcept {
            _writer = std::move(writer);
        }

        rpc::RpcTask<> _task;
        rpc::RpcTask<> do_task(intrusive_ptr<Self> guard) noexcept {
            co_await this->_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));
            if (!_is_started) {
                _writer->StartCall(this->_stepper.tag());
                if (!co_await step_checked())
                    co_await rpc::RpcStatus::cancelled()
                        .as_broken();
                _is_started = true;
            }

            _writer->Write(_input, this->_stepper.tag());
            if (!co_await step_checked())
                co_await rpc::RpcStatus::cancelled()
                    .as_broken();
            co_return;
        }

        using op_ret_t = rpc::RpcGenerator<typename input_builder_t::template layered_t<rpc::RpcTask<>>>;

        op_ret_t run() {
            for(;;) {
                _task = do_task({this});
                _input = input_pb_t {};
                co_yield Ctx::input_pb_builder_spec_t::wrap(_input).layered(&_task);
            }
        }

        rpc::RpcTask<> read_finish_status() noexcept {
            if (_finish_sts) {
                if (!_finish_sts->ok())
                    co_await rpc::grpc::rpc_from_grpc_status(_finish_sts.value())
                        .as_unexpected();
                co_return;
            }

            co_await this->_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));
            _finish_sts = ::grpc::Status::OK;
            _writer->Finish(&_finish_sts.value(), this->_stepper.tag());
            if (!co_await this->step())
                co_await rpc::RpcStatus::cancelled()
                    .as_unexpected();

            if (!_finish_sts->ok())
                co_await rpc::grpc::rpc_from_grpc_status(_finish_sts.value())
                    .as_unexpected();

            co_return;
        }

        rpc::RpcTask<bool> step_checked() noexcept {
            bool need_continue = co_await this->step();
            if (!need_continue)
                co_await read_finish_status();
            co_return need_continue;
        }


        rpc::RpcTask<output_ref_t> finish() noexcept {
            if (_is_finished.exchange(true))
                co_await rpc::RpcStatus::internal("not started")
                    .as_broken();

            co_await this->_scheduler->ensure_on($yaz_debug(
                Base::s_handle_id.c_str()
            ));

            _writer->WritesDone(this->_stepper.tag());
            co_await step_checked();
            co_await read_finish_status();
            co_return Ctx::output_pb_spec_t::wrap(_output);
        }

    };

    template<typename Ctx>
    intrusive_ptr<ClientStreamingClientControl<Ctx>> ClientStreamingClientFactory<Ctx>::control(
        const rpc::grpc::queue_scheduler_ptr_t& scheduler
    ) {
        intrusive_ptr client = new  ClientStreamingClientControl<Ctx>{ scheduler };
        client->set_writer(
            ((*this->_stub.get()).*_prepare_fn)(
                this->grpc_ctx_of(client.get()),
                &client->_output,
                scheduler->queue()
            )
        );
        return client;
    }

    template<typename Ctx>
    class ClientStreamingClient {
        using output_ref_t = typename Ctx::output_ref_t;
        Ctx _ctx;
        ClientStreamingClientFactory<Ctx>* _factory;
        rpc::grpc::queue_scheduler_ptr_t _scheduler;
        intrusive_ptr<ClientStreamingClientControl<Ctx>> _cur_control;

    public:

        ClientStreamingClient(
            ClientStreamingClientFactory<Ctx>* factory,
            rpc::grpc::queue_scheduler_ptr_t scheduler
        )
        : _factory { factory }
        , _scheduler { std::move(scheduler) }
        {}

        Ctx& ctx() noexcept { return _ctx; }

        auto operator ()() {
            _cur_control =  _factory->control(_scheduler);
            return _cur_control->run();
        }

        rpc::RpcTask<output_ref_t> finish() noexcept {
            if (!_cur_control)
                return rpc::RpcStatus::internal("not started")
                    .as_broken_task<output_ref_t>();
            return _cur_control->finish();
        }

    };

    template<typename Ctx>
    ClientStreamingClient<Ctx> ClientStreamingClientFactory<Ctx>::client(
        const rpc::grpc::queue_scheduler_ptr_t& scheduler
    ) {
        return { this, scheduler };
    }

} // end of ::yazik::compiler::grpc_support namespace