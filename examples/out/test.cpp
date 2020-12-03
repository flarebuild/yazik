#include <book/sample.yaz.pb.h>
#include <book/sample.yaz.rpc.h>
#include <book/sample.yaz.grpc.h>
#include <grpcpp/grpcpp.h>
#include <yazik/utility/logging.hpp>
#include <yazik/rpc/grpc/grpc_runtime.hpp>

using namespace ::com::book;
using namespace ::com::book::yaz;
using namespace ::yazik;
using namespace ::yazik::compiler::support;

void fill(book::Builder builder) {
    builder.set_isbn(42);
    builder.set_title("So uniq title");
    builder.set_author([](author::Builder& author) {
        author.set_name("Arthur Bookman");
    });
    builder.set_pages([](VecEntityBuilder<book::page::Builder>& pages) {
        pages.push_back([](book::page::Builder& page) {
            page.set_lines([](VecBuilder<std::string>& lines){
                lines.push_back("first line");
                lines.push_back("second line");
                lines.push_back("third line");
            });
        });
        pages.push_back([](book::page::Builder& page) {
            page.set_lines([](VecBuilder<std::string>& lines){
                lines.push_back("first line");
                lines.push_back("second line");
                lines.push_back("third line");
            });
        });
        pages.push_back([](book::page::Builder& page) {
            page.set_lines([](VecBuilder<std::string>& lines){
                lines.push_back("first line");
                lines.push_back("second line");
                lines.push_back("third line");
            });
        });
    });
    builder.set_first_oneof([](book::first_oneof::Builder& first_oneof) {
        first_oneof.set_first_oneof_int(42);
    });
    builder.set_availability(book::availability::Enum::Available);
    builder.set_recommended(can_recommend::Enum::Nope);
    builder.set_second_oneof([](book::second_oneof::Builder& second_oneof) {
        second_oneof.set_second_oneof_string("second_oneof_string");
    });
}

template<typename T>
void fill_layered(book::LayeredBuilder<T>& builder) {
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
        .availability(book::availability::Enum::Available)
        .recommended(can_recommend::Enum::Nope)
    ;
}

void print(book::Ref book) {
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
        [&](book::first_oneof::Ref one_of) {
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
        [&](book::second_oneof::Ref one_of) {
            if (one_of.is_second_oneof_int())
                return do_format("int - {}", one_of.second_oneof_int());
            else
                return do_format("string - {}", one_of.second_oneof_string());
        }(book.second_oneof())
    );
}

struct GetBookSync: book_service::get_book::SyncCall {
    ::yazik::rpc::RpcResult<::yazik::rpc::RespOk<book::Ref>> call(
        book_service::get_book::Ctx& ctx,
        get_book_request::Ref request
    ) {
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
            .availability(book::availability::Enum::Available)
            .recommended(can_recommend::Enum::Nope)
            .done();
    }
};

struct SyncCallSample: book_service::get_book::SyncCall {
    ::yazik::rpc::RpcResult<::yazik::rpc::RespOk<book::Ref>> call(
        context_t context,
        get_book_request::Ref request
    ) noexcept {
        co_return context.response.done();
    }
};

int main() {
    {
        auto book = Book{};
        fill(book::BuilderPbSpec::wrap(book));
        print(book::RefPbSpec::wrap(book));
    }
    L::debug("#########################");
    {
        auto book = Book{};
        compiler::support::Initializer i;
        ::folly::Unit unit;
        auto book_wrap = Initializer::create_builder<
            book::LayeredBuilder<::folly::Unit>
        >(
            (void *) &book,
            book::BuilderPbSpec::vtable(),
            &unit
        );
        fill_layered(book_wrap);
        print(book_wrap.as_ref());
    }
    {
        rpc::grpc::Runtime runtime;
        auto& worker = runtime.add_worker();
        static_assert(book_service::get_book::c_sync_unit<SyncCallSample>);
        worker->spawn<book_service::get_book::GrpcHandle>(SyncCallSample{});
        runtime.start().wait();
    }
    return 0;
}