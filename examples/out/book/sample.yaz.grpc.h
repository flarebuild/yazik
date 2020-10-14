#pragma once

#include <yazik/compiler/support/grpc_support.hpp>
#include <book/sample.yaz.rpc.h>
#include <book/sample.grpc.pb.h>

namespace com::book {

    template <c_book_service_get_book_unit Unit>
    struct BookServiceGetBookGrpcHandle: ::yazik::compiler::support::Initializer {
        using base_service_t = BookService;
        using service_t = BookService::AsyncService;
        using request_t = GetBookRequest;
        using response_t = Book;
        using context_t = BookServiceGetBookCtx;
        using scheduler_ptr_t = ::yazik::rpc::grpc::server_queue_thread_scheduler_ptr_t;

        static ::yazik::OneWayTask spawn(
            Unit unit,
            service_t* service,
            const scheduler_ptr_t& scheduler
        ) {
            auto stepper = ::yazik::rpc::grpc::GrpcTagStepper {
                scheduler.get() $yaz_debug(, YAZ_LOCATION_STR)
            };
            ::grpc::ServerContext grpc_ctx;
            auto* cq = scheduler->server_queue();

            request_t request;
            ::grpc::ServerAsyncResponseWriter<response_t> responder { &grpc_ctx };
            service->RequestGetBook(
                &grpc_ctx,
                &request,
                &responder,
                cq,
                cq,
                stepper.tag()
            );

            co_await stepper.step(YAZ_LOCATION_STR);
            spawn(unit, service, scheduler);

            response_t response;
            context_t::Tag tag;
            context_t ctx {
                .scheduler = scheduler,
                .response = BookPbBuilderSpec::wrap_with_parent<context_t::Tag>(
                    response,
                    tag
                )
            };

            ::yazik::rpc::RpcResult<context_t::Tag> result;
            if constexpr (c_book_service_get_book_sync_unit<Unit>) {
                result = unit(ctx, GetBookRequestPbSpec::wrap(request));
            } else {
                result = co_await unit(ctx, GetBookRequestPbSpec::wrap(request)).wrapped();
            }

            if (result) {
                responder.Finish(
                    response,
                    ::grpc::Status::OK,
                    stepper.tag()
                );
            } else {
                responder.FinishWithError(
                    ::yazik::rpc::grpc::grpc_from_rpc_status(result.error()),
                    stepper.tag()
                );
            }

            co_await stepper.step(YAZ_LOCATION_STR);
            co_return;
        }
    };

    template <c_book_service_get_books_via_author_unit Unit>
    struct BookServiceGetBooksViaAuthorGrpcHandle: ::yazik::compiler::support::Initializer {
        using base_service_t = BookService;
        using service_t = BookService::AsyncService;
        using request_t = GetBookViaAuthor;
        using response_t = Book;
        using context_t = BookServiceGetBooksViaAuthorCtx;
        using scheduler_ptr_t = ::yazik::rpc::grpc::server_queue_thread_scheduler_ptr_t;

        static ::yazik::OneWayTask spawn(
            Unit unit,
            service_t* service,
            const scheduler_ptr_t& scheduler
        ) {
            auto stepper = ::yazik::rpc::grpc::GrpcTagStepper {
                scheduler.get() $yaz_debug(, YAZ_LOCATION_STR)
            };
            ::grpc::ServerContext grpc_ctx;
            auto* cq = scheduler->server_queue();

            request_t request;
            ::grpc::ServerAsyncWriter<response_t> writer { &grpc_ctx };
            service->RequestGetBooksViaAuthor(
                &grpc_ctx,
                &request,
                &writer,
                cq,
                cq,
                stepper.tag()
            );

            co_await stepper.step(YAZ_LOCATION_STR);
            spawn(unit, service, scheduler);

            response_t response;
            context_t::Tag tag;
            context_t ctx {
                .scheduler = scheduler,
                .response = BookPbBuilderSpec::wrap_with_parent<context_t::Tag>(
                    response,
                    tag
                )
            };

            auto channel = unit(ctx, GetBookViaAuthorPbSpec::wrap(request));
            if (auto result = channel.one_result_only()) {
                if (*result) {
                    writer.WriteAndFinish(
                        response,
                        ::grpc::WriteOptions{},
                        ::grpc::Status::OK,
                        stepper.tag()
                    );
                } else {
                    writer.Finish(
                        ::yazik::rpc::grpc::grpc_from_rpc_status(result->error()),
                        stepper.tag()
                    );
                }
            } else {
                for co_await(auto&& _: channel) {
                    co_await scheduler->ensure_on(YAZ_LOCATION_STR);
                    writer.Write(response, stepper.tag());
                    response = response_t{};
                    if (!(co_await stepper.step(YAZ_LOCATION_STR))) {
                        writer.Finish(::grpc::Status::CANCELLED, stepper.tag());
                        co_await stepper.step(YAZ_LOCATION_STR);
                        co_return;
                    }
                }
                co_await scheduler->ensure_on(YAZ_LOCATION_STR);
                writer.Finish(
                    ::yazik::rpc::grpc::grpc_from_rpc_status(channel.status()),
                    stepper.tag()
                );
            }

            co_await stepper.step(YAZ_LOCATION_STR);
            co_return;
        }
    };

    template <c_book_service_get_greatest_book_unit Unit>
    struct BookServiceGetGreatestBookGrpcHandle: ::yazik::compiler::support::Initializer {
        using base_service_t = BookService;
        using service_t = BookService::AsyncService;
        using request_t = GetBookRequest;
        using response_t = Book;
        using context_t = BookServiceGetGreatestBookCtx;
        using scheduler_ptr_t = ::yazik::rpc::grpc::server_queue_thread_scheduler_ptr_t;

        static ::yazik::OneWayTask spawn(
            Unit unit,
            service_t* service,
            const scheduler_ptr_t& scheduler
        ) {
            auto stepper = ::yazik::rpc::grpc::GrpcTagStepper {
                scheduler.get() $yaz_debug(, YAZ_LOCATION_STR)
            };
            ::grpc::ServerContext grpc_ctx;
            auto* cq = scheduler->server_queue();

            ::grpc::ServerAsyncReader<response_t, request_t> reader { &grpc_ctx };
            service->RequestGetGreatestBook(
                &grpc_ctx,
                &reader,
                cq,
                cq,
                stepper.tag()
            );

            co_await stepper.step(YAZ_LOCATION_STR);
            spawn(unit, service, scheduler);

            response_t response;
            context_t::Tag tag;
            context_t ctx {
                .scheduler = scheduler,
                .response = BookPbBuilderSpec::wrap_with_parent<context_t::Tag>(
                    response,
                    tag
                )
            };

            request_t request;
            auto result = co_await unit(
                ctx,
                [&]() -> ::yazik::rpc::RpcChannel<GetBookRequestEntityRef> {
                    for (;;) {
                        reader.Read(&request, stepper.tag());
                        if (!(co_await stepper.step(YAZ_LOCATION_STR))) {
                            break;
                        }
                        co_yield GetBookRequestPbSpec::wrap(request);
                        request = request_t{};
                    }
                }
            ).wrapped();
            co_await scheduler->ensure_on(YAZ_LOCATION_STR);

            if (result) {
                reader.Finish(
                    response,
                    ::grpc::Status::OK,
                    stepper.tag()
                );
            } else {
                reader.FinishWithError(
                    ::yazik::rpc::grpc::grpc_from_rpc_status(result.error()),
                    stepper.tag()
                );
            }

            co_await stepper.step(YAZ_LOCATION_STR);
            co_return;
        }
    };

}

