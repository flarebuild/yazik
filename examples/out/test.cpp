#include <book/sample.yaz.pb.h>
#include <book/sample.yaz.grpc.h>
#include <grpcpp/grpcpp.h>
#include <yazik/utility/logging.hpp>
#include <yazik/rpc/grpc/grpc_runtime.hpp>

using namespace ::com::book;
using namespace ::yazik;
using namespace ::yazik::compiler::support;

void fill(BookBuilder builder) {
    builder.set_isbn(42);
    builder.set_title("So uniq title");
    builder.set_author([](AuthorBuilder& author) {
        author.set_name("Arthur Bookman");
    });
    builder.set_pages([](VecEntityBuilder<BookPageBuilder>& pages) {
        pages.push_back([](BookPageBuilder& page) {
            page.set_lines([](VecBuilder<std::string>& lines){
                lines.push_back("first line");
                lines.push_back("second line");
                lines.push_back("third line");
            });
        });
        pages.push_back([](BookPageBuilder& page) {
            page.set_lines([](VecBuilder<std::string>& lines){
                lines.push_back("first line");
                lines.push_back("second line");
                lines.push_back("third line");
            });
        });
        pages.push_back([](BookPageBuilder& page) {
            page.set_lines([](VecBuilder<std::string>& lines){
                lines.push_back("first line");
                lines.push_back("second line");
                lines.push_back("third line");
            });
        });
    });
    builder.set_first_oneof([](BookFirstOneofVariantBuilder& first_oneof) {
        first_oneof.set_first_oneof_int(42);
    });
    builder.set_availability(BookAvailabilityEnum::Available);
    builder.set_recommended(CanRecommendEnum::Nope);
    builder.set_second_oneof([](BookSecondOneofVariantBuilder& second_oneof) {
        second_oneof.set_second_oneof_string("second_oneof_string");
    });
}

template<typename T>
void fill_layered(BookLayeredBuilder<T>& builder) {
    builder
        .isbn(42)
        .title("So uniq title")
        .author()
            .name("Arthur Bookman")
            .done()
        .pages()
            .push()
                .lines()
                    .push("first line")
                    .push("second line")
                    .push("third line")
                    .done()
                .done()
            .push()
                .lines()
                    .push("first line")
                    .push("second line")
                    .push("third line")
                    .done()
                .done()
            .push()
                .lines()
                    .push("first line")
                    .push("second line")
                    .push("third line")
                    .done()
                .done()
            .done()
        .availability(BookAvailabilityEnum::Available)
        .recommended(CanRecommendEnum::Nope)
    ;
}

void print(BookEntityRef book) {
    L::debug("isbn: {}", book.isbn());
    L::debug("title: {}", book.title());
    L::debug("author: {}", book.author().name());
    L::debug("pages:");
    int page_count = 0;
    for (auto page: book.pages()) {
        L::debug("  - {}:", ++page_count);
        for(auto line: page.lines())
            L::debug("    - {}", line);
    }
    L::debug(
        "first_oneof: {}",
        [&](BookFirstOneofVariantRef one_of) {
            if (one_of.is_first_oneof_int())
                return do_format("int - {}", one_of.first_oneof_int());
            else
                return do_format("string - {}", one_of.first_oneof_string());
        }(book.first_oneof())
    );
    L::debug(
        "availability: {}",
        book.availability().is_available()
            ? "Available"
            : "NotAvailable"
    );
    L::debug(
        "recommend: {}",
        book.recommended().is_yep()
            ? "Yep"
            : "Nope"
    );
    L::debug(
        "second_oneof: {}",
        [&](BookSecondOneofVariantRef one_of) {
            if (one_of.is_second_oneof_int())
                return do_format("int - {}", one_of.second_oneof_int());
            else
                return do_format("string - {}", one_of.second_oneof_string());
        }(book.second_oneof())
    );
}

struct GetBookSync: BookServiceGetBookSyncFunctor {
    ::yazik::rpc::RpcResult<ResponseTag> operator () (
        BookServiceGetBookCtx& ctx,
        GetBookRequestEntityRef&& request
    ) override {
        co_return ctx.response
            .isbn(42)
            .title("So uniq title")
            .author()
                .name("Arthur Bookman")
                .done()
            .pages()
                .push()
                    .lines()
                        .push("first line")
                        .push("second line")
                        .push("third line")
                        .done()
                    .done()
                .push()
                    .lines()
                        .push("first line")
                        .push("second line")
                        .push("third line")
                        .done()
                    .done()
                .push()
                    .lines()
                        .push("first line")
                        .push("second line")
                        .push("third line")
                        .done()
                    .done()
                .done()
            .availability(BookAvailabilityEnum::Available)
            .recommended(CanRecommendEnum::Nope)
            .done();
    }
};

int main() {
    {
        auto book = Book{};
        fill(BookPbBuilderSpec::wrap(book));
        print(BookPbSpec::wrap(book));
    }
    L::debug("#########################");
    {
        auto book = Book{};
        compiler::support::Initializer i;
        ::folly::Unit unit;
        auto book_wrap = Initializer::create_builder<
            BookLayeredBuilder<::folly::Unit>
        >(std::make_tuple(
            (void *) &book,
            BookPbBuilderSpec::vtable(),
            &unit
        ));
        fill_layered(book_wrap);
        print(book_wrap.as_ref());
    }
    {
        rpc::grpc::Runtime runtime;
        auto& worker = runtime.add_worker();
        static_assert(c_book_service_get_book_sync_unit<BookServiceGetBookSyncFunctor>);
        worker->spawn<BookServiceGetBookGrpcHandle>(BookServiceGetBookSyncFunctor{});
        runtime.start().wait();
    }
    return 0;
}