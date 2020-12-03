#pragma once

#include <yazik/compiler/support/grpc_support.hpp>
#include <book/sample.yaz.rpc.h>
#include <book/sample.yaz.pb.h>
#include <book/sample.grpc.pb.h>

namespace com::book::yaz {
    namespace book_service {
        namespace get_book {
            struct GrpcCtx: Ctx {
                using Ctx::Ctx;
                using grpc_service_t = BookService::AsyncService;
                using input_pb_t = GetBookRequest;
                using input_pb_spec_t = get_book_request::RefPbSpec;
                using input_pb_builder_spec_t = get_book_request::BuilderPbSpec;
                using output_pb_t = Book;
                using output_pb_spec_t = book::RefPbSpec;
                using output_pb_builder_spec_t = book::BuilderPbSpec;
            };
            template <c_unit Unit = class UnitTag>
            struct GrpcHandle {
                using payload_t = std::conditional_t<
                    c_sync_unit<Unit>,
                    ::yazik::compiler::grpc_support::UnaryPayloadSync<GrpcCtx, Unit>,
                    ::yazik::compiler::grpc_support::UnaryPayloadAsync<GrpcCtx, Unit>
                >;
                using scheduler_ptr_t = ::yazik::rpc::grpc::server_queue_thread_scheduler_ptr_t;
                using service_t = GrpcCtx::grpc_service_t;
                Unit unit;
                GrpcHandle(Unit unit) noexcept
                : unit { std::move(unit) }
                {}
                static ::yazik::OneWayTask spawn(
                    Unit unit,
                    GrpcCtx::grpc_service_t* service,
                    const scheduler_ptr_t& scheduler
                ) {
                    auto payload = payload_t { scheduler };
                    co_await payload.request(service, &BookService::AsyncService::RequestGetBook);
                    spawn(unit, service, scheduler);
                    co_await payload.run(unit);
                    co_return;
                }
            };
        }
        namespace get_books_via_author {
            struct GrpcCtx: Ctx {
                using Ctx::Ctx;
                using grpc_service_t = BookService::AsyncService;
                using input_pb_t = GetBookViaAuthor;
                using input_pb_spec_t = get_book_via_author::RefPbSpec;
                using input_pb_builder_spec_t = get_book_via_author::BuilderPbSpec;
                using output_pb_t = Book;
                using output_pb_spec_t = book::RefPbSpec;
                using output_pb_builder_spec_t = book::BuilderPbSpec;
            };
            template <c_unit Unit = class UnitTag>
            struct GrpcHandle {
                using payload_t = std::conditional_t<
                    c_sync_unit<Unit>,
                    ::yazik::compiler::grpc_support::ServerStreamingPayloadSync<GrpcCtx, Unit>,
                    ::yazik::compiler::grpc_support::ServerStreamingPayloadAsync<GrpcCtx, Unit>
                >;
                using scheduler_ptr_t = ::yazik::rpc::grpc::server_queue_thread_scheduler_ptr_t;
                using service_t = GrpcCtx::grpc_service_t;
                Unit unit;
                GrpcHandle(Unit unit) noexcept
                : unit { std::move(unit) }
                {}
                static ::yazik::OneWayTask spawn(
                    Unit unit,
                    GrpcCtx::grpc_service_t* service,
                    const scheduler_ptr_t& scheduler
                ) {
                    auto payload = payload_t { scheduler };
                    co_await payload.request(service, &BookService::AsyncService::RequestGetBooksViaAuthor);
                    spawn(unit, service, scheduler);
                    co_await payload.run(unit);
                    co_return;
                }
            };
        }
        namespace get_greatest_book {
            struct GrpcCtx: Ctx {
                using Ctx::Ctx;
                using grpc_service_t = BookService::AsyncService;
                using input_pb_t = GetBookRequest;
                using input_pb_spec_t = get_book_request::RefPbSpec;
                using input_pb_builder_spec_t = get_book_request::BuilderPbSpec;
                using output_pb_t = Book;
                using output_pb_spec_t = book::RefPbSpec;
                using output_pb_builder_spec_t = book::BuilderPbSpec;
            };
            template <c_unit Unit = class UnitTag>
            struct GrpcHandle {
                using payload_t = std::conditional_t<
                    c_sync_unit<Unit>,
                    ::yazik::compiler::grpc_support::ClientStreamingPayloadSync<GrpcCtx, Unit>,
                    ::yazik::compiler::grpc_support::ClientStreamingPayloadAsync<GrpcCtx, Unit>
                >;
                using scheduler_ptr_t = ::yazik::rpc::grpc::server_queue_thread_scheduler_ptr_t;
                using service_t = GrpcCtx::grpc_service_t;
                Unit unit;
                GrpcHandle(Unit unit) noexcept
                : unit { std::move(unit) }
                {}
                static ::yazik::OneWayTask spawn(
                    Unit unit,
                    GrpcCtx::grpc_service_t* service,
                    const scheduler_ptr_t& scheduler
                ) {
                    auto payload = payload_t { scheduler };
                    co_await payload.request(service, &BookService::AsyncService::RequestGetGreatestBook);
                    spawn(unit, service, scheduler);
                    co_await payload.run(unit);
                    co_return;
                }
            };
        }
    }
}
