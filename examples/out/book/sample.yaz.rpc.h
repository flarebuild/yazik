#pragma once

#include <yazik/compiler/support/rpc_support.hpp>
#include <book/sample.yaz.h>

namespace com::book {

    struct BookServiceGetBookCtx {
        struct Tag {
            static constexpr auto service_name = ::folly::makeFixedString("BookService");
            static constexpr auto method_name = ::folly::makeFixedString("GetBook");
        };
        static constexpr Tag tag;
        const ::yazik::concurrency::scheduler_ptr_t& scheduler;
        BookLayeredBuilder<Tag> response;
    };

    template<typename Fn>
    concept c_book_service_get_book_async_unit = !boost::di::aux::is_complete<Fn>::value || ::yazik::rpc::c_unary_async_unit<
        Fn,
        GetBookRequestEntityRef,
        BookLayeredBuilder<BookServiceGetBookCtx::Tag>,
        BookServiceGetBookCtx
    >;

    struct BookServiceGetBookAsyncFunctor {
        using Ctx = BookServiceGetBookCtx;
        using ResponseTag = Ctx::Tag;
        using Request = GetBookRequestEntityRef;
        using Response = ::yazik::rpc::RpcTask<ResponseTag>;
        constexpr static auto rpc_type = ::yazik::compiler::rpc_support::RpcUnitType::UnaryAsync;
        virtual ::yazik::rpc::RpcTask<ResponseTag> operator ()(
            BookServiceGetBookCtx& ctx,
            GetBookRequestEntityRef&& request
        ) {
            return ::yazik::rpc::RpcStatus::unimplemenmted()
                .as_broken_task<ResponseTag>();
        }
        virtual ~BookServiceGetBookAsyncFunctor() = default;
    };

    template<typename Fn>
    concept c_book_service_get_book_sync_unit = !boost::di::aux::is_complete<Fn>::value || ::yazik::rpc::c_unary_sync_unit<
        Fn,
        GetBookRequestEntityRef,
        BookLayeredBuilder<BookServiceGetBookCtx::Tag>,
        BookServiceGetBookCtx
    >;

    struct BookServiceGetBookSyncFunctor {
        using Ctx = BookServiceGetBookCtx;
        using ResponseTag = Ctx::Tag;
        using Request = GetBookRequestEntityRef;
        using Response = ::yazik::rpc::RpcResult<ResponseTag>;
        constexpr static auto rpc_type = ::yazik::compiler::rpc_support::RpcUnitType::UnarySync;
        virtual ::yazik::rpc::RpcResult<ResponseTag> operator ()(
            BookServiceGetBookCtx& ctx,
            GetBookRequestEntityRef&& request
        ) {
            return ::yazik::rpc::RpcStatus::unimplemenmted()
                .as_broken<ResponseTag>();
        }
        virtual ~BookServiceGetBookSyncFunctor() = default;
    };

    template <typename Fn>
    concept c_book_service_get_book_unit = c_book_service_get_book_sync_unit<Fn> || c_book_service_get_book_async_unit<Fn>;

    struct BookServiceGetBooksViaAuthorCtx {
        struct Tag {
            static constexpr auto service_name = ::folly::makeFixedString("BookService");
            static constexpr auto method_name = ::folly::makeFixedString("GetBooksViaAuthor");
        };
        static constexpr Tag tag;
        const ::yazik::concurrency::scheduler_ptr_t& scheduler;
        BookLayeredBuilder<Tag> response;
    };

    template<typename Fn>
    concept c_book_service_get_books_via_author_unit = !boost::di::aux::is_complete<Fn>::value || ::yazik::rpc::c_stream_writer_unit<
        Fn,
        GetBookViaAuthorEntityRef,
        BookLayeredBuilder<BookServiceGetBooksViaAuthorCtx::Tag>,
        BookServiceGetBooksViaAuthorCtx
    >;

    struct BookServiceGetBooksViaAuthorFunctor {
        using Ctx = BookServiceGetBooksViaAuthorCtx;
        using ResponseTag = Ctx::Tag;
        using Request = GetBookViaAuthorEntityRef;
        using Response = ::yazik::rpc::RpcChannel<ResponseTag>;
        constexpr static auto rpc_type = ::yazik::compiler::rpc_support::RpcUnitType::ServerStreaming;
        virtual ::yazik::rpc::RpcChannel<ResponseTag> operator ()(
            BookServiceGetBooksViaAuthorCtx& ctx,
            GetBookViaAuthorEntityRef&& request
        ) {
            return ::yazik::rpc::RpcStatus::unimplemenmted()
                .as_broken_channel<ResponseTag>();
        }
        virtual ~BookServiceGetBooksViaAuthorFunctor() = default;
    };

    struct BookServiceGetGreatestBookCtx {
        struct Tag {
            static constexpr auto service_name = ::folly::makeFixedString("BookService");
            static constexpr auto method_name = ::folly::makeFixedString("GetGreatestBook");
        };
        static constexpr Tag tag;
        const ::yazik::concurrency::scheduler_ptr_t& scheduler;
        BookLayeredBuilder<Tag> response;
    };

    template<typename Fn>
    concept c_book_service_get_greatest_book_unit = !boost::di::aux::is_complete<Fn>::value || ::yazik::rpc::c_stream_reader_unit<
        Fn,
        GetBookRequestEntityRef,
        BookLayeredBuilder<BookServiceGetGreatestBookCtx::Tag>,
        BookServiceGetGreatestBookCtx
    >;

    struct BookServiceGetGreatestBookFunctor {
        using Ctx = BookServiceGetGreatestBookCtx;
        using ResponseTag = Ctx::Tag;
        using Request = ::yazik::rpc::RpcChannel<GetBookRequestEntityRef>;
        using Response = ::yazik::rpc::RpcTask<ResponseTag>;
        constexpr static auto rpc_type = ::yazik::compiler::rpc_support::RpcUnitType::ClientStreaming;
        virtual ::yazik::rpc::RpcTask<ResponseTag> operator ()(
            BookServiceGetGreatestBookCtx& ctx,
            ::yazik::rpc::RpcChannel<GetBookRequestEntityRef>&& request
        ) {
            return ::yazik::rpc::RpcStatus::unimplemenmted()
                .as_broken_task<ResponseTag>();
        }
        virtual ~BookServiceGetGreatestBookFunctor() = default;
    };

}

