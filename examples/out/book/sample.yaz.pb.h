#pragma once

#include <book/sample.yaz.h>
#include <book/sample.pb.h>

namespace com::book {
    struct AuthorPbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static std::string_view name(const Author*);

        static inline const AuthorVtable* vtable() {
            const static AuthorVtable s_vtable {
                .name = (AuthorVtable::name_fn)&AuthorPbSpec::name,
            };
            return &s_vtable;
        }

        static inline AuthorEntityRef wrap(const Author& a){
            return create_entity<AuthorEntityRef>(static_cast<const void*>(&a), vtable());
        }
    };

    struct AuthorPbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_name(Author*, std::string);
        [[nodiscard]] static AuthorEntityRef as_ref(const void*);

        static inline const AuthorBuilderVtable* vtable() {
            const static AuthorBuilderVtable s_vtable {
                .set_name = (AuthorBuilderVtable::set_name_fn)&AuthorPbBuilderSpec::set_name,
                .as_ref = (AuthorBuilderVtable::as_ref_fn)&AuthorPbBuilderSpec::as_ref,
            };
            return &s_vtable;
        }

        static inline AuthorBuilder wrap(Author& a){
            return create_builder<AuthorBuilder>(std::make_tuple(static_cast<void*>(&a), vtable()));
        }

        template <typename Parent>
        static inline AuthorLayeredBuilder<Parent> wrap_with_parent(
            Author& pb,
            Parent& parent
        ) {
            return create_builder_with_parent<AuthorLayeredBuilder<Parent>>(
                std::make_tuple(
                    &pb,
                    AuthorPbBuilderSpec::vtable()
                ),
                &parent
            );
        }
    };

    struct InsideOneOfMessagePbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static std::string_view field(const InsideOneOfMessage*);

        static inline const InsideOneOfMessageVtable* vtable() {
            const static InsideOneOfMessageVtable s_vtable {
                .field = (InsideOneOfMessageVtable::field_fn)&InsideOneOfMessagePbSpec::field,
            };
            return &s_vtable;
        }

        static inline InsideOneOfMessageEntityRef wrap(const InsideOneOfMessage& a){
            return create_entity<InsideOneOfMessageEntityRef>(static_cast<const void*>(&a), vtable());
        }
    };

    struct InsideOneOfMessagePbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_field(InsideOneOfMessage*, std::string);
        [[nodiscard]] static InsideOneOfMessageEntityRef as_ref(const void*);

        static inline const InsideOneOfMessageBuilderVtable* vtable() {
            const static InsideOneOfMessageBuilderVtable s_vtable {
                .set_field = (InsideOneOfMessageBuilderVtable::set_field_fn)&InsideOneOfMessagePbBuilderSpec::set_field,
                .as_ref = (InsideOneOfMessageBuilderVtable::as_ref_fn)&InsideOneOfMessagePbBuilderSpec::as_ref,
            };
            return &s_vtable;
        }

        static inline InsideOneOfMessageBuilder wrap(InsideOneOfMessage& a){
            return create_builder<InsideOneOfMessageBuilder>(std::make_tuple(static_cast<void*>(&a), vtable()));
        }

        template <typename Parent>
        static inline InsideOneOfMessageLayeredBuilder<Parent> wrap_with_parent(
            InsideOneOfMessage& pb,
            Parent& parent
        ) {
            return create_builder_with_parent<InsideOneOfMessageLayeredBuilder<Parent>>(
                std::make_tuple(
                    &pb,
                    InsideOneOfMessagePbBuilderSpec::vtable()
                ),
                &parent
            );
        }
    };

    struct BookPagePbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static ::ranges::any_view<std::string_view> lines(const Book_Page*);

        static inline const BookPageVtable* vtable() {
            const static BookPageVtable s_vtable {
                .lines = (BookPageVtable::lines_fn)&BookPagePbSpec::lines,
            };
            return &s_vtable;
        }

        static inline BookPageEntityRef wrap(const Book_Page& a){
            return create_entity<BookPageEntityRef>(static_cast<const void*>(&a), vtable());
        }
    };

    struct BookPagePbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void lines_pusher(Book_Page*, std::string);
        static BookPageBuilderVtable::lines_args_tuple_t lines_builder_args_factory(Book_Page *);
        [[nodiscard]] static BookPageEntityRef as_ref(const void*);

        static inline const BookPageBuilderVtable* vtable() {
            const static BookPageBuilderVtable s_vtable {
                .lines_builder_args_factory = (BookPageBuilderVtable::lines_builder_args_factory_fn)
                    &BookPagePbBuilderSpec::lines_builder_args_factory,
                .as_ref = (BookPageBuilderVtable::as_ref_fn)&BookPagePbBuilderSpec::as_ref,
            };
            return &s_vtable;
        }

        static inline BookPageBuilder wrap(Book_Page& a){
            return create_builder<BookPageBuilder>(std::make_tuple(static_cast<void*>(&a), vtable()));
        }

        template <typename Parent>
        static inline BookPageLayeredBuilder<Parent> wrap_with_parent(
            Book_Page& pb,
            Parent& parent
        ) {
            return create_builder_with_parent<BookPageLayeredBuilder<Parent>>(
                std::make_tuple(
                    &pb,
                    BookPagePbBuilderSpec::vtable()
                ),
                &parent
            );
        }
    };

    struct BookFirstOneofPbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static bool is_first_oneof_string(const Book*);
        [[nodiscard]] static std::string_view first_oneof_string(const Book*);
        [[nodiscard]] static bool is_first_oneof_int(const Book*);
        [[nodiscard]] static int64_t first_oneof_int(const Book*);
        [[nodiscard]] static bool is_null(const Book* a);

        static inline const BookFirstOneofVtable* vtable() {
            const static BookFirstOneofVtable s_vtable {
                .is_first_oneof_string = (BookFirstOneofVtable::is_first_oneof_string_fn)&BookFirstOneofPbSpec::is_first_oneof_string,
                .first_oneof_string = (BookFirstOneofVtable::first_oneof_string_fn)&BookFirstOneofPbSpec::first_oneof_string,
                .is_first_oneof_int = (BookFirstOneofVtable::is_first_oneof_int_fn)&BookFirstOneofPbSpec::is_first_oneof_int,
                .first_oneof_int = (BookFirstOneofVtable::first_oneof_int_fn)&BookFirstOneofPbSpec::first_oneof_int,
                .is_null = (BookFirstOneofVtable::is_null_fn)&BookFirstOneofPbSpec::is_null,
            };
            return &s_vtable;
        }

        static inline BookFirstOneofVariantRef wrap(const Book& a) {
            return create_entity<BookFirstOneofVariantRef>(static_cast<const void*>(&a), vtable());
        }
    };

    struct BookFirstOneofPbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_first_oneof_string(Book*, std::string);
        static void set_first_oneof_int(Book*, int64_t);

        static inline const BookFirstOneofBuilderVtable* vtable() {
            const static BookFirstOneofBuilderVtable s_vtable {
                .set_first_oneof_string = (BookFirstOneofBuilderVtable::set_first_oneof_string_fn)&BookFirstOneofPbBuilderSpec::set_first_oneof_string,
                .set_first_oneof_int = (BookFirstOneofBuilderVtable::set_first_oneof_int_fn)&BookFirstOneofPbBuilderSpec::set_first_oneof_int,
            };
            return &s_vtable;
        }

        static inline BookFirstOneofVariantBuilder wrap(Book& a) {
            return create_builder<BookFirstOneofVariantBuilder>(std::make_tuple(static_cast<void*>(&a), vtable()));
        }
    };

    struct BookSecondOneofPbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static bool is_second_oneof_string(const Book*);
        [[nodiscard]] static std::string_view second_oneof_string(const Book*);
        [[nodiscard]] static bool is_second_oneof_int(const Book*);
        [[nodiscard]] static int64_t second_oneof_int(const Book*);
        [[nodiscard]] static bool is_second_oneof_message(const Book*);
        [[nodiscard]] static InsideOneOfMessageEntityRef second_oneof_message(const Book*);
        [[nodiscard]] static bool is_null(const Book* a);

        static inline const BookSecondOneofVtable* vtable() {
            const static BookSecondOneofVtable s_vtable {
                .is_second_oneof_string = (BookSecondOneofVtable::is_second_oneof_string_fn)&BookSecondOneofPbSpec::is_second_oneof_string,
                .second_oneof_string = (BookSecondOneofVtable::second_oneof_string_fn)&BookSecondOneofPbSpec::second_oneof_string,
                .is_second_oneof_int = (BookSecondOneofVtable::is_second_oneof_int_fn)&BookSecondOneofPbSpec::is_second_oneof_int,
                .second_oneof_int = (BookSecondOneofVtable::second_oneof_int_fn)&BookSecondOneofPbSpec::second_oneof_int,
                .is_second_oneof_message = (BookSecondOneofVtable::is_second_oneof_message_fn)&BookSecondOneofPbSpec::is_second_oneof_message,
                .second_oneof_message = (BookSecondOneofVtable::second_oneof_message_fn)&BookSecondOneofPbSpec::second_oneof_message,
                .is_null = (BookSecondOneofVtable::is_null_fn)&BookSecondOneofPbSpec::is_null,
            };
            return &s_vtable;
        }

        static inline BookSecondOneofVariantRef wrap(const Book& a) {
            return create_entity<BookSecondOneofVariantRef>(static_cast<const void*>(&a), vtable());
        }
    };

    struct BookSecondOneofPbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_second_oneof_string(Book*, std::string);
        static void set_second_oneof_int(Book*, int64_t);
        static void set_second_oneof_message(Book*, ::folly::Function<void(InsideOneOfMessageBuilder&)>);

        static inline const BookSecondOneofBuilderVtable* vtable() {
            const static BookSecondOneofBuilderVtable s_vtable {
                .set_second_oneof_string = (BookSecondOneofBuilderVtable::set_second_oneof_string_fn)&BookSecondOneofPbBuilderSpec::set_second_oneof_string,
                .set_second_oneof_int = (BookSecondOneofBuilderVtable::set_second_oneof_int_fn)&BookSecondOneofPbBuilderSpec::set_second_oneof_int,
                .set_second_oneof_message = (BookSecondOneofBuilderVtable::set_second_oneof_message_fn)&BookSecondOneofPbBuilderSpec::set_second_oneof_message,
            };
            return &s_vtable;
        }

        static inline BookSecondOneofVariantBuilder wrap(Book& a) {
            return create_builder<BookSecondOneofVariantBuilder>(std::make_tuple(static_cast<void*>(&a), vtable()));
        }
    };

    struct BookPbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static int64_t isbn(const Book*);
        [[nodiscard]] static std::string_view title(const Book*);
        [[nodiscard]] static AuthorEntityRef author(const Book*);
        [[nodiscard]] static ::ranges::any_view<BookPageEntityRef> pages(const Book*);
        [[nodiscard]] static BookFirstOneofVariantRef first_oneof(const Book*);
        [[nodiscard]] static BookAvailabilityEnum availability(const Book*);
        [[nodiscard]] static CanRecommendEnum recommended(const Book*);
        [[nodiscard]] static BookSecondOneofVariantRef second_oneof(const Book*);

        static inline const BookVtable* vtable() {
            const static BookVtable s_vtable {
                .isbn = (BookVtable::isbn_fn)&BookPbSpec::isbn,
                .title = (BookVtable::title_fn)&BookPbSpec::title,
                .author = (BookVtable::author_fn)&BookPbSpec::author,
                .pages = (BookVtable::pages_fn)&BookPbSpec::pages,
                .first_oneof = (BookVtable::first_oneof_fn)&BookPbSpec::first_oneof,
                .availability = (BookVtable::availability_fn)&BookPbSpec::availability,
                .recommended = (BookVtable::recommended_fn)&BookPbSpec::recommended,
                .second_oneof = (BookVtable::second_oneof_fn)&BookPbSpec::second_oneof,
            };
            return &s_vtable;
        }

        static inline BookEntityRef wrap(const Book& a){
            return create_entity<BookEntityRef>(static_cast<const void*>(&a), vtable());
        }
    };

    struct BookPbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_isbn(Book*, int64_t);
        static void set_title(Book*, std::string);
        static BookBuilderVtable::author_args_tuple_t author_builder_args_factory(Book *);
        static Book_Page* pages_factory(Book*);
        static BookBuilderVtable::pages_args_tuple_t pages_builder_args_factory(Book *);
        static BookBuilderVtable::first_oneof_args_tuple_t first_oneof_builder_args_factory(Book *);
        static void set_availability(Book*, BookAvailabilityEnum::Value);
        static void set_recommended(Book*, CanRecommendEnum::Value);
        static BookBuilderVtable::second_oneof_args_tuple_t second_oneof_builder_args_factory(Book *);
        [[nodiscard]] static BookEntityRef as_ref(const void*);

        static inline const BookBuilderVtable* vtable() {
            const static BookBuilderVtable s_vtable {
                .set_isbn = (BookBuilderVtable::set_isbn_fn)&BookPbBuilderSpec::set_isbn,
                .set_title = (BookBuilderVtable::set_title_fn)&BookPbBuilderSpec::set_title,
                .author_builder_args_factory = (BookBuilderVtable::author_builder_args_factory_fn)
                    &BookPbBuilderSpec::author_builder_args_factory,
                .pages_builder_args_factory = (BookBuilderVtable::pages_builder_args_factory_fn)
                    &BookPbBuilderSpec::pages_builder_args_factory,
                .first_oneof_builder_args_factory = (BookBuilderVtable::first_oneof_builder_args_factory_fn)
                    &BookPbBuilderSpec::first_oneof_builder_args_factory,
                .set_availability = (BookBuilderVtable::set_availability_fn)&BookPbBuilderSpec::set_availability,
                .set_recommended = (BookBuilderVtable::set_recommended_fn)&BookPbBuilderSpec::set_recommended,
                .second_oneof_builder_args_factory = (BookBuilderVtable::second_oneof_builder_args_factory_fn)
                    &BookPbBuilderSpec::second_oneof_builder_args_factory,
                .as_ref = (BookBuilderVtable::as_ref_fn)&BookPbBuilderSpec::as_ref,
            };
            return &s_vtable;
        }

        static inline BookBuilder wrap(Book& a){
            return create_builder<BookBuilder>(std::make_tuple(static_cast<void*>(&a), vtable()));
        }

        template <typename Parent>
        static inline BookLayeredBuilder<Parent> wrap_with_parent(
            Book& pb,
            Parent& parent
        ) {
            return create_builder_with_parent<BookLayeredBuilder<Parent>>(
                std::make_tuple(
                    &pb,
                    BookPbBuilderSpec::vtable()
                ),
                &parent
            );
        }
    };

    struct GetBookRequestPbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static int64_t isbn(const GetBookRequest*);

        static inline const GetBookRequestVtable* vtable() {
            const static GetBookRequestVtable s_vtable {
                .isbn = (GetBookRequestVtable::isbn_fn)&GetBookRequestPbSpec::isbn,
            };
            return &s_vtable;
        }

        static inline GetBookRequestEntityRef wrap(const GetBookRequest& a){
            return create_entity<GetBookRequestEntityRef>(static_cast<const void*>(&a), vtable());
        }
    };

    struct GetBookRequestPbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_isbn(GetBookRequest*, int64_t);
        [[nodiscard]] static GetBookRequestEntityRef as_ref(const void*);

        static inline const GetBookRequestBuilderVtable* vtable() {
            const static GetBookRequestBuilderVtable s_vtable {
                .set_isbn = (GetBookRequestBuilderVtable::set_isbn_fn)&GetBookRequestPbBuilderSpec::set_isbn,
                .as_ref = (GetBookRequestBuilderVtable::as_ref_fn)&GetBookRequestPbBuilderSpec::as_ref,
            };
            return &s_vtable;
        }

        static inline GetBookRequestBuilder wrap(GetBookRequest& a){
            return create_builder<GetBookRequestBuilder>(std::make_tuple(static_cast<void*>(&a), vtable()));
        }

        template <typename Parent>
        static inline GetBookRequestLayeredBuilder<Parent> wrap_with_parent(
            GetBookRequest& pb,
            Parent& parent
        ) {
            return create_builder_with_parent<GetBookRequestLayeredBuilder<Parent>>(
                std::make_tuple(
                    &pb,
                    GetBookRequestPbBuilderSpec::vtable()
                ),
                &parent
            );
        }
    };

    struct GetBookViaAuthorPbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static std::string_view author(const GetBookViaAuthor*);

        static inline const GetBookViaAuthorVtable* vtable() {
            const static GetBookViaAuthorVtable s_vtable {
                .author = (GetBookViaAuthorVtable::author_fn)&GetBookViaAuthorPbSpec::author,
            };
            return &s_vtable;
        }

        static inline GetBookViaAuthorEntityRef wrap(const GetBookViaAuthor& a){
            return create_entity<GetBookViaAuthorEntityRef>(static_cast<const void*>(&a), vtable());
        }
    };

    struct GetBookViaAuthorPbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_author(GetBookViaAuthor*, std::string);
        [[nodiscard]] static GetBookViaAuthorEntityRef as_ref(const void*);

        static inline const GetBookViaAuthorBuilderVtable* vtable() {
            const static GetBookViaAuthorBuilderVtable s_vtable {
                .set_author = (GetBookViaAuthorBuilderVtable::set_author_fn)&GetBookViaAuthorPbBuilderSpec::set_author,
                .as_ref = (GetBookViaAuthorBuilderVtable::as_ref_fn)&GetBookViaAuthorPbBuilderSpec::as_ref,
            };
            return &s_vtable;
        }

        static inline GetBookViaAuthorBuilder wrap(GetBookViaAuthor& a){
            return create_builder<GetBookViaAuthorBuilder>(std::make_tuple(static_cast<void*>(&a), vtable()));
        }

        template <typename Parent>
        static inline GetBookViaAuthorLayeredBuilder<Parent> wrap_with_parent(
            GetBookViaAuthor& pb,
            Parent& parent
        ) {
            return create_builder_with_parent<GetBookViaAuthorLayeredBuilder<Parent>>(
                std::make_tuple(
                    &pb,
                    GetBookViaAuthorPbBuilderSpec::vtable()
                ),
                &parent
            );
        }
    };

    struct BookStorePbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static std::string_view name(const BookStore*);

        static inline const BookStoreVtable* vtable() {
            const static BookStoreVtable s_vtable {
                .name = (BookStoreVtable::name_fn)&BookStorePbSpec::name,
            };
            return &s_vtable;
        }

        static inline BookStoreEntityRef wrap(const BookStore& a){
            return create_entity<BookStoreEntityRef>(static_cast<const void*>(&a), vtable());
        }
    };

    struct BookStorePbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_name(BookStore*, std::string);
        [[nodiscard]] static BookStoreEntityRef as_ref(const void*);

        static inline const BookStoreBuilderVtable* vtable() {
            const static BookStoreBuilderVtable s_vtable {
                .set_name = (BookStoreBuilderVtable::set_name_fn)&BookStorePbBuilderSpec::set_name,
                .as_ref = (BookStoreBuilderVtable::as_ref_fn)&BookStorePbBuilderSpec::as_ref,
            };
            return &s_vtable;
        }

        static inline BookStoreBuilder wrap(BookStore& a){
            return create_builder<BookStoreBuilder>(std::make_tuple(static_cast<void*>(&a), vtable()));
        }

        template <typename Parent>
        static inline BookStoreLayeredBuilder<Parent> wrap_with_parent(
            BookStore& pb,
            Parent& parent
        ) {
            return create_builder_with_parent<BookStoreLayeredBuilder<Parent>>(
                std::make_tuple(
                    &pb,
                    BookStorePbBuilderSpec::vtable()
                ),
                &parent
            );
        }
    };

    struct CycleDep1PbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static CycleDep2EntityRef dep(const CycleDep1*);

        static inline const CycleDep1Vtable* vtable() {
            const static CycleDep1Vtable s_vtable {
                .dep = (CycleDep1Vtable::dep_fn)&CycleDep1PbSpec::dep,
            };
            return &s_vtable;
        }

        static inline CycleDep1EntityRef wrap(const CycleDep1& a){
            return create_entity<CycleDep1EntityRef>(static_cast<const void*>(&a), vtable());
        }
    };

    struct CycleDep1PbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static CycleDep1BuilderVtable::dep_args_tuple_t dep_builder_args_factory(CycleDep1 *);
        [[nodiscard]] static CycleDep1EntityRef as_ref(const void*);

        static inline const CycleDep1BuilderVtable* vtable() {
            const static CycleDep1BuilderVtable s_vtable {
                .dep_builder_args_factory = (CycleDep1BuilderVtable::dep_builder_args_factory_fn)
                    &CycleDep1PbBuilderSpec::dep_builder_args_factory,
                .as_ref = (CycleDep1BuilderVtable::as_ref_fn)&CycleDep1PbBuilderSpec::as_ref,
            };
            return &s_vtable;
        }

        static inline CycleDep1Builder wrap(CycleDep1& a){
            return create_builder<CycleDep1Builder>(std::make_tuple(static_cast<void*>(&a), vtable()));
        }

        template <typename Parent>
        static inline CycleDep1LayeredBuilder<Parent> wrap_with_parent(
            CycleDep1& pb,
            Parent& parent
        ) {
            return create_builder_with_parent<CycleDep1LayeredBuilder<Parent>>(
                std::make_tuple(
                    &pb,
                    CycleDep1PbBuilderSpec::vtable()
                ),
                &parent
            );
        }
    };

    struct CycleDep2PbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static CycleDep1EntityRef dep(const CycleDep2*);

        static inline const CycleDep2Vtable* vtable() {
            const static CycleDep2Vtable s_vtable {
                .dep = (CycleDep2Vtable::dep_fn)&CycleDep2PbSpec::dep,
            };
            return &s_vtable;
        }

        static inline CycleDep2EntityRef wrap(const CycleDep2& a){
            return create_entity<CycleDep2EntityRef>(static_cast<const void*>(&a), vtable());
        }
    };

    struct CycleDep2PbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static CycleDep2BuilderVtable::dep_args_tuple_t dep_builder_args_factory(CycleDep2 *);
        [[nodiscard]] static CycleDep2EntityRef as_ref(const void*);

        static inline const CycleDep2BuilderVtable* vtable() {
            const static CycleDep2BuilderVtable s_vtable {
                .dep_builder_args_factory = (CycleDep2BuilderVtable::dep_builder_args_factory_fn)
                    &CycleDep2PbBuilderSpec::dep_builder_args_factory,
                .as_ref = (CycleDep2BuilderVtable::as_ref_fn)&CycleDep2PbBuilderSpec::as_ref,
            };
            return &s_vtable;
        }

        static inline CycleDep2Builder wrap(CycleDep2& a){
            return create_builder<CycleDep2Builder>(std::make_tuple(static_cast<void*>(&a), vtable()));
        }

        template <typename Parent>
        static inline CycleDep2LayeredBuilder<Parent> wrap_with_parent(
            CycleDep2& pb,
            Parent& parent
        ) {
            return create_builder_with_parent<CycleDep2LayeredBuilder<Parent>>(
                std::make_tuple(
                    &pb,
                    CycleDep2PbBuilderSpec::vtable()
                ),
                &parent
            );
        }
    };

    inline std::string_view AuthorPbSpec::name(const Author* a) {
        return a->name();
    }

    inline void AuthorPbBuilderSpec::set_name(Author* a, std::string value) {
        a->set_name(std::move(value));
    }
    inline AuthorEntityRef AuthorPbBuilderSpec::as_ref(const void* ptr) {
        return create_entity<AuthorEntityRef>(ptr, AuthorPbSpec::vtable());
    }

    inline std::string_view InsideOneOfMessagePbSpec::field(const InsideOneOfMessage* a) {
        return a->field();
    }

    inline void InsideOneOfMessagePbBuilderSpec::set_field(InsideOneOfMessage* a, std::string value) {
        a->set_field(std::move(value));
    }
    inline InsideOneOfMessageEntityRef InsideOneOfMessagePbBuilderSpec::as_ref(const void* ptr) {
        return create_entity<InsideOneOfMessageEntityRef>(ptr, InsideOneOfMessagePbSpec::vtable());
    }

    inline ::ranges::any_view<std::string_view> BookPagePbSpec::lines(const Book_Page* a) {
        return ::ranges::views::all(a->lines())
            | ::ranges::views::transform([](const auto& x) {
                return std::string_view { x };
            });
    }

    inline void BookPagePbBuilderSpec::lines_pusher(Book_Page* a, std::string value) {
        a->mutable_lines()->Add(std::move(value));
    }
    inline BookPageBuilderVtable::lines_args_tuple_t BookPagePbBuilderSpec::lines_builder_args_factory(Book_Page * a) {
        return { a, (::yazik::compiler::support::VecBuilder<std::string>::push_clbk_t)&BookPagePbBuilderSpec::lines_pusher };
    }
    inline BookPageEntityRef BookPagePbBuilderSpec::as_ref(const void* ptr) {
        return create_entity<BookPageEntityRef>(ptr, BookPagePbSpec::vtable());
    }

    inline bool BookFirstOneofPbSpec::is_first_oneof_string(const Book* a) {
        return a->first_oneof_case() == Book::kFirstOneofString;
    }
    inline std::string_view BookFirstOneofPbSpec::first_oneof_string(const Book* a) {
        return a->first_oneof_string();
    }
    inline bool BookFirstOneofPbSpec::is_first_oneof_int(const Book* a) {
        return a->first_oneof_case() == Book::kFirstOneofInt;
    }
    inline int64_t BookFirstOneofPbSpec::first_oneof_int(const Book* a) {
        return a->first_oneof_int();
    }
    inline bool BookFirstOneofPbSpec::is_null(const Book* a) {
        return a->first_oneof_case() == Book::FIRST_ONEOF_NOT_SET;
    }

    inline void BookFirstOneofPbBuilderSpec::set_first_oneof_string(Book* a, std::string value) {
        a->set_first_oneof_string(std::move(value));
    }
    inline void BookFirstOneofPbBuilderSpec::set_first_oneof_int(Book* a, int64_t value) {
        a->set_first_oneof_int(std::move(value));
    }

    inline bool BookSecondOneofPbSpec::is_second_oneof_string(const Book* a) {
        return a->second_oneof_case() == Book::kSecondOneofString;
    }
    inline std::string_view BookSecondOneofPbSpec::second_oneof_string(const Book* a) {
        return a->second_oneof_string();
    }
    inline bool BookSecondOneofPbSpec::is_second_oneof_int(const Book* a) {
        return a->second_oneof_case() == Book::kSecondOneofInt;
    }
    inline int64_t BookSecondOneofPbSpec::second_oneof_int(const Book* a) {
        return a->second_oneof_int();
    }
    inline bool BookSecondOneofPbSpec::is_second_oneof_message(const Book* a) {
        return a->second_oneof_case() == Book::kSecondOneofMessage;
    }
    inline InsideOneOfMessageEntityRef BookSecondOneofPbSpec::second_oneof_message(const Book* a) {
        return InsideOneOfMessagePbSpec::wrap(a->second_oneof_message());
    }
    inline bool BookSecondOneofPbSpec::is_null(const Book* a) {
        return a->second_oneof_case() == Book::SECOND_ONEOF_NOT_SET;
    }

    inline void BookSecondOneofPbBuilderSpec::set_second_oneof_string(Book* a, std::string value) {
        a->set_second_oneof_string(std::move(value));
    }
    inline void BookSecondOneofPbBuilderSpec::set_second_oneof_int(Book* a, int64_t value) {
        a->set_second_oneof_int(std::move(value));
    }
    inline void BookSecondOneofPbBuilderSpec::set_second_oneof_message(Book* a, ::folly::Function<void(InsideOneOfMessageBuilder&)> clbk) {
        auto builder = InsideOneOfMessagePbBuilderSpec::wrap(* a->mutable_second_oneof_message());
        clbk(builder);
    }

    inline int64_t BookPbSpec::isbn(const Book* a) {
        return a->isbn();
    }
    inline std::string_view BookPbSpec::title(const Book* a) {
        return a->title();
    }
    inline AuthorEntityRef BookPbSpec::author(const Book* a) {
        return AuthorPbSpec::wrap(a->author());
    }
    inline ::ranges::any_view<BookPageEntityRef> BookPbSpec::pages(const Book* a) {
        return ::ranges::views::all(a->pages())
            | ::ranges::views::transform([](const Book_Page& x) {
                return BookPagePbSpec::wrap(x);
            });
    }
    inline BookFirstOneofVariantRef BookPbSpec::first_oneof(const Book* a) {
        return BookFirstOneofPbSpec::wrap(*a);
    }
    inline BookAvailabilityEnum BookPbSpec::availability(const Book* a) {
        return create_enum<BookAvailabilityEnum>((int)a->availability());
    }
    inline CanRecommendEnum BookPbSpec::recommended(const Book* a) {
        return create_enum<CanRecommendEnum>((int)a->recommended());
    }
    inline BookSecondOneofVariantRef BookPbSpec::second_oneof(const Book* a) {
        return BookSecondOneofPbSpec::wrap(*a);
    }

    inline void BookPbBuilderSpec::set_isbn(Book* a, int64_t value) {
        a->set_isbn(std::move(value));
    }
    inline void BookPbBuilderSpec::set_title(Book* a, std::string value) {
        a->set_title(std::move(value));
    }
    inline BookBuilderVtable::author_args_tuple_t BookPbBuilderSpec::author_builder_args_factory(Book * a) {
        return { a->mutable_author(), AuthorPbBuilderSpec::vtable() };
    }
    inline Book_Page* BookPbBuilderSpec::pages_factory(Book* a) {
        return a->mutable_pages()->Add();
    }
    inline BookBuilderVtable::pages_args_tuple_t BookPbBuilderSpec::pages_builder_args_factory(Book * a) {
        return { a, (::yazik::compiler::support::VecEntityBuilder<BookPageBuilder, BookPageBuilderVtable>::builder_factory_t)&BookPbBuilderSpec::pages_factory, BookPagePbBuilderSpec::vtable() };
    }
    inline BookBuilderVtable::first_oneof_args_tuple_t BookPbBuilderSpec::first_oneof_builder_args_factory(Book * a) {
        return { a, BookFirstOneofPbBuilderSpec::vtable() };
    }
    inline void BookPbBuilderSpec::set_availability(Book* a, BookAvailabilityEnum::Value value) {
        a->set_availability((Book_Availability)value);
    }
    inline void BookPbBuilderSpec::set_recommended(Book* a, CanRecommendEnum::Value value) {
        a->set_recommended((CanRecommend)value);
    }
    inline BookBuilderVtable::second_oneof_args_tuple_t BookPbBuilderSpec::second_oneof_builder_args_factory(Book * a) {
        return { a, BookSecondOneofPbBuilderSpec::vtable() };
    }
    inline BookEntityRef BookPbBuilderSpec::as_ref(const void* ptr) {
        return create_entity<BookEntityRef>(ptr, BookPbSpec::vtable());
    }

    inline int64_t GetBookRequestPbSpec::isbn(const GetBookRequest* a) {
        return a->isbn();
    }

    inline void GetBookRequestPbBuilderSpec::set_isbn(GetBookRequest* a, int64_t value) {
        a->set_isbn(std::move(value));
    }
    inline GetBookRequestEntityRef GetBookRequestPbBuilderSpec::as_ref(const void* ptr) {
        return create_entity<GetBookRequestEntityRef>(ptr, GetBookRequestPbSpec::vtable());
    }

    inline std::string_view GetBookViaAuthorPbSpec::author(const GetBookViaAuthor* a) {
        return a->author();
    }

    inline void GetBookViaAuthorPbBuilderSpec::set_author(GetBookViaAuthor* a, std::string value) {
        a->set_author(std::move(value));
    }
    inline GetBookViaAuthorEntityRef GetBookViaAuthorPbBuilderSpec::as_ref(const void* ptr) {
        return create_entity<GetBookViaAuthorEntityRef>(ptr, GetBookViaAuthorPbSpec::vtable());
    }

    inline std::string_view BookStorePbSpec::name(const BookStore* a) {
        return a->name();
    }

    inline void BookStorePbBuilderSpec::set_name(BookStore* a, std::string value) {
        a->set_name(std::move(value));
    }
    inline BookStoreEntityRef BookStorePbBuilderSpec::as_ref(const void* ptr) {
        return create_entity<BookStoreEntityRef>(ptr, BookStorePbSpec::vtable());
    }

    inline CycleDep2EntityRef CycleDep1PbSpec::dep(const CycleDep1* a) {
        return CycleDep2PbSpec::wrap(a->dep());
    }

    inline CycleDep1BuilderVtable::dep_args_tuple_t CycleDep1PbBuilderSpec::dep_builder_args_factory(CycleDep1 * a) {
        return { a->mutable_dep(), CycleDep2PbBuilderSpec::vtable() };
    }
    inline CycleDep1EntityRef CycleDep1PbBuilderSpec::as_ref(const void* ptr) {
        return create_entity<CycleDep1EntityRef>(ptr, CycleDep1PbSpec::vtable());
    }

    inline CycleDep1EntityRef CycleDep2PbSpec::dep(const CycleDep2* a) {
        return CycleDep1PbSpec::wrap(a->dep());
    }

    inline CycleDep2BuilderVtable::dep_args_tuple_t CycleDep2PbBuilderSpec::dep_builder_args_factory(CycleDep2 * a) {
        return { a->mutable_dep(), CycleDep1PbBuilderSpec::vtable() };
    }
    inline CycleDep2EntityRef CycleDep2PbBuilderSpec::as_ref(const void* ptr) {
        return create_entity<CycleDep2EntityRef>(ptr, CycleDep2PbSpec::vtable());
    }

}
