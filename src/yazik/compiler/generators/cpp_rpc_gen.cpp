#include "cpp_rpc_gen.hpp"
#include <google/protobuf/descriptor.pb.h>
#include <yazik/compiler/utility/writer.hpp>
#include <yazik/compiler/formatters/yaz_formatter.hpp>

namespace yazik::compiler {

    class CppGrpcGenerator final
    : public Generator {

        Result<> gen_yaz_rpc_header(
            const gp::FileDescriptor* file,
            gpc::GeneratorContext* generator_context
        ) const {
            auto w = FileWriter { file, generator_context, "yaz.rpc.h" };
            auto f = Formatter { file, "" };

            w.wl("#pragma once");
            w.l();
            w.wl("#include {}", ab("yazik/compiler/support/rpc_support.hpp"));
            w.wl("#include {}", ab(replace_proto(file->name(), "yaz.h")));
            for (int d = 0; d < file->dependency_count(); ++d) {
                auto* dep = file->dependency(d);
                if (dep->syntax() != gp::FileDescriptor::SYNTAX_PROTO3)
                    continue;

                if (dep->service_count()) {
                    auto dep_name = replace_proto(dep->name(), "yaz.rpc.h");
                    w.wl("#include {}", ab(dep_name));
                } else {
                    auto dep_name = replace_proto(dep->name(), "yaz.h");
                    w.wl("#include {}", ab(dep_name));
                }
            }
            w.l();

            auto service_desc = [&](const gp::ServiceDescriptor* service) {
                for (int m = 0; m < service->method_count(); ++m) {
                    auto* method = service->method(m);
                    if (!is_supported(method))
                        continue;

                    std::string base_name = do_sformat("{}_{}", service->name(), method->name());
                    std::string name = camel_case(base_name);
                    std::string c_name = snake_case(base_name);
                    std::string input = do_sformat("{}EntityRef", f.type_name(method->input_type()));
                    std::string output = do_sformat("{}LayeredBuilder<Tag>", f.type_name(method->output_type()));
                    std::string output_full = do_sformat("{}LayeredBuilder<{}Ctx::Tag>", f.type_name(method->output_type()), name);
                    w.w("struct {}Ctx ", name)
                        .class_braced([&] {
                            w.w("struct Tag ")
                                .class_braced([&] {
                                    w.wl("static constexpr auto service_name = ::folly::makeFixedString({});", q(service->name()));
                                    w.wl("static constexpr auto method_name = ::folly::makeFixedString({});", q(method->name()));
                                });
                            w.wl("static constexpr Tag tag;")
;                            w.wl("const ::yazik::concurrency::scheduler_ptr_t& scheduler;");
                            w.wl("{} response;", output);
                        });
                    w.l();
                    auto unit_pattern = [&](
                        const std::string& unit_type,
                        const std::string& resp_wr,
                        const std::string& req_wr,
                        const std::string& resp_broken,
                        const std::string& postf
                    ) {

                        std::string concept_postf = postf.empty()
                            ? ""
                            : "_" + snake_case(postf);

                        w.wl("template<typename Fn>");
                        w.w(
                            "concept c_{}{}_unit = ::yazik::rpc::c_{}_unit", c_name, concept_postf, unit_type)
                            .braced_detail([&]{
                                w.wl("Fn,");
                                w.wl("{},", input);
                                w.wl("{},", output_full);
                                w.wl("{}Ctx", name);
                            }, false, "<", ">")
                            .wl(";");
                        w.l();
                        w.w("struct {}{}Functor ", name, postf)
                            .class_braced([&]{
                                w.wl("using ResponseTag = typename {}Ctx::Tag;", name);
                                w.w("virtual ::yazik::rpc::{}<ResponseTag> operator ()", resp_wr)
                                    .braced_detail([&]{
                                        w.wl("{}Ctx& /*ctx*/,", name);
                                        w.wl("{}&& request", req_wr);
                                    }, false, "(",")")
                                    .w(" ")
                                    .braced([&]{
                                        w.wl("return ::yazik::rpc::RpcStatus::unimplemenmted()");
                                        w.wl_indent(".as_broken{}<ResponseTag>();", resp_broken);
                                    });
                                w.wl("virtual ~{}{}Functor() = default;", name, postf);
                            });
                        w.l();
                    };

                    if (method->server_streaming()) {
                        unit_pattern(
                            "stream_writer",
                            "RpcChannel",
                            input,
                            "_channel",
                            ""
                        );
                    } else if (method->client_streaming()) {
                        unit_pattern(
                            "stream_reader",
                            "RpcTask",
                            do_sformat("::yazik::rpc::RpcChannel<{}>", input),
                            "_task",
                            ""
                        );
                    } else {
                        unit_pattern(
                            "unary_async",
                            "RpcTask",
                            input,
                            "_task",
                            "Async"
                        );
                        unit_pattern(
                            "unary_sync",
                            "RpcResult",
                            input,
                            "",
                            "Sync"
                        );
                        w.wl("template <typename Fn>");
                        w.wl("concept c_{0}_unit = c_{0}_sync_unit<Fn> || c_{0}_async_unit<Fn>;", c_name);
                        w.l();
                    }
                }
            };

            auto body = [&] {
                w.l();
                for (int s = 0; s < file->service_count(); ++s)
                    service_desc(file->service(s));

            };

            if (file->package().empty()) body();
            else w.w("namespace {} ", to_cpp_ns(file->package())).braced(body);
            w.l();

            co_return;
        }

        Result<> gen_yaz_grpc_header(
            const gp::FileDescriptor* file,
            gpc::GeneratorContext* generator_context
        ) const {
            auto w = FileWriter { file, generator_context, "yaz.grpc.h" };
            auto f = Formatter { file, "" };

            w.wl("#pragma once");
            w.l();
            w.wl("#include {}", ab("yazik/compiler/support/grpc_support.hpp"));
            w.wl("#include {}", ab(replace_proto(file->name(), "yaz.rpc.h")));
            w.wl("#include {}", ab(replace_proto(file->name(), "yaz.pb.h")));
            w.wl("#include {}", ab(replace_proto(file->name(), "grpc.pb.h")));

            for (int d = 0; d < file->dependency_count(); ++d) {
                auto* dep = file->dependency(d);
                if (dep->syntax() != gp::FileDescriptor::SYNTAX_PROTO3)
                    continue;

                if (dep->service_count()) {
                    auto dep_name = replace_proto(dep->name(), "yaz.grpc.h");
                    w.wl("#include {}", ab(dep_name));
                } else {
                    auto dep_name = replace_proto(dep->name(), "yaz.pb.h");
                    w.wl("#include {}", ab(dep_name));
                }
            }
            w.l();

            auto service_desc = [&](const gp::ServiceDescriptor* service) {
                for (int m = 0; m < service->method_count(); ++m) {
                    auto* method = service->method(m);
                    if (!is_supported(method))
                        continue;

                    std::string base_name = do_sformat("{}_{}", service->name(), method->name());
                    std::string name = camel_case(base_name);
                    std::string c_name = snake_case(base_name);

                    std::string input_tname = f.type_name(method->input_type());
                    std::string output_tname = f.type_name(method->output_type());

                    w.wl("template <c_{}_unit Unit>", c_name);
                    w.w("struct {}GrpcHandle: ::yazik::compiler::support::Initializer ", name)
                        .class_braced([&] {
                            w.wl("using base_service_t = {};", service->name());
                            w.wl("using service_t = {}::AsyncService;", service->name());
                            w.wl("using request_t = {};", input_tname);
                            w.wl("using response_t = {};", output_tname);
                            w.wl("using context_t = {}Ctx;", name);
                            w.wl("using scheduler_ptr_t = ::yazik::rpc::grpc::server_queue_thread_scheduler_ptr_t;");
                            w.l();
                            w.w("static ::yazik::OneWayTask spawn")
                                .braced_detail([&]{
                                    w.wl("Unit unit,");
                                    w.wl("service_t* service,");
                                    w.wl("const scheduler_ptr_t& scheduler");
                                }, false, "(", ") ")
                                .braced([&]{
                                    w.w("auto stepper = ::yazik::rpc::grpc::GrpcTagStepper ")
                                        .class_braced([&] {
                                            w.wl("scheduler.get() $$yaz_debug(, YAZ_LOCATION_STR)");
                                        });

                                    w.wl("::grpc::ServerContext grpc_ctx;");
                                    w.wl("auto* cq = scheduler->server_queue();");
                                    w.l();
                                    if (method->server_streaming()) {
                                        w.wl("request_t request;");
                                        w.wl("::grpc::ServerAsyncWriter<response_t> writer {{ &grpc_ctx }};");

                                        w.w("service->Request{}", method->name())
                                            .braced_detail([&]{
                                                w.wl("&grpc_ctx,");
                                                w.wl("&request,");
                                                w.wl("&writer,");
                                                w.wl("cq,");
                                                w.wl("cq,");
                                                w.wl("stepper.tag()");
                                            }, true, "(", ");");
                                    } else if (method->client_streaming()) {
                                        w.wl("::grpc::ServerAsyncReader<response_t, request_t> reader {{ &grpc_ctx }};");

                                        w.w("service->Request{}", method->name())
                                            .braced_detail([&]{
                                                w.wl("&grpc_ctx,");
                                                w.wl("&reader,");
                                                w.wl("cq,");
                                                w.wl("cq,");
                                                w.wl("stepper.tag()");
                                            }, true, "(", ");");
                                    } else {
                                        w.wl("request_t request;");
                                        w.wl("::grpc::ServerAsyncResponseWriter<response_t> responder {{ &grpc_ctx }};");

                                        w.w("service->Request{}", method->name())
                                            .braced_detail([&]{
                                                w.wl("&grpc_ctx,");
                                                w.wl("&request,");
                                                w.wl("&responder,");
                                                w.wl("cq,");
                                                w.wl("cq,");
                                                w.wl("stepper.tag()");
                                            }, true, "(", ");");
                                    }
                                    w.l();
                                    w.wl("co_await stepper.step(YAZ_LOCATION_STR);");
                                    w.wl("spawn(unit, service, scheduler);");
                                    w.l();
                                    w.wl("response_t response;");
                                    w.wl("context_t::Tag tag;");
                                    w.w("context_t ctx ")
                                        .class_braced([&] {
                                            w.wl(".scheduler = scheduler,");
                                            w.w(".response = {}PbBuilderSpec::wrap_with_parent<context_t::Tag>", output_tname)
                                                .braced_detail([&] {
                                                    w.wl("response,");
                                                    w.wl("tag");
                                                }, true, "(", ")");
                                        });
                                    w.l();
                                    if (method->server_streaming()) {
                                        w.wl("auto channel = unit(ctx, {}PbSpec::wrap(request));", input_tname);
                                        w.w("if (auto result = channel.one_result_only()) ")
                                            .braced_detail([&] {
                                                w.w("if (*result) ")
                                                    .braced_detail([&] {
                                                        w.w("writer.WriteAndFinish")
                                                            .braced_detail([&]{
                                                                w.wl("response,");
                                                                w.wl("::grpc::WriteOptions{{}},");
                                                                w.wl("::grpc::Status::OK,");
                                                                w.wl("stepper.tag()");
                                                            }, true, "(", ");");
                                                    }, false)
                                                    .w(" else ")
                                                    .braced([&] {
                                                        w.w("writer.Finish")
                                                            .braced_detail([&]{
                                                                w.wl("::yazik::rpc::grpc::grpc_from_rpc_status(result->error()),");
                                                                w.wl("stepper.tag()");
                                                            }, true, "(", ");");
                                                    });
                                            }, false)
                                            .w(" else ")
                                            .braced([&] {
                                                w.w("for co_await(auto&& _: channel) ")
                                                    .braced([&] {
                                                        w.wl("co_await scheduler->ensure_on(YAZ_LOCATION_STR);");
                                                        w.wl("writer.Write(response, stepper.tag());");
                                                        w.wl("response = response_t{{}};");
                                                        w.w("if (!(co_await stepper.step(YAZ_LOCATION_STR))) ")
                                                            .braced([&] {
                                                                w.wl("writer.Finish(::grpc::Status::CANCELLED, stepper.tag());");
                                                                w.wl("co_await stepper.step(YAZ_LOCATION_STR);");
                                                                w.wl("co_return;");
                                                            });
                                                    });
                                                w.wl("co_await scheduler->ensure_on(YAZ_LOCATION_STR);");
                                                w.w("writer.Finish")
                                                    .braced_detail([&]{
                                                        w.wl("::yazik::rpc::grpc::grpc_from_rpc_status(channel.status()),");
                                                        w.wl("stepper.tag()");
                                                    }, true, "(", ");");
                                            });
                                    } else if (method->client_streaming()) {
                                        w.wl("request_t request;");
                                        w.w("auto result = co_await unit")
                                            .braced_detail([&]{
                                                w.wl("ctx,");
                                                w.w("[&]() -> ::yazik::rpc::RpcChannel<{}EntityRef> ", input_tname)
                                                    .braced([&]{
                                                        w.w("for (;;) ")
                                                            .braced([&] {
                                                                w.wl("reader.Read(&request, stepper.tag());");
                                                                w.w("if (!(co_await stepper.step(YAZ_LOCATION_STR))) ")
                                                                    .braced([&] {
                                                                        w.wl("break;");
                                                                    });
                                                                w.wl("co_yield {}PbSpec::wrap(request);", input_tname);
                                                                w.wl("request = request_t{{}};");
                                                            });
                                                    });
                                            }, false, "(", ")")
                                            .wl(".wrapped();");
                                        w.wl("co_await scheduler->ensure_on(YAZ_LOCATION_STR);");
                                        w.l();
                                        w.w("if (result) ")
                                            .braced_detail([&] {
                                                w.w("reader.Finish")
                                                    .braced_detail([&] {
                                                        w.wl("response,");
                                                        w.wl("::grpc::Status::OK,");
                                                        w.wl("stepper.tag()");
                                                    }, true, "(", ");");
                                            }, false)
                                            .w(" else ")
                                            .braced([&] {
                                                w.w("reader.FinishWithError")
                                                    .braced_detail([&] {
                                                        w.wl("::yazik::rpc::grpc::grpc_from_rpc_status(result.error()),");
                                                        w.wl("stepper.tag()");
                                                    }, true, "(", ");");
                                            });
                                    } else {
                                        w.wl("::yazik::rpc::RpcResult<context_t::Tag> result;");
                                        w.w("if constexpr (c_{}_sync_unit<Unit>) ", c_name)
                                            .braced_detail([&] {
                                                w.wl("result = unit(ctx, {}PbSpec::wrap(request));", input_tname);
                                            }, false)
                                            .w(" else ")
                                            .braced([&] {
                                                w.wl("result = co_await unit(ctx, {}PbSpec::wrap(request)).wrapped();", input_tname);
                                            });
                                        w.l();
                                        w.w("if (result) ")
                                            .braced_detail([&] {
                                                w.w("responder.Finish")
                                                    .braced_detail([&] {
                                                        w.wl("response,");
                                                        w.wl("::grpc::Status::OK,");
                                                        w.wl("stepper.tag()");
                                                    }, true, "(", ");");
                                            }, false)
                                            .w(" else ")
                                            .braced([&] {
                                                w.w("responder.FinishWithError")
                                                    .braced_detail([&] {
                                                        w.wl("::yazik::rpc::grpc::grpc_from_rpc_status(result.error()),");
                                                        w.wl("stepper.tag()");
                                                    }, true, "(", ");");
                                            });
                                    }
                                    w.l();
                                    w.wl("co_await stepper.step(YAZ_LOCATION_STR);");
                                    w.wl("co_return;");
                                });
                        });
                    w.l();

                }
            };

            auto body = [&] {
                w.l();
                for (int s = 0; s < file->service_count(); ++s)
                    service_desc(file->service(s));

            };

            if (file->package().empty()) body();
            else w.w("namespace {} ", to_cpp_ns(file->package())).braced(body);
            w.l();

            co_return;
        }

        Result<bool> generate(
            const gp::FileDescriptor* file,
            const std::string& parameter,
            gpc::GeneratorContext* generator_context
        ) const noexcept override {
            co_await gen_yaz_rpc_header(file, generator_context);
            co_await gen_yaz_grpc_header(file, generator_context);
            co_return true;
        }
    };

    generator_ptr_t create_cpp_rpc_generator() {
        return std::make_shared<CppGrpcGenerator>();
    }

} // end of ::yazik::compiler namespace