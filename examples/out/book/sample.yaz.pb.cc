#include <book/sample.yaz.pb.h>

namespace com::book::yaz {
    namespace author {
        ::yazik::string_view RefPbSpec::name(Author* p) {
            return p->name();
        }
        const RefVtable* RefPbSpec::vtable() {
            const static RefVtable s_vtable {
                .name = (RefVtable::name_fn)&RefPbSpec::name,
                .as_builder = (RefVtable::as_builder_fn)&RefPbSpec::as_builder,
                .serialize = (RefVtable::serialize_fn)&RefPbSpec::serialize,
            };
            return &s_vtable;
        }
        Builder RefPbSpec::as_builder(Author* p) {
            return BuilderPbSpec::wrap(*p);
        }
        std::string RefPbSpec::serialize(Author* p) {
            return p->SerializeAsString();
        }
        Ref RefPbSpec::wrap(Author& p) {
            return create_entity<Ref>(static_cast<void*>(&p), vtable());
        }
        void BuilderPbSpec::set_name(
            Author* p,
            std::string value
        ) {
            p->set_name(std::move(value));
        }
        const BuilderVtable* BuilderPbSpec::vtable() {
            const static BuilderVtable s_vtable {
                .set_name = (BuilderVtable::set_name_fn)&BuilderPbSpec::set_name,
                .move_initialize_from = (BuilderVtable::move_initialize_from_fn)&BuilderPbSpec::move_initialize_from,
                .deserialize = (BuilderVtable::deserialize_fn)&BuilderPbSpec::deserialize,
                .as_ref = (BuilderVtable::as_ref_fn)&BuilderPbSpec::as_ref,
            };
            return &s_vtable;
        }
        void BuilderPbSpec::move_initialize_from(
            Author* a,
            Author* b
        ) {
            a->Swap(b);
        }
        bool BuilderPbSpec::deserialize(
            Author* p,
            ::yazik::string_view data
        ) {
            return p->ParseFromArray(data.data(), data.size());
        }
        Ref BuilderPbSpec::as_ref(Author* p) {
            return create_entity<Ref>(static_cast<void*>(p), RefPbSpec::vtable());
        }
        Builder BuilderPbSpec::wrap(Author& p) {
            return create_builder<Builder>(static_cast<void*>(&p), vtable());
        }
    }
    namespace inside_one_of_message {
        ::yazik::string_view RefPbSpec::field(InsideOneOfMessage* p) {
            return p->field();
        }
        const RefVtable* RefPbSpec::vtable() {
            const static RefVtable s_vtable {
                .field = (RefVtable::field_fn)&RefPbSpec::field,
                .as_builder = (RefVtable::as_builder_fn)&RefPbSpec::as_builder,
                .serialize = (RefVtable::serialize_fn)&RefPbSpec::serialize,
            };
            return &s_vtable;
        }
        Builder RefPbSpec::as_builder(InsideOneOfMessage* p) {
            return BuilderPbSpec::wrap(*p);
        }
        std::string RefPbSpec::serialize(InsideOneOfMessage* p) {
            return p->SerializeAsString();
        }
        Ref RefPbSpec::wrap(InsideOneOfMessage& p) {
            return create_entity<Ref>(static_cast<void*>(&p), vtable());
        }
        void BuilderPbSpec::set_field(
            InsideOneOfMessage* p,
            std::string value
        ) {
            p->set_field(std::move(value));
        }
        const BuilderVtable* BuilderPbSpec::vtable() {
            const static BuilderVtable s_vtable {
                .set_field = (BuilderVtable::set_field_fn)&BuilderPbSpec::set_field,
                .move_initialize_from = (BuilderVtable::move_initialize_from_fn)&BuilderPbSpec::move_initialize_from,
                .deserialize = (BuilderVtable::deserialize_fn)&BuilderPbSpec::deserialize,
                .as_ref = (BuilderVtable::as_ref_fn)&BuilderPbSpec::as_ref,
            };
            return &s_vtable;
        }
        void BuilderPbSpec::move_initialize_from(
            InsideOneOfMessage* a,
            InsideOneOfMessage* b
        ) {
            a->Swap(b);
        }
        bool BuilderPbSpec::deserialize(
            InsideOneOfMessage* p,
            ::yazik::string_view data
        ) {
            return p->ParseFromArray(data.data(), data.size());
        }
        Ref BuilderPbSpec::as_ref(InsideOneOfMessage* p) {
            return create_entity<Ref>(static_cast<void*>(p), RefPbSpec::vtable());
        }
        Builder BuilderPbSpec::wrap(InsideOneOfMessage& p) {
            return create_builder<Builder>(static_cast<void*>(&p), vtable());
        }
    }
    namespace book {
        namespace page {
            ::yazik::compiler::support::repeated_type_t<::yazik::string_view> RefPbSpec::lines(Book_Page* p) {
                return ::ranges::views::all(p->lines())
                    | ::ranges::views::transform([](const auto& x) {
                        return ::yazik::string_view { x };
                    });
            }
            const RefVtable* RefPbSpec::vtable() {
                const static RefVtable s_vtable {
                    .lines = (RefVtable::lines_fn)&RefPbSpec::lines,
                    .as_builder = (RefVtable::as_builder_fn)&RefPbSpec::as_builder,
                    .serialize = (RefVtable::serialize_fn)&RefPbSpec::serialize,
                };
                return &s_vtable;
            }
            Builder RefPbSpec::as_builder(Book_Page* p) {
                return BuilderPbSpec::wrap(*p);
            }
            std::string RefPbSpec::serialize(Book_Page* p) {
                return p->SerializeAsString();
            }
            Ref RefPbSpec::wrap(Book_Page& p) {
                return create_entity<Ref>(static_cast<void*>(&p), vtable());
            }
            ::yazik::compiler::support::VecBuilder<std::string> BuilderPbSpec::construct_lines_builder(Book_Page* p) {
                return create_builder<::yazik::compiler::support::VecBuilder<std::string>>(
                    p,
                    [](void* ptr, auto value) {
                        ((Book_Page*)ptr)->add_lines(std::move(value));
                    }
                );
            }
            const BuilderVtable* BuilderPbSpec::vtable() {
                const static BuilderVtable s_vtable {
                    .construct_lines_builder = (BuilderVtable::construct_lines_builder_fn)&BuilderPbSpec::construct_lines_builder,
                    .move_initialize_from = (BuilderVtable::move_initialize_from_fn)&BuilderPbSpec::move_initialize_from,
                    .deserialize = (BuilderVtable::deserialize_fn)&BuilderPbSpec::deserialize,
                    .as_ref = (BuilderVtable::as_ref_fn)&BuilderPbSpec::as_ref,
                };
                return &s_vtable;
            }
            void BuilderPbSpec::move_initialize_from(
                Book_Page* a,
                Book_Page* b
            ) {
                a->Swap(b);
            }
            bool BuilderPbSpec::deserialize(
                Book_Page* p,
                ::yazik::string_view data
            ) {
                return p->ParseFromArray(data.data(), data.size());
            }
            Ref BuilderPbSpec::as_ref(Book_Page* p) {
                return create_entity<Ref>(static_cast<void*>(p), RefPbSpec::vtable());
            }
            Builder BuilderPbSpec::wrap(Book_Page& p) {
                return create_builder<Builder>(static_cast<void*>(&p), vtable());
            }
        }
        namespace first_oneof {
            bool RefPbSpec::is_first_oneof_string(const Book* p) {
                return p->first_oneof_case() == Book::kFirstOneofString;
            }
            ::yazik::string_view RefPbSpec::first_oneof_string(const Book* p) {
                return p->first_oneof_string();
            }
            bool RefPbSpec::is_first_oneof_int(const Book* p) {
                return p->first_oneof_case() == Book::kFirstOneofInt;
            }
            int64_t RefPbSpec::first_oneof_int(const Book* p) {
                return p->first_oneof_int();
            }
            bool RefPbSpec::is_null(const Book* p) {
                return p->first_oneof_case() == Book::FIRST_ONEOF_NOT_SET;
            }
            const RefVtable* RefPbSpec::vtable() {
                const static RefVtable s_vtable {
                    .is_first_oneof_string = (RefVtable::is_first_oneof_string_fn)&RefPbSpec::is_first_oneof_string,
                    .first_oneof_string = (RefVtable::first_oneof_string_fn)&RefPbSpec::first_oneof_string,
                    .is_first_oneof_int = (RefVtable::is_first_oneof_int_fn)&RefPbSpec::is_first_oneof_int,
                    .first_oneof_int = (RefVtable::first_oneof_int_fn)&RefPbSpec::first_oneof_int,
                    .is_null = (RefVtable::is_null_fn)&RefPbSpec::is_null,
                };
                return &s_vtable;
            }
            Ref RefPbSpec::wrap(Book& p) {
                return create_entity<Ref>(static_cast<void*>(&p), vtable());
            }
            void BuilderPbSpec::set_first_oneof_string(
                Book* p,
                std::string value
            ) {
                p->set_first_oneof_string(std::move(value));
            }
            void BuilderPbSpec::set_first_oneof_int(
                Book* p,
                int64_t value
            ) {
                p->set_first_oneof_int(std::move(value));
            }
            const BuilderVtable* BuilderPbSpec::vtable() {
                const static BuilderVtable s_vtable {
                    .set_first_oneof_string = (BuilderVtable::set_first_oneof_string_fn)&BuilderPbSpec::set_first_oneof_string,
                    .set_first_oneof_int = (BuilderVtable::set_first_oneof_int_fn)&BuilderPbSpec::set_first_oneof_int,
                };
                return &s_vtable;
            }
            Builder BuilderPbSpec::wrap(Book& p) {
                return create_builder<Builder>(static_cast<void*>(&p), vtable());
            }
        }
        namespace second_oneof {
            bool RefPbSpec::is_second_oneof_string(const Book* p) {
                return p->second_oneof_case() == Book::kSecondOneofString;
            }
            ::yazik::string_view RefPbSpec::second_oneof_string(const Book* p) {
                return p->second_oneof_string();
            }
            bool RefPbSpec::is_second_oneof_int(const Book* p) {
                return p->second_oneof_case() == Book::kSecondOneofInt;
            }
            int64_t RefPbSpec::second_oneof_int(const Book* p) {
                return p->second_oneof_int();
            }
            bool RefPbSpec::is_second_oneof_message(const Book* p) {
                return p->second_oneof_case() == Book::kSecondOneofMessage;
            }
            inside_one_of_message::Ref RefPbSpec::second_oneof_message(const Book* p) {
                return inside_one_of_message::RefPbSpec::wrap(*((Book*)p)->mutable_second_oneof_message());
            }
            bool RefPbSpec::is_null(const Book* p) {
                return p->second_oneof_case() == Book::SECOND_ONEOF_NOT_SET;
            }
            const RefVtable* RefPbSpec::vtable() {
                const static RefVtable s_vtable {
                    .is_second_oneof_string = (RefVtable::is_second_oneof_string_fn)&RefPbSpec::is_second_oneof_string,
                    .second_oneof_string = (RefVtable::second_oneof_string_fn)&RefPbSpec::second_oneof_string,
                    .is_second_oneof_int = (RefVtable::is_second_oneof_int_fn)&RefPbSpec::is_second_oneof_int,
                    .second_oneof_int = (RefVtable::second_oneof_int_fn)&RefPbSpec::second_oneof_int,
                    .is_second_oneof_message = (RefVtable::is_second_oneof_message_fn)&RefPbSpec::is_second_oneof_message,
                    .second_oneof_message = (RefVtable::second_oneof_message_fn)&RefPbSpec::second_oneof_message,
                    .is_null = (RefVtable::is_null_fn)&RefPbSpec::is_null,
                };
                return &s_vtable;
            }
            Ref RefPbSpec::wrap(Book& p) {
                return create_entity<Ref>(static_cast<void*>(&p), vtable());
            }
            void BuilderPbSpec::set_second_oneof_string(
                Book* p,
                std::string value
            ) {
                p->set_second_oneof_string(std::move(value));
            }
            void BuilderPbSpec::set_second_oneof_int(
                Book* p,
                int64_t value
            ) {
                p->set_second_oneof_int(std::move(value));
            }
            void BuilderPbSpec::set_second_oneof_message(
                Book* p,
                ::yazik::unique_function<void (inside_one_of_message::Builder&)> clbk
            ) {
                auto builder = inside_one_of_message::BuilderPbSpec::wrap(*((Book*)p)->mutable_second_oneof_message());
                clbk(builder);
            }
            const BuilderVtable* BuilderPbSpec::vtable() {
                const static BuilderVtable s_vtable {
                    .set_second_oneof_string = (BuilderVtable::set_second_oneof_string_fn)&BuilderPbSpec::set_second_oneof_string,
                    .set_second_oneof_int = (BuilderVtable::set_second_oneof_int_fn)&BuilderPbSpec::set_second_oneof_int,
                    .set_second_oneof_message = (BuilderVtable::set_second_oneof_message_fn)&BuilderPbSpec::set_second_oneof_message,
                };
                return &s_vtable;
            }
            Builder BuilderPbSpec::wrap(Book& p) {
                return create_builder<Builder>(static_cast<void*>(&p), vtable());
            }
        }
        int64_t RefPbSpec::isbn(Book* p) {
            return p->isbn();
        }
        ::yazik::string_view RefPbSpec::title(Book* p) {
            return p->title();
        }
        author::Ref RefPbSpec::author(Book* p) {
            return author::RefPbSpec::wrap(*((Book*)p)->mutable_author());
        }
        ::yazik::compiler::support::repeated_type_t<book::page::Ref> RefPbSpec::pages(Book* p) {
            return ::ranges::views::all(*p->mutable_pages())
                | ::ranges::views::transform([](Book_Page& x) {
                    return book::page::RefPbSpec::wrap(x);
                });
        }
        book::first_oneof::Ref RefPbSpec::first_oneof(Book* p) {
            return book::first_oneof::RefPbSpec::wrap(*p);
        }
        book::availability::Enum RefPbSpec::availability(Book* p) {
            return create_enum<book::availability::Enum>((int)p->availability());
        }
        can_recommend::Enum RefPbSpec::recommended(Book* p) {
            return create_enum<can_recommend::Enum>((int)p->recommended());
        }
        book::second_oneof::Ref RefPbSpec::second_oneof(Book* p) {
            return book::second_oneof::RefPbSpec::wrap(*p);
        }
        const RefVtable* RefPbSpec::vtable() {
            const static RefVtable s_vtable {
                .isbn = (RefVtable::isbn_fn)&RefPbSpec::isbn,
                .title = (RefVtable::title_fn)&RefPbSpec::title,
                .author = (RefVtable::author_fn)&RefPbSpec::author,
                .pages = (RefVtable::pages_fn)&RefPbSpec::pages,
                .first_oneof = (RefVtable::first_oneof_fn)&RefPbSpec::first_oneof,
                .availability = (RefVtable::availability_fn)&RefPbSpec::availability,
                .recommended = (RefVtable::recommended_fn)&RefPbSpec::recommended,
                .second_oneof = (RefVtable::second_oneof_fn)&RefPbSpec::second_oneof,
                .as_builder = (RefVtable::as_builder_fn)&RefPbSpec::as_builder,
                .serialize = (RefVtable::serialize_fn)&RefPbSpec::serialize,
            };
            return &s_vtable;
        }
        Builder RefPbSpec::as_builder(Book* p) {
            return BuilderPbSpec::wrap(*p);
        }
        std::string RefPbSpec::serialize(Book* p) {
            return p->SerializeAsString();
        }
        Ref RefPbSpec::wrap(Book& p) {
            return create_entity<Ref>(static_cast<void*>(&p), vtable());
        }
        void BuilderPbSpec::set_isbn(
            Book* p,
            int64_t value
        ) {
            p->set_isbn(std::move(value));
        }
        void BuilderPbSpec::set_title(
            Book* p,
            std::string value
        ) {
            p->set_title(std::move(value));
        }
        author::Builder BuilderPbSpec::construct_author_builder(Book* p) {
            return author::BuilderPbSpec::wrap(*((Book*)p)->mutable_author());
        }
        ::yazik::compiler::support::VecEntityBuilder<book::page::Builder> BuilderPbSpec::construct_pages_builder(Book* p) {
            return create_builder<::yazik::compiler::support::VecEntityBuilder<book::page::Builder>>(
                p,
                [](void* ptr) -> void* {
                    return (void*)((Book*)ptr)->mutable_pages()->Add();
                },
                book::page::BuilderPbSpec::vtable()
            );
        }
        book::first_oneof::Builder BuilderPbSpec::construct_first_oneof_builder(Book* p) {
            return book::first_oneof::BuilderPbSpec::wrap(*p);
        }
        void BuilderPbSpec::set_availability(
            Book* p,
            book::availability::Enum value
        ) {
            p->set_availability((Book_Availability)value.which());
        }
        void BuilderPbSpec::set_recommended(
            Book* p,
            can_recommend::Enum value
        ) {
            p->set_recommended((CanRecommend)value.which());
        }
        book::second_oneof::Builder BuilderPbSpec::construct_second_oneof_builder(Book* p) {
            return book::second_oneof::BuilderPbSpec::wrap(*p);
        }
        const BuilderVtable* BuilderPbSpec::vtable() {
            const static BuilderVtable s_vtable {
                .set_isbn = (BuilderVtable::set_isbn_fn)&BuilderPbSpec::set_isbn,
                .set_title = (BuilderVtable::set_title_fn)&BuilderPbSpec::set_title,
                .construct_author_builder = (BuilderVtable::construct_author_builder_fn)&BuilderPbSpec::construct_author_builder,
                .construct_pages_builder = (BuilderVtable::construct_pages_builder_fn)&BuilderPbSpec::construct_pages_builder,
                .construct_first_oneof_builder = (BuilderVtable::construct_first_oneof_builder_fn)&BuilderPbSpec::construct_first_oneof_builder,
                .set_availability = (BuilderVtable::set_availability_fn)&BuilderPbSpec::set_availability,
                .set_recommended = (BuilderVtable::set_recommended_fn)&BuilderPbSpec::set_recommended,
                .construct_second_oneof_builder = (BuilderVtable::construct_second_oneof_builder_fn)&BuilderPbSpec::construct_second_oneof_builder,
                .move_initialize_from = (BuilderVtable::move_initialize_from_fn)&BuilderPbSpec::move_initialize_from,
                .deserialize = (BuilderVtable::deserialize_fn)&BuilderPbSpec::deserialize,
                .as_ref = (BuilderVtable::as_ref_fn)&BuilderPbSpec::as_ref,
            };
            return &s_vtable;
        }
        void BuilderPbSpec::move_initialize_from(
            Book* a,
            Book* b
        ) {
            a->Swap(b);
        }
        bool BuilderPbSpec::deserialize(
            Book* p,
            ::yazik::string_view data
        ) {
            return p->ParseFromArray(data.data(), data.size());
        }
        Ref BuilderPbSpec::as_ref(Book* p) {
            return create_entity<Ref>(static_cast<void*>(p), RefPbSpec::vtable());
        }
        Builder BuilderPbSpec::wrap(Book& p) {
            return create_builder<Builder>(static_cast<void*>(&p), vtable());
        }
    }
    namespace get_book_request {
        int64_t RefPbSpec::isbn(GetBookRequest* p) {
            return p->isbn();
        }
        const RefVtable* RefPbSpec::vtable() {
            const static RefVtable s_vtable {
                .isbn = (RefVtable::isbn_fn)&RefPbSpec::isbn,
                .as_builder = (RefVtable::as_builder_fn)&RefPbSpec::as_builder,
                .serialize = (RefVtable::serialize_fn)&RefPbSpec::serialize,
            };
            return &s_vtable;
        }
        Builder RefPbSpec::as_builder(GetBookRequest* p) {
            return BuilderPbSpec::wrap(*p);
        }
        std::string RefPbSpec::serialize(GetBookRequest* p) {
            return p->SerializeAsString();
        }
        Ref RefPbSpec::wrap(GetBookRequest& p) {
            return create_entity<Ref>(static_cast<void*>(&p), vtable());
        }
        void BuilderPbSpec::set_isbn(
            GetBookRequest* p,
            int64_t value
        ) {
            p->set_isbn(std::move(value));
        }
        const BuilderVtable* BuilderPbSpec::vtable() {
            const static BuilderVtable s_vtable {
                .set_isbn = (BuilderVtable::set_isbn_fn)&BuilderPbSpec::set_isbn,
                .move_initialize_from = (BuilderVtable::move_initialize_from_fn)&BuilderPbSpec::move_initialize_from,
                .deserialize = (BuilderVtable::deserialize_fn)&BuilderPbSpec::deserialize,
                .as_ref = (BuilderVtable::as_ref_fn)&BuilderPbSpec::as_ref,
            };
            return &s_vtable;
        }
        void BuilderPbSpec::move_initialize_from(
            GetBookRequest* a,
            GetBookRequest* b
        ) {
            a->Swap(b);
        }
        bool BuilderPbSpec::deserialize(
            GetBookRequest* p,
            ::yazik::string_view data
        ) {
            return p->ParseFromArray(data.data(), data.size());
        }
        Ref BuilderPbSpec::as_ref(GetBookRequest* p) {
            return create_entity<Ref>(static_cast<void*>(p), RefPbSpec::vtable());
        }
        Builder BuilderPbSpec::wrap(GetBookRequest& p) {
            return create_builder<Builder>(static_cast<void*>(&p), vtable());
        }
    }
    namespace get_book_via_author {
        ::yazik::string_view RefPbSpec::author(GetBookViaAuthor* p) {
            return p->author();
        }
        const RefVtable* RefPbSpec::vtable() {
            const static RefVtable s_vtable {
                .author = (RefVtable::author_fn)&RefPbSpec::author,
                .as_builder = (RefVtable::as_builder_fn)&RefPbSpec::as_builder,
                .serialize = (RefVtable::serialize_fn)&RefPbSpec::serialize,
            };
            return &s_vtable;
        }
        Builder RefPbSpec::as_builder(GetBookViaAuthor* p) {
            return BuilderPbSpec::wrap(*p);
        }
        std::string RefPbSpec::serialize(GetBookViaAuthor* p) {
            return p->SerializeAsString();
        }
        Ref RefPbSpec::wrap(GetBookViaAuthor& p) {
            return create_entity<Ref>(static_cast<void*>(&p), vtable());
        }
        void BuilderPbSpec::set_author(
            GetBookViaAuthor* p,
            std::string value
        ) {
            p->set_author(std::move(value));
        }
        const BuilderVtable* BuilderPbSpec::vtable() {
            const static BuilderVtable s_vtable {
                .set_author = (BuilderVtable::set_author_fn)&BuilderPbSpec::set_author,
                .move_initialize_from = (BuilderVtable::move_initialize_from_fn)&BuilderPbSpec::move_initialize_from,
                .deserialize = (BuilderVtable::deserialize_fn)&BuilderPbSpec::deserialize,
                .as_ref = (BuilderVtable::as_ref_fn)&BuilderPbSpec::as_ref,
            };
            return &s_vtable;
        }
        void BuilderPbSpec::move_initialize_from(
            GetBookViaAuthor* a,
            GetBookViaAuthor* b
        ) {
            a->Swap(b);
        }
        bool BuilderPbSpec::deserialize(
            GetBookViaAuthor* p,
            ::yazik::string_view data
        ) {
            return p->ParseFromArray(data.data(), data.size());
        }
        Ref BuilderPbSpec::as_ref(GetBookViaAuthor* p) {
            return create_entity<Ref>(static_cast<void*>(p), RefPbSpec::vtable());
        }
        Builder BuilderPbSpec::wrap(GetBookViaAuthor& p) {
            return create_builder<Builder>(static_cast<void*>(&p), vtable());
        }
    }
    namespace book_store {
        ::yazik::string_view RefPbSpec::name(BookStore* p) {
            return p->name();
        }
        const RefVtable* RefPbSpec::vtable() {
            const static RefVtable s_vtable {
                .name = (RefVtable::name_fn)&RefPbSpec::name,
                .as_builder = (RefVtable::as_builder_fn)&RefPbSpec::as_builder,
                .serialize = (RefVtable::serialize_fn)&RefPbSpec::serialize,
            };
            return &s_vtable;
        }
        Builder RefPbSpec::as_builder(BookStore* p) {
            return BuilderPbSpec::wrap(*p);
        }
        std::string RefPbSpec::serialize(BookStore* p) {
            return p->SerializeAsString();
        }
        Ref RefPbSpec::wrap(BookStore& p) {
            return create_entity<Ref>(static_cast<void*>(&p), vtable());
        }
        void BuilderPbSpec::set_name(
            BookStore* p,
            std::string value
        ) {
            p->set_name(std::move(value));
        }
        const BuilderVtable* BuilderPbSpec::vtable() {
            const static BuilderVtable s_vtable {
                .set_name = (BuilderVtable::set_name_fn)&BuilderPbSpec::set_name,
                .move_initialize_from = (BuilderVtable::move_initialize_from_fn)&BuilderPbSpec::move_initialize_from,
                .deserialize = (BuilderVtable::deserialize_fn)&BuilderPbSpec::deserialize,
                .as_ref = (BuilderVtable::as_ref_fn)&BuilderPbSpec::as_ref,
            };
            return &s_vtable;
        }
        void BuilderPbSpec::move_initialize_from(
            BookStore* a,
            BookStore* b
        ) {
            a->Swap(b);
        }
        bool BuilderPbSpec::deserialize(
            BookStore* p,
            ::yazik::string_view data
        ) {
            return p->ParseFromArray(data.data(), data.size());
        }
        Ref BuilderPbSpec::as_ref(BookStore* p) {
            return create_entity<Ref>(static_cast<void*>(p), RefPbSpec::vtable());
        }
        Builder BuilderPbSpec::wrap(BookStore& p) {
            return create_builder<Builder>(static_cast<void*>(&p), vtable());
        }
    }
    namespace cycle_dep1 {
        cycle_dep2::Ref RefPbSpec::dep(CycleDep1* p) {
            return cycle_dep2::RefPbSpec::wrap(*((CycleDep1*)p)->mutable_dep());
        }
        const RefVtable* RefPbSpec::vtable() {
            const static RefVtable s_vtable {
                .dep = (RefVtable::dep_fn)&RefPbSpec::dep,
                .as_builder = (RefVtable::as_builder_fn)&RefPbSpec::as_builder,
                .serialize = (RefVtable::serialize_fn)&RefPbSpec::serialize,
            };
            return &s_vtable;
        }
        Builder RefPbSpec::as_builder(CycleDep1* p) {
            return BuilderPbSpec::wrap(*p);
        }
        std::string RefPbSpec::serialize(CycleDep1* p) {
            return p->SerializeAsString();
        }
        Ref RefPbSpec::wrap(CycleDep1& p) {
            return create_entity<Ref>(static_cast<void*>(&p), vtable());
        }
        cycle_dep2::Builder BuilderPbSpec::construct_dep_builder(CycleDep1* p) {
            return cycle_dep2::BuilderPbSpec::wrap(*((CycleDep1*)p)->mutable_dep());
        }
        const BuilderVtable* BuilderPbSpec::vtable() {
            const static BuilderVtable s_vtable {
                .construct_dep_builder = (BuilderVtable::construct_dep_builder_fn)&BuilderPbSpec::construct_dep_builder,
                .move_initialize_from = (BuilderVtable::move_initialize_from_fn)&BuilderPbSpec::move_initialize_from,
                .deserialize = (BuilderVtable::deserialize_fn)&BuilderPbSpec::deserialize,
                .as_ref = (BuilderVtable::as_ref_fn)&BuilderPbSpec::as_ref,
            };
            return &s_vtable;
        }
        void BuilderPbSpec::move_initialize_from(
            CycleDep1* a,
            CycleDep1* b
        ) {
            a->Swap(b);
        }
        bool BuilderPbSpec::deserialize(
            CycleDep1* p,
            ::yazik::string_view data
        ) {
            return p->ParseFromArray(data.data(), data.size());
        }
        Ref BuilderPbSpec::as_ref(CycleDep1* p) {
            return create_entity<Ref>(static_cast<void*>(p), RefPbSpec::vtable());
        }
        Builder BuilderPbSpec::wrap(CycleDep1& p) {
            return create_builder<Builder>(static_cast<void*>(&p), vtable());
        }
    }
    namespace cycle_dep2 {
        cycle_dep1::Ref RefPbSpec::dep(CycleDep2* p) {
            return cycle_dep1::RefPbSpec::wrap(*((CycleDep2*)p)->mutable_dep());
        }
        const RefVtable* RefPbSpec::vtable() {
            const static RefVtable s_vtable {
                .dep = (RefVtable::dep_fn)&RefPbSpec::dep,
                .as_builder = (RefVtable::as_builder_fn)&RefPbSpec::as_builder,
                .serialize = (RefVtable::serialize_fn)&RefPbSpec::serialize,
            };
            return &s_vtable;
        }
        Builder RefPbSpec::as_builder(CycleDep2* p) {
            return BuilderPbSpec::wrap(*p);
        }
        std::string RefPbSpec::serialize(CycleDep2* p) {
            return p->SerializeAsString();
        }
        Ref RefPbSpec::wrap(CycleDep2& p) {
            return create_entity<Ref>(static_cast<void*>(&p), vtable());
        }
        cycle_dep1::Builder BuilderPbSpec::construct_dep_builder(CycleDep2* p) {
            return cycle_dep1::BuilderPbSpec::wrap(*((CycleDep2*)p)->mutable_dep());
        }
        const BuilderVtable* BuilderPbSpec::vtable() {
            const static BuilderVtable s_vtable {
                .construct_dep_builder = (BuilderVtable::construct_dep_builder_fn)&BuilderPbSpec::construct_dep_builder,
                .move_initialize_from = (BuilderVtable::move_initialize_from_fn)&BuilderPbSpec::move_initialize_from,
                .deserialize = (BuilderVtable::deserialize_fn)&BuilderPbSpec::deserialize,
                .as_ref = (BuilderVtable::as_ref_fn)&BuilderPbSpec::as_ref,
            };
            return &s_vtable;
        }
        void BuilderPbSpec::move_initialize_from(
            CycleDep2* a,
            CycleDep2* b
        ) {
            a->Swap(b);
        }
        bool BuilderPbSpec::deserialize(
            CycleDep2* p,
            ::yazik::string_view data
        ) {
            return p->ParseFromArray(data.data(), data.size());
        }
        Ref BuilderPbSpec::as_ref(CycleDep2* p) {
            return create_entity<Ref>(static_cast<void*>(p), RefPbSpec::vtable());
        }
        Builder BuilderPbSpec::wrap(CycleDep2& p) {
            return create_builder<Builder>(static_cast<void*>(&p), vtable());
        }
    }
}
