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
    concept c_book_service_get_book_async_unit = ::yazik::rpc::c_unary_async_unit<
        Fn,
        GetBookRequestEntityRef,
        BookLayeredBuilder<BookServiceGetBookCtx::Tag>,
        BookServiceGetBookCtx
    >;

    struct BookServiceGetBookAsyncFunctor {
        using ResponseTag = typename BookServiceGetBookCtx::Tag;
        virtual ::yazik::rpc::RpcTask<ResponseTag> operator ()(
            BookServiceGetBookCtx& /*ctx*/,
            GetBookRequestEntityRef&& request
        ) {
            return ::yazik::rpc::RpcStatus::unimplemenmted()
                .as_broken_task<ResponseTag>();
        }
        virtual ~BookServiceGetBookAsyncFunctor() = default;
    };

    template<typename Fn>
    concept c_book_service_get_book_sync_unit = ::yazik::rpc::c_unary_sync_unit<
        Fn,
        GetBookRequestEntityRef,
        BookLayeredBuilder<BookServiceGetBookCtx::Tag>,
        BookServiceGetBookCtx
    >;

    struct BookServiceGetBookSyncFunctor {
        using ResponseTag = typename BookServiceGetBookCtx::Tag;
        virtual ::yazik::rpc::RpcResult<ResponseTag> operator ()(
            BookServiceGetBookCtx& /*ctx*/,
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
    concept c_book_service_get_books_via_author_unit = ::yazik::rpc::c_stream_writer_unit<
        Fn,
        GetBookViaAuthorEntityRef,
        BookLayeredBuilder<BookServiceGetBooksViaAuthorCtx::Tag>,
        BookServiceGetBooksViaAuthorCtx
    >;

    struct BookServiceGetBooksViaAuthorFunctor {
        using ResponseTag = typename BookServiceGetBooksViaAuthorCtx::Tag;
        virtual ::yazik::rpc::RpcChannel<ResponseTag> operator ()(
            BookServiceGetBooksViaAuthorCtx& /*ctx*/,
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
    concept c_book_service_get_greatest_book_unit = ::yazik::rpc::c_stream_reader_unit<
        Fn,
        GetBookRequestEntityRef,
        BookLayeredBuilder<BookServiceGetGreatestBookCtx::Tag>,
        BookServiceGetGreatestBookCtx
    >;

    struct BookServiceGetGreatestBookFunctor {
        using ResponseTag = typename BookServiceGetGreatestBookCtx::Tag;
        virtual ::yazik::rpc::RpcTask<ResponseTag> operator ()(
            BookServiceGetGreatestBookCtx& /*ctx*/,
            ::yazik::rpc::RpcChannel<GetBookRequestEntityRef>&& request
        ) {
            return ::yazik::rpc::RpcStatus::unimplemenmted()
                .as_broken_task<ResponseTag>();
        }
        virtual ~BookServiceGetGreatestBookFunctor() = default;
    };

}

