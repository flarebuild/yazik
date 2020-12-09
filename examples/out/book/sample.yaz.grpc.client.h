#pragma once

#include <yazik/compiler/support/grpc_client_support.hpp>
#include <book/sample.yaz.rpc.client.h>
#include <book/sample.yaz.pb.h>
#include <book/sample.grpc.pb.h>

namespace com::book::yaz {
    namespace book_service {
        namespace get_book {
            struct GrpcClientCtx: ClientCtx {
                using ClientCtx::ClientCtx;
                using stub_t = BookService::Stub;
                using input_pb_t = GetBookRequest;
                using input_pb_spec_t = get_book_request::RefPbSpec;
                using input_pb_builder_spec_t = get_book_request::BuilderPbSpec;
                using output_pb_t = Book;
                using output_pb_spec_t = book::RefPbSpec;
                using output_pb_builder_spec_t = book::BuilderPbSpec;
            };
            using GrpcClient = ::yazik::compiler::grpc_support::UnaryClient<GrpcClientCtx>;
            struct GrpcClientFactory
            : ::yazik::compiler::grpc_support::UnaryClientFactory<GrpcClientCtx> {
                using Base = ::yazik::compiler::grpc_support::UnaryClientFactory<GrpcClientCtx>;
                GrpcClientFactory(std::shared_ptr<GrpcClientCtx::stub_t> stub) noexcept
                : Base { std::move(stub), &GrpcClientCtx::stub_t::PrepareAsyncGetBook }
                {}
            };
        }
        namespace get_books_via_author {
            struct GrpcClientCtx: ClientCtx {
                using ClientCtx::ClientCtx;
                using stub_t = BookService::Stub;
                using input_pb_t = GetBookViaAuthor;
                using input_pb_spec_t = get_book_via_author::RefPbSpec;
                using input_pb_builder_spec_t = get_book_via_author::BuilderPbSpec;
                using output_pb_t = Book;
                using output_pb_spec_t = book::RefPbSpec;
                using output_pb_builder_spec_t = book::BuilderPbSpec;
            };
            using GrpcClient = ::yazik::compiler::grpc_support::ServerStreamingClient<GrpcClientCtx>;
            struct GrpcClientFactory
            : ::yazik::compiler::grpc_support::ServerStreamingClientFactory<GrpcClientCtx> {
                using Base = ::yazik::compiler::grpc_support::ServerStreamingClientFactory<GrpcClientCtx>;
                GrpcClientFactory(std::shared_ptr<GrpcClientCtx::stub_t> stub) noexcept
                : Base { std::move(stub), &GrpcClientCtx::stub_t::PrepareAsyncGetBooksViaAuthor }
                {}
            };
        }
        namespace get_greatest_book {
            struct GrpcClientCtx: ClientCtx {
                using ClientCtx::ClientCtx;
                using stub_t = BookService::Stub;
                using input_pb_t = GetBookRequest;
                using input_pb_spec_t = get_book_request::RefPbSpec;
                using input_pb_builder_spec_t = get_book_request::BuilderPbSpec;
                using output_pb_t = Book;
                using output_pb_spec_t = book::RefPbSpec;
                using output_pb_builder_spec_t = book::BuilderPbSpec;
            };
            using GrpcClient = ::yazik::compiler::grpc_support::ClientStreamingClient<GrpcClientCtx>;
            struct GrpcClientFactory
            : ::yazik::compiler::grpc_support::ClientStreamingClientFactory<GrpcClientCtx> {
                using Base = ::yazik::compiler::grpc_support::ClientStreamingClientFactory<GrpcClientCtx>;
                GrpcClientFactory(std::shared_ptr<GrpcClientCtx::stub_t> stub) noexcept
                : Base { std::move(stub), &GrpcClientCtx::stub_t::PrepareAsyncGetGreatestBook }
                {}
            };
        }
    }
}
