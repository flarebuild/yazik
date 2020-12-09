#pragma once

#include <yazik/compiler/support/rpc_client_support.hpp>
#include <book/sample.yaz.rpc.h>

namespace com::book::yaz {
    namespace book_service {
        namespace get_book {
            struct ClientCtx {
                struct Tag {
                    static constexpr auto service_name = ::folly::makeFixedString("BookService");
                    static constexpr auto method_name = ::folly::makeFixedString("GetBook");
                };
                static constexpr Tag tag;
                using input_ref_t = get_book_request::Ref;
                using output_ref_t = book::Ref;
            };
            template<typename Fn>
            concept c_client = !boost::di::aux::is_complete<Fn>::value || 
                ::yazik::rpc::c_rpc_unary_client<Fn, ClientCtx::input_ref_t, ClientCtx::output_ref_t, ClientCtx>;
        }
        namespace get_books_via_author {
            struct ClientCtx {
                struct Tag {
                    static constexpr auto service_name = ::folly::makeFixedString("BookService");
                    static constexpr auto method_name = ::folly::makeFixedString("GetBooksViaAuthor");
                };
                static constexpr Tag tag;
                using input_ref_t = get_book_via_author::Ref;
                using output_ref_t = book::Ref;
            };
            template<typename Fn>
            concept c_client = !boost::di::aux::is_complete<Fn>::value || 
                ::yazik::rpc::c_stream_reader_client<Fn, ClientCtx::input_ref_t, ClientCtx::output_ref_t, ClientCtx>;
        }
        namespace get_greatest_book {
            struct ClientCtx {
                struct Tag {
                    static constexpr auto service_name = ::folly::makeFixedString("BookService");
                    static constexpr auto method_name = ::folly::makeFixedString("GetGreatestBook");
                };
                static constexpr Tag tag;
                using input_ref_t = get_book_request::Ref;
                using output_ref_t = book::Ref;
            };
            template<typename Fn>
            concept c_client = !boost::di::aux::is_complete<Fn>::value || 
                ::yazik::rpc::c_stream_writer_client<Fn, ClientCtx::input_ref_t, ClientCtx::output_ref_t, ClientCtx>;
        }
    }
}
