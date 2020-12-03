#pragma once

#include <yazik/compiler/support/rpc_support.hpp>
#include <book/sample.yaz.h>

namespace com::book::yaz {
    namespace book_service {
        namespace get_book {
            struct Ctx {
                struct Tag {
                    static constexpr auto service_name = ::folly::makeFixedString("BookService");
                    static constexpr auto method_name = ::folly::makeFixedString("GetBook");
                };
                static constexpr Tag tag;
                using input_ref_t = get_book_request::Ref;
                using output_ref_t = book::Ref;
                using resp_ok_t = ::yazik::rpc::RespOk<output_ref_t>;
                using output_builder_t = book::LayeredBuilder<resp_ok_t>;
                ::yazik::concurrency::scheduler_ptr_t scheduler;
                output_builder_t response;
                inline Ctx(
                    ::yazik::concurrency::scheduler_ptr_t scheduler,
                    output_builder_t response
                )
                : scheduler { std::move(scheduler) }
                , response { std::move(response) }
                {}
            };
            template<typename Fn>
            concept c_sync_unit = ::yazik::rpc::c_unary_sync_unit<Fn, Ctx::input_ref_t, Ctx::output_ref_t, Ctx>;
            template<typename Fn>
            concept c_async_unit = ::yazik::rpc::c_unary_async_unit<Fn, Ctx::input_ref_t, Ctx::output_ref_t, Ctx>;
            template<typename Fn>
            concept c_unit = !boost::di::aux::is_complete<Fn>::value ||  c_sync_unit<Fn> || c_async_unit<Fn>;
            struct SyncCall {
                constexpr static auto s_rpc_type = ::yazik::rpc::RpcUnitType::UnarySync;
                using context_t = Ctx;
                /*
                ::yazik::rpc::RpcResult<contenx_t::resp_ok_t> call(
                    context_t& context,
                    get_book_request::Ref request
                ) noexcept {
                    return ::yazik::rpc::RpcStatus::unimplemenmted()
                        .as_broken<contenx_t::resp_ok_t>;
                }
                */
            };
            struct AsyncCall {
                constexpr static auto s_rpc_type = ::yazik::rpc::RpcUnitType::UnaryAsync;
                using context_t = Ctx;
                /*
                ::yazik::rpc::RpcTask<contenx_t::resp_ok_t> call(
                    context_t& context,
                    get_book_request::Ref request
                ) noexcept {
                    return ::yazik::rpc::RpcStatus::unimplemenmted()
                        .as_broken_task<contenx_t::resp_ok_t>;
                }
                */
            };
        }
        namespace get_books_via_author {
            struct Ctx {
                struct Tag {
                    static constexpr auto service_name = ::folly::makeFixedString("BookService");
                    static constexpr auto method_name = ::folly::makeFixedString("GetBooksViaAuthor");
                };
                static constexpr Tag tag;
                using input_ref_t = get_book_via_author::Ref;
                using output_ref_t = book::Ref;
                using resp_ok_t = ::yazik::rpc::RespOk<output_ref_t>;
                using output_builder_t = book::LayeredBuilder<resp_ok_t>;
                ::yazik::concurrency::scheduler_ptr_t scheduler;
                output_builder_t response;
                inline Ctx(
                    ::yazik::concurrency::scheduler_ptr_t scheduler,
                    output_builder_t response
                )
                : scheduler { std::move(scheduler) }
                , response { std::move(response) }
                {}
            };
            template<typename Fn>
            concept c_sync_unit = ::yazik::rpc::c_stream_writer_sync_unit<Fn, Ctx::input_ref_t, Ctx::output_ref_t, Ctx>;
            template<typename Fn>
            concept c_async_unit = ::yazik::rpc::c_stream_writer_async_unit<Fn, Ctx::input_ref_t, Ctx::output_ref_t, Ctx>;
            template<typename Fn>
            concept c_unit = !boost::di::aux::is_complete<Fn>::value ||  c_sync_unit<Fn> || c_async_unit<Fn>;
            struct SyncCall {
                constexpr static auto s_rpc_type = ::yazik::rpc::RpcUnitType::ServerStreamingSync;
                using context_t = Ctx;
                /*
                ::yazik::rpc::RpcGenerator<contenx_t::resp_ok_t> call(
                    context_t& context,
                    get_book_via_author::Ref request
                ) noexcept {
                    return ::yazik::rpc::RpcStatus::unimplemenmted()
                        .as_broken<contenx_t::resp_ok_t>;
                }
                */
            };
            struct AsyncCall {
                constexpr static auto s_rpc_type = ::yazik::rpc::RpcUnitType::ServerStreamingAsync;
                using context_t = Ctx;
                /*
                ::yazik::rpc::RpcChannel<contenx_t::resp_ok_t> call(
                    context_t& context,
                    get_book_via_author::Ref request
                ) noexcept {
                    return ::yazik::rpc::RpcStatus::unimplemenmted()
                        .as_broken_channel<contenx_t::resp_ok_t>;
                }
                */
            };
        }
        namespace get_greatest_book {
            struct Ctx {
                struct Tag {
                    static constexpr auto service_name = ::folly::makeFixedString("BookService");
                    static constexpr auto method_name = ::folly::makeFixedString("GetGreatestBook");
                };
                static constexpr Tag tag;
                using input_ref_t = get_book_request::Ref;
                using output_ref_t = book::Ref;
                using resp_ok_t = ::yazik::rpc::RespOk<output_ref_t>;
                using output_builder_t = book::LayeredBuilder<resp_ok_t>;
                ::yazik::concurrency::scheduler_ptr_t scheduler;
                output_builder_t response;
                inline Ctx(
                    ::yazik::concurrency::scheduler_ptr_t scheduler,
                    output_builder_t response
                )
                : scheduler { std::move(scheduler) }
                , response { std::move(response) }
                {}
            };
            template<typename Fn>
            concept c_sync_unit = ::yazik::rpc::c_stream_reader_sync_unit<Fn, Ctx::input_ref_t, Ctx::output_ref_t, Ctx>;
            template<typename Fn>
            concept c_async_unit = ::yazik::rpc::c_stream_reader_async_unit<Fn, Ctx::input_ref_t, Ctx::output_ref_t, Ctx>;
            template<typename Fn>
            concept c_unit = !boost::di::aux::is_complete<Fn>::value ||  c_sync_unit<Fn> || c_async_unit<Fn>;
            struct SyncCall {
                constexpr static auto s_rpc_type = ::yazik::rpc::RpcUnitType::ClientStreamingSync;
                using context_t = Ctx;
                /*
                ::yazik::rpc::RpcResult<contenx_t::resp_ok_t> call(
                    context_t& context,
                    ::yazik::rpc::RpcGenerator<get_book_request::Ref> request
                ) noexcept {
                    return ::yazik::rpc::RpcStatus::unimplemenmted()
                        .as_broken<contenx_t::resp_ok_t>;
                }
                */
            };
            struct AsyncCall {
                constexpr static auto s_rpc_type = ::yazik::rpc::RpcUnitType::ClientStreamingAsync;
                using context_t = Ctx;
                /*
                ::yazik::rpc::RpcTask<contenx_t::resp_ok_t> call(
                    context_t& context,
                    ::yazik::rpc::RpcChannel<get_book_request::Ref> request
                ) noexcept {
                    return ::yazik::rpc::RpcStatus::unimplemenmted()
                        .as_broken_task<contenx_t::resp_ok_t>;
                }
                */
            };
        }
    }
}
