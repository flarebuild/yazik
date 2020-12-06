#pragma once

#include <book/sample.yaz.h>
#include <book/sample.pb.h>

namespace com::book::yaz {
    namespace author {
        struct RefPbSpec: protected ::yazik::compiler::support::Initializer {
            [[nodiscard]] static ::yazik::string_view name(Author*);
            [[nodiscard]] static const RefVtable* vtable();
            [[nodiscard]] static Builder as_builder(Author*);
            [[nodiscard]] static std::string serialize(Author*);
            [[nodiscard]] static Ref wrap(Author&);
        };
        struct BuilderPbSpec: protected ::yazik::compiler::support::Initializer {
            static void set_name(
                Author*,
                std::string
            );
            static const BuilderVtable* vtable();
            static void move_initialize_from(
                Author*,
                Author*
            );
            [[nodiscard]] static bool deserialize(
                Author*,
                ::yazik::string_view
            );
            [[nodiscard]] static Ref as_ref(Author*);
            static Builder wrap(Author&);
        };
    }
    namespace inside_one_of_message {
        struct RefPbSpec: protected ::yazik::compiler::support::Initializer {
            [[nodiscard]] static ::yazik::string_view field(InsideOneOfMessage*);
            [[nodiscard]] static const RefVtable* vtable();
            [[nodiscard]] static Builder as_builder(InsideOneOfMessage*);
            [[nodiscard]] static std::string serialize(InsideOneOfMessage*);
            [[nodiscard]] static Ref wrap(InsideOneOfMessage&);
        };
        struct BuilderPbSpec: protected ::yazik::compiler::support::Initializer {
            static void set_field(
                InsideOneOfMessage*,
                std::string
            );
            static const BuilderVtable* vtable();
            static void move_initialize_from(
                InsideOneOfMessage*,
                InsideOneOfMessage*
            );
            [[nodiscard]] static bool deserialize(
                InsideOneOfMessage*,
                ::yazik::string_view
            );
            [[nodiscard]] static Ref as_ref(InsideOneOfMessage*);
            static Builder wrap(InsideOneOfMessage&);
        };
    }
    namespace book {
        namespace page {
            struct RefPbSpec: protected ::yazik::compiler::support::Initializer {
                [[nodiscard]] static ::yazik::compiler::support::repeated_type_t<::yazik::string_view> lines(Book_Page*);
                [[nodiscard]] static const RefVtable* vtable();
                [[nodiscard]] static Builder as_builder(Book_Page*);
                [[nodiscard]] static std::string serialize(Book_Page*);
                [[nodiscard]] static Ref wrap(Book_Page&);
            };
            struct BuilderPbSpec: protected ::yazik::compiler::support::Initializer {
                [[nodiscard]] static ::yazik::compiler::support::VecBuilder<std::string> construct_lines_builder(Book_Page*);
                static const BuilderVtable* vtable();
                static void move_initialize_from(
                    Book_Page*,
                    Book_Page*
                );
                [[nodiscard]] static bool deserialize(
                    Book_Page*,
                    ::yazik::string_view
                );
                [[nodiscard]] static Ref as_ref(Book_Page*);
                static Builder wrap(Book_Page&);
            };
        }
        namespace first_oneof {
            struct RefPbSpec: protected ::yazik::compiler::support::Initializer {
                [[nodiscard]] static bool is_first_oneof_string(const Book*);
                [[nodiscard]] static ::yazik::string_view first_oneof_string(const Book*);
                [[nodiscard]] static bool is_first_oneof_int(const Book*);
                [[nodiscard]] static int64_t first_oneof_int(const Book*);
                [[nodiscard]] static bool is_null(const Book*);
                [[nodiscard]] static const RefVtable* vtable();
                [[nodiscard]] static Ref wrap(Book&);
            };
            struct BuilderPbSpec: protected ::yazik::compiler::support::Initializer {
                static void set_first_oneof_string(
                    Book*,
                    std::string
                );
                static void set_first_oneof_int(
                    Book*,
                    int64_t
                );
                static const BuilderVtable* vtable();
                static Builder wrap(Book&);
            };
        }
        namespace second_oneof {
            struct RefPbSpec: protected ::yazik::compiler::support::Initializer {
                [[nodiscard]] static bool is_second_oneof_string(const Book*);
                [[nodiscard]] static ::yazik::string_view second_oneof_string(const Book*);
                [[nodiscard]] static bool is_second_oneof_int(const Book*);
                [[nodiscard]] static int64_t second_oneof_int(const Book*);
                [[nodiscard]] static bool is_second_oneof_message(const Book*);
                [[nodiscard]] static inside_one_of_message::Ref second_oneof_message(const Book*);
                [[nodiscard]] static bool is_null(const Book*);
                [[nodiscard]] static const RefVtable* vtable();
                [[nodiscard]] static Ref wrap(Book&);
            };
            struct BuilderPbSpec: protected ::yazik::compiler::support::Initializer {
                static void set_second_oneof_string(
                    Book*,
                    std::string
                );
                static void set_second_oneof_int(
                    Book*,
                    int64_t
                );
                static void set_second_oneof_message(
                    Book*,
                    ::yazik::unique_function<void (inside_one_of_message::Builder&)>
                );
                static const BuilderVtable* vtable();
                static Builder wrap(Book&);
            };
        }
        struct RefPbSpec: protected ::yazik::compiler::support::Initializer {
            [[nodiscard]] static int64_t isbn(Book*);
            [[nodiscard]] static ::yazik::string_view title(Book*);
            [[nodiscard]] static author::Ref author(Book*);
            [[nodiscard]] static ::yazik::compiler::support::repeated_type_t<book::page::Ref> pages(Book*);
            [[nodiscard]] static book::first_oneof::Ref first_oneof(Book*);
            [[nodiscard]] static book::availability::Enum availability(Book*);
            [[nodiscard]] static can_recommend::Enum recommended(Book*);
            [[nodiscard]] static book::second_oneof::Ref second_oneof(Book*);
            [[nodiscard]] static const RefVtable* vtable();
            [[nodiscard]] static Builder as_builder(Book*);
            [[nodiscard]] static std::string serialize(Book*);
            [[nodiscard]] static Ref wrap(Book&);
        };
        struct BuilderPbSpec: protected ::yazik::compiler::support::Initializer {
            static void set_isbn(
                Book*,
                int64_t
            );
            static void set_title(
                Book*,
                std::string
            );
            [[nodiscard]] static author::Builder construct_author_builder(Book*);
            [[nodiscard]] static ::yazik::compiler::support::VecEntityBuilder<book::page::Builder> construct_pages_builder(Book*);
            [[nodiscard]] static book::first_oneof::Builder construct_first_oneof_builder(Book*);
            static void set_availability(
                Book*,
                book::availability::Enum
            );
            static void set_recommended(
                Book*,
                can_recommend::Enum
            );
            [[nodiscard]] static book::second_oneof::Builder construct_second_oneof_builder(Book*);
            static const BuilderVtable* vtable();
            static void move_initialize_from(
                Book*,
                Book*
            );
            [[nodiscard]] static bool deserialize(
                Book*,
                ::yazik::string_view
            );
            [[nodiscard]] static Ref as_ref(Book*);
            static Builder wrap(Book&);
        };
    }
    namespace get_book_request {
        struct RefPbSpec: protected ::yazik::compiler::support::Initializer {
            [[nodiscard]] static int64_t isbn(GetBookRequest*);
            [[nodiscard]] static const RefVtable* vtable();
            [[nodiscard]] static Builder as_builder(GetBookRequest*);
            [[nodiscard]] static std::string serialize(GetBookRequest*);
            [[nodiscard]] static Ref wrap(GetBookRequest&);
        };
        struct BuilderPbSpec: protected ::yazik::compiler::support::Initializer {
            static void set_isbn(
                GetBookRequest*,
                int64_t
            );
            static const BuilderVtable* vtable();
            static void move_initialize_from(
                GetBookRequest*,
                GetBookRequest*
            );
            [[nodiscard]] static bool deserialize(
                GetBookRequest*,
                ::yazik::string_view
            );
            [[nodiscard]] static Ref as_ref(GetBookRequest*);
            static Builder wrap(GetBookRequest&);
        };
    }
    namespace get_book_via_author {
        struct RefPbSpec: protected ::yazik::compiler::support::Initializer {
            [[nodiscard]] static ::yazik::string_view author(GetBookViaAuthor*);
            [[nodiscard]] static const RefVtable* vtable();
            [[nodiscard]] static Builder as_builder(GetBookViaAuthor*);
            [[nodiscard]] static std::string serialize(GetBookViaAuthor*);
            [[nodiscard]] static Ref wrap(GetBookViaAuthor&);
        };
        struct BuilderPbSpec: protected ::yazik::compiler::support::Initializer {
            static void set_author(
                GetBookViaAuthor*,
                std::string
            );
            static const BuilderVtable* vtable();
            static void move_initialize_from(
                GetBookViaAuthor*,
                GetBookViaAuthor*
            );
            [[nodiscard]] static bool deserialize(
                GetBookViaAuthor*,
                ::yazik::string_view
            );
            [[nodiscard]] static Ref as_ref(GetBookViaAuthor*);
            static Builder wrap(GetBookViaAuthor&);
        };
    }
    namespace book_store {
        struct RefPbSpec: protected ::yazik::compiler::support::Initializer {
            [[nodiscard]] static ::yazik::string_view name(BookStore*);
            [[nodiscard]] static const RefVtable* vtable();
            [[nodiscard]] static Builder as_builder(BookStore*);
            [[nodiscard]] static std::string serialize(BookStore*);
            [[nodiscard]] static Ref wrap(BookStore&);
        };
        struct BuilderPbSpec: protected ::yazik::compiler::support::Initializer {
            static void set_name(
                BookStore*,
                std::string
            );
            static const BuilderVtable* vtable();
            static void move_initialize_from(
                BookStore*,
                BookStore*
            );
            [[nodiscard]] static bool deserialize(
                BookStore*,
                ::yazik::string_view
            );
            [[nodiscard]] static Ref as_ref(BookStore*);
            static Builder wrap(BookStore&);
        };
    }
    namespace cycle_dep1 {
        struct RefPbSpec: protected ::yazik::compiler::support::Initializer {
            [[nodiscard]] static cycle_dep2::Ref dep(CycleDep1*);
            [[nodiscard]] static const RefVtable* vtable();
            [[nodiscard]] static Builder as_builder(CycleDep1*);
            [[nodiscard]] static std::string serialize(CycleDep1*);
            [[nodiscard]] static Ref wrap(CycleDep1&);
        };
        struct BuilderPbSpec: protected ::yazik::compiler::support::Initializer {
            [[nodiscard]] static cycle_dep2::Builder construct_dep_builder(CycleDep1*);
            static const BuilderVtable* vtable();
            static void move_initialize_from(
                CycleDep1*,
                CycleDep1*
            );
            [[nodiscard]] static bool deserialize(
                CycleDep1*,
                ::yazik::string_view
            );
            [[nodiscard]] static Ref as_ref(CycleDep1*);
            static Builder wrap(CycleDep1&);
        };
    }
    namespace cycle_dep2 {
        struct RefPbSpec: protected ::yazik::compiler::support::Initializer {
            [[nodiscard]] static cycle_dep1::Ref dep(CycleDep2*);
            [[nodiscard]] static const RefVtable* vtable();
            [[nodiscard]] static Builder as_builder(CycleDep2*);
            [[nodiscard]] static std::string serialize(CycleDep2*);
            [[nodiscard]] static Ref wrap(CycleDep2&);
        };
        struct BuilderPbSpec: protected ::yazik::compiler::support::Initializer {
            [[nodiscard]] static cycle_dep1::Builder construct_dep_builder(CycleDep2*);
            static const BuilderVtable* vtable();
            static void move_initialize_from(
                CycleDep2*,
                CycleDep2*
            );
            [[nodiscard]] static bool deserialize(
                CycleDep2*,
                ::yazik::string_view
            );
            [[nodiscard]] static Ref as_ref(CycleDep2*);
            static Builder wrap(CycleDep2&);
        };
    }
}
