#pragma once

#include <book/sample.yaz.h>
#include <book/sample.pb.h>

namespace com::book {
    struct AuthorPbSpec: protected ::yazik::compiler::support::Initializer {
        [[nodiscard]] static std::string_view name(Author*);
        static AuthorBuilder as_builder(Author*);
        static std::string serialize(Author*);

        static inline const AuthorVtable* vtable() {
            const static AuthorVtable s_vtable {
                .name = (AuthorVtable::name_fn)&AuthorPbSpec::name,
                .as_builder = (AuthorVtable::as_builder_fn)&AuthorPbSpec::as_builder,
                .serialize = (AuthorVtable::serialize_fn)&AuthorPbSpec::serialize,
            };
            return &s_vtable;
        }

        static inline AuthorEntityRef wrap(Author& a){
            return create_entity<AuthorEntityRef>(static_cast<void*>(&a), vtable());
        }
    };

    struct AuthorPbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_name(Author*, std::string);
        static void move_initialize_from(Author*, Author*);
        [[nodiscard]] static bool deserialize(Author*, std::string_view);
        [[nodiscard]] static AuthorEntityRef as_ref(void*);

        static inline const AuthorBuilderVtable* vtable() {
            const static AuthorBuilderVtable s_vtable {
                .set_name = (AuthorBuilderVtable::set_name_fn)&AuthorPbBuilderSpec::set_name,
                .move_initialize_from = (AuthorBuilderVtable::move_initialize_from_fn)&AuthorPbBuilderSpec::move_initialize_from,
                .deserialize = (AuthorBuilderVtable::deserialize_fn)&AuthorPbBuilderSpec::deserialize,
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
        [[nodiscard]] static std::string_view field(InsideOneOfMessage*);
        static InsideOneOfMessageBuilder as_builder(InsideOneOfMessage*);
        static std::string serialize(InsideOneOfMessage*);

        static inline const InsideOneOfMessageVtable* vtable() {
            const static InsideOneOfMessageVtable s_vtable {
                .field = (InsideOneOfMessageVtable::field_fn)&InsideOneOfMessagePbSpec::field,
                .as_builder = (InsideOneOfMessageVtable::as_builder_fn)&InsideOneOfMessagePbSpec::as_builder,
                .serialize = (InsideOneOfMessageVtable::serialize_fn)&InsideOneOfMessagePbSpec::serialize,
            };
            return &s_vtable;
        }

        static inline InsideOneOfMessageEntityRef wrap(InsideOneOfMessage& a){
            return create_entity<InsideOneOfMessageEntityRef>(static_cast<void*>(&a), vtable());
        }
    };

    struct InsideOneOfMessagePbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_field(InsideOneOfMessage*, std::string);
        static void move_initialize_from(InsideOneOfMessage*, InsideOneOfMessage*);
        [[nodiscard]] static bool deserialize(InsideOneOfMessage*, std::string_view);
        [[nodiscard]] static InsideOneOfMessageEntityRef as_ref(void*);

        static inline const InsideOneOfMessageBuilderVtable* vtable() {
            const static InsideOneOfMessageBuilderVtable s_vtable {
                .set_field = (InsideOneOfMessageBuilderVtable::set_field_fn)&InsideOneOfMessagePbBuilderSpec::set_field,
                .move_initialize_from = (InsideOneOfMessageBuilderVtable::move_initialize_from_fn)&InsideOneOfMessagePbBuilderSpec::move_initialize_from,
                .deserialize = (InsideOneOfMessageBuilderVtable::deserialize_fn)&InsideOneOfMessagePbBuilderSpec::deserialize,
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
        [[nodiscard]] static ::ranges::any_view<std::string_view> lines(Book_Page*);
        static BookPageBuilder as_builder(Book_Page*);
        static std::string serialize(Book_Page*);

        static inline const BookPageVtable* vtable() {
            const static BookPageVtable s_vtable {
                .lines = (BookPageVtable::lines_fn)&BookPagePbSpec::lines,
                .as_builder = (BookPageVtable::as_builder_fn)&BookPagePbSpec::as_builder,
                .serialize = (BookPageVtable::serialize_fn)&BookPagePbSpec::serialize,
            };
            return &s_vtable;
        }

        static inline BookPageEntityRef wrap(Book_Page& a){
            return create_entity<BookPageEntityRef>(static_cast<void*>(&a), vtable());
        }
    };

    struct BookPagePbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void lines_pusher(Book_Page*, std::string);
        static BookPageBuilderVtable::lines_args_tuple_t lines_builder_args_factory(Book_Page *);
        static void move_initialize_from(Book_Page*, Book_Page*);
        [[nodiscard]] static bool deserialize(Book_Page*, std::string_view);
        [[nodiscard]] static BookPageEntityRef as_ref(void*);

        static inline const BookPageBuilderVtable* vtable() {
            const static BookPageBuilderVtable s_vtable {
                .lines_builder_args_factory = (BookPageBuilderVtable::lines_builder_args_factory_fn)
                    &BookPagePbBuilderSpec::lines_builder_args_factory,
                .move_initialize_from = (BookPageBuilderVtable::move_initialize_from_fn)&BookPagePbBuilderSpec::move_initialize_from,
                .deserialize = (BookPageBuilderVtable::deserialize_fn)&BookPagePbBuilderSpec::deserialize,
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
        [[nodiscard]] static std::string_view first_oneof_string(Book*);
        [[nodiscard]] static bool is_first_oneof_int(const Book*);
        [[nodiscard]] static int64_t first_oneof_int(Book*);
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

        static inline BookFirstOneofVariantRef wrap(Book& a) {
            return create_entity<BookFirstOneofVariantRef>(static_cast<void*>(&a), vtable());
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
        [[nodiscard]] static std::string_view second_oneof_string(Book*);
        [[nodiscard]] static bool is_second_oneof_int(const Book*);
        [[nodiscard]] static int64_t second_oneof_int(Book*);
        [[nodiscard]] static bool is_second_oneof_message(const Book*);
        [[nodiscard]] static InsideOneOfMessageEntityRef second_oneof_message(Book*);
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

        static inline BookSecondOneofVariantRef wrap(Book& a) {
            return create_entity<BookSecondOneofVariantRef>(static_cast<void*>(&a), vtable());
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
        [[nodiscard]] static int64_t isbn(Book*);
        [[nodiscard]] static std::string_view title(Book*);
        [[nodiscard]] static AuthorEntityRef author(Book*);
        [[nodiscard]] static ::ranges::any_view<BookPageEntityRef> pages(Book*);
        [[nodiscard]] static BookFirstOneofVariantRef first_oneof(Book*);
        [[nodiscard]] static BookAvailabilityEnum availability(Book*);
        [[nodiscard]] static CanRecommendEnum recommended(Book*);
        [[nodiscard]] static BookSecondOneofVariantRef second_oneof(Book*);
        static BookBuilder as_builder(Book*);
        static std::string serialize(Book*);

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
                .as_builder = (BookVtable::as_builder_fn)&BookPbSpec::as_builder,
                .serialize = (BookVtable::serialize_fn)&BookPbSpec::serialize,
            };
            return &s_vtable;
        }

        static inline BookEntityRef wrap(Book& a){
            return create_entity<BookEntityRef>(static_cast<void*>(&a), vtable());
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
        static void move_initialize_from(Book*, Book*);
        [[nodiscard]] static bool deserialize(Book*, std::string_view);
        [[nodiscard]] static BookEntityRef as_ref(void*);

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
                .move_initialize_from = (BookBuilderVtable::move_initialize_from_fn)&BookPbBuilderSpec::move_initialize_from,
                .deserialize = (BookBuilderVtable::deserialize_fn)&BookPbBuilderSpec::deserialize,
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
        [[nodiscard]] static int64_t isbn(GetBookRequest*);
        static GetBookRequestBuilder as_builder(GetBookRequest*);
        static std::string serialize(GetBookRequest*);

        static inline const GetBookRequestVtable* vtable() {
            const static GetBookRequestVtable s_vtable {
                .isbn = (GetBookRequestVtable::isbn_fn)&GetBookRequestPbSpec::isbn,
                .as_builder = (GetBookRequestVtable::as_builder_fn)&GetBookRequestPbSpec::as_builder,
                .serialize = (GetBookRequestVtable::serialize_fn)&GetBookRequestPbSpec::serialize,
            };
            return &s_vtable;
        }

        static inline GetBookRequestEntityRef wrap(GetBookRequest& a){
            return create_entity<GetBookRequestEntityRef>(static_cast<void*>(&a), vtable());
        }
    };

    struct GetBookRequestPbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_isbn(GetBookRequest*, int64_t);
        static void move_initialize_from(GetBookRequest*, GetBookRequest*);
        [[nodiscard]] static bool deserialize(GetBookRequest*, std::string_view);
        [[nodiscard]] static GetBookRequestEntityRef as_ref(void*);

        static inline const GetBookRequestBuilderVtable* vtable() {
            const static GetBookRequestBuilderVtable s_vtable {
                .set_isbn = (GetBookRequestBuilderVtable::set_isbn_fn)&GetBookRequestPbBuilderSpec::set_isbn,
                .move_initialize_from = (GetBookRequestBuilderVtable::move_initialize_from_fn)&GetBookRequestPbBuilderSpec::move_initialize_from,
                .deserialize = (GetBookRequestBuilderVtable::deserialize_fn)&GetBookRequestPbBuilderSpec::deserialize,
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
        [[nodiscard]] static std::string_view author(GetBookViaAuthor*);
        static GetBookViaAuthorBuilder as_builder(GetBookViaAuthor*);
        static std::string serialize(GetBookViaAuthor*);

        static inline const GetBookViaAuthorVtable* vtable() {
            const static GetBookViaAuthorVtable s_vtable {
                .author = (GetBookViaAuthorVtable::author_fn)&GetBookViaAuthorPbSpec::author,
                .as_builder = (GetBookViaAuthorVtable::as_builder_fn)&GetBookViaAuthorPbSpec::as_builder,
                .serialize = (GetBookViaAuthorVtable::serialize_fn)&GetBookViaAuthorPbSpec::serialize,
            };
            return &s_vtable;
        }

        static inline GetBookViaAuthorEntityRef wrap(GetBookViaAuthor& a){
            return create_entity<GetBookViaAuthorEntityRef>(static_cast<void*>(&a), vtable());
        }
    };

    struct GetBookViaAuthorPbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_author(GetBookViaAuthor*, std::string);
        static void move_initialize_from(GetBookViaAuthor*, GetBookViaAuthor*);
        [[nodiscard]] static bool deserialize(GetBookViaAuthor*, std::string_view);
        [[nodiscard]] static GetBookViaAuthorEntityRef as_ref(void*);

        static inline const GetBookViaAuthorBuilderVtable* vtable() {
            const static GetBookViaAuthorBuilderVtable s_vtable {
                .set_author = (GetBookViaAuthorBuilderVtable::set_author_fn)&GetBookViaAuthorPbBuilderSpec::set_author,
                .move_initialize_from = (GetBookViaAuthorBuilderVtable::move_initialize_from_fn)&GetBookViaAuthorPbBuilderSpec::move_initialize_from,
                .deserialize = (GetBookViaAuthorBuilderVtable::deserialize_fn)&GetBookViaAuthorPbBuilderSpec::deserialize,
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
        [[nodiscard]] static std::string_view name(BookStore*);
        static BookStoreBuilder as_builder(BookStore*);
        static std::string serialize(BookStore*);

        static inline const BookStoreVtable* vtable() {
            const static BookStoreVtable s_vtable {
                .name = (BookStoreVtable::name_fn)&BookStorePbSpec::name,
                .as_builder = (BookStoreVtable::as_builder_fn)&BookStorePbSpec::as_builder,
                .serialize = (BookStoreVtable::serialize_fn)&BookStorePbSpec::serialize,
            };
            return &s_vtable;
        }

        static inline BookStoreEntityRef wrap(BookStore& a){
            return create_entity<BookStoreEntityRef>(static_cast<void*>(&a), vtable());
        }
    };

    struct BookStorePbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static void set_name(BookStore*, std::string);
        static void move_initialize_from(BookStore*, BookStore*);
        [[nodiscard]] static bool deserialize(BookStore*, std::string_view);
        [[nodiscard]] static BookStoreEntityRef as_ref(void*);

        static inline const BookStoreBuilderVtable* vtable() {
            const static BookStoreBuilderVtable s_vtable {
                .set_name = (BookStoreBuilderVtable::set_name_fn)&BookStorePbBuilderSpec::set_name,
                .move_initialize_from = (BookStoreBuilderVtable::move_initialize_from_fn)&BookStorePbBuilderSpec::move_initialize_from,
                .deserialize = (BookStoreBuilderVtable::deserialize_fn)&BookStorePbBuilderSpec::deserialize,
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
        [[nodiscard]] static CycleDep2EntityRef dep(CycleDep1*);
        static CycleDep1Builder as_builder(CycleDep1*);
        static std::string serialize(CycleDep1*);

        static inline const CycleDep1Vtable* vtable() {
            const static CycleDep1Vtable s_vtable {
                .dep = (CycleDep1Vtable::dep_fn)&CycleDep1PbSpec::dep,
                .as_builder = (CycleDep1Vtable::as_builder_fn)&CycleDep1PbSpec::as_builder,
                .serialize = (CycleDep1Vtable::serialize_fn)&CycleDep1PbSpec::serialize,
            };
            return &s_vtable;
        }

        static inline CycleDep1EntityRef wrap(CycleDep1& a){
            return create_entity<CycleDep1EntityRef>(static_cast<void*>(&a), vtable());
        }
    };

    struct CycleDep1PbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static CycleDep1BuilderVtable::dep_args_tuple_t dep_builder_args_factory(CycleDep1 *);
        static void move_initialize_from(CycleDep1*, CycleDep1*);
        [[nodiscard]] static bool deserialize(CycleDep1*, std::string_view);
        [[nodiscard]] static CycleDep1EntityRef as_ref(void*);

        static inline const CycleDep1BuilderVtable* vtable() {
            const static CycleDep1BuilderVtable s_vtable {
                .dep_builder_args_factory = (CycleDep1BuilderVtable::dep_builder_args_factory_fn)
                    &CycleDep1PbBuilderSpec::dep_builder_args_factory,
                .move_initialize_from = (CycleDep1BuilderVtable::move_initialize_from_fn)&CycleDep1PbBuilderSpec::move_initialize_from,
                .deserialize = (CycleDep1BuilderVtable::deserialize_fn)&CycleDep1PbBuilderSpec::deserialize,
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
        [[nodiscard]] static CycleDep1EntityRef dep(CycleDep2*);
        static CycleDep2Builder as_builder(CycleDep2*);
        static std::string serialize(CycleDep2*);

        static inline const CycleDep2Vtable* vtable() {
            const static CycleDep2Vtable s_vtable {
                .dep = (CycleDep2Vtable::dep_fn)&CycleDep2PbSpec::dep,
                .as_builder = (CycleDep2Vtable::as_builder_fn)&CycleDep2PbSpec::as_builder,
                .serialize = (CycleDep2Vtable::serialize_fn)&CycleDep2PbSpec::serialize,
            };
            return &s_vtable;
        }

        static inline CycleDep2EntityRef wrap(CycleDep2& a){
            return create_entity<CycleDep2EntityRef>(static_cast<void*>(&a), vtable());
        }
    };

    struct CycleDep2PbBuilderSpec: protected ::yazik::compiler::support::Initializer {
        static CycleDep2BuilderVtable::dep_args_tuple_t dep_builder_args_factory(CycleDep2 *);
        static void move_initialize_from(CycleDep2*, CycleDep2*);
        [[nodiscard]] static bool deserialize(CycleDep2*, std::string_view);
        [[nodiscard]] static CycleDep2EntityRef as_ref(void*);

        static inline const CycleDep2BuilderVtable* vtable() {
            const static CycleDep2BuilderVtable s_vtable {
                .dep_builder_args_factory = (CycleDep2BuilderVtable::dep_builder_args_factory_fn)
                    &CycleDep2PbBuilderSpec::dep_builder_args_factory,
                .move_initialize_from = (CycleDep2BuilderVtable::move_initialize_from_fn)&CycleDep2PbBuilderSpec::move_initialize_from,
                .deserialize = (CycleDep2BuilderVtable::deserialize_fn)&CycleDep2PbBuilderSpec::deserialize,
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

    inline std::string_view AuthorPbSpec::name(Author* a) {
        return a->name();
    }
    inline AuthorBuilder AuthorPbSpec::as_builder(Author* a) {
        return AuthorPbBuilderSpec::wrap(*a);
    }
    std::string AuthorPbSpec::serialize(Author* a){
        return a->SerializeAsString();
    }

    inline void AuthorPbBuilderSpec::set_name(Author* a, std::string value) {
        a->set_name(std::move(value));
    }
    inline void AuthorPbBuilderSpec::move_initialize_from(Author* a, Author* other) {
        a->Swap(other);
    }
    inline bool AuthorPbBuilderSpec::deserialize(Author* a, std::string_view data) {
        return a->ParseFromArray(data.data(), data.size());
    }
    inline AuthorEntityRef AuthorPbBuilderSpec::as_ref(void* ptr) {
        return create_entity<AuthorEntityRef>(ptr, AuthorPbSpec::vtable());
    }

    inline std::string_view InsideOneOfMessagePbSpec::field(InsideOneOfMessage* a) {
        return a->field();
    }
    inline InsideOneOfMessageBuilder InsideOneOfMessagePbSpec::as_builder(InsideOneOfMessage* a) {
        return InsideOneOfMessagePbBuilderSpec::wrap(*a);
    }
    std::string InsideOneOfMessagePbSpec::serialize(InsideOneOfMessage* a){
        return a->SerializeAsString();
    }

    inline void InsideOneOfMessagePbBuilderSpec::set_field(InsideOneOfMessage* a, std::string value) {
        a->set_field(std::move(value));
    }
    inline void InsideOneOfMessagePbBuilderSpec::move_initialize_from(InsideOneOfMessage* a, InsideOneOfMessage* other) {
        a->Swap(other);
    }
    inline bool InsideOneOfMessagePbBuilderSpec::deserialize(InsideOneOfMessage* a, std::string_view data) {
        return a->ParseFromArray(data.data(), data.size());
    }
    inline InsideOneOfMessageEntityRef InsideOneOfMessagePbBuilderSpec::as_ref(void* ptr) {
        return create_entity<InsideOneOfMessageEntityRef>(ptr, InsideOneOfMessagePbSpec::vtable());
    }

    inline ::ranges::any_view<std::string_view> BookPagePbSpec::lines(Book_Page* a) {
        return ::ranges::views::all(a->lines())
            | ::ranges::views::transform([](const auto& x) {
                return std::string_view { x };
            });
    }
    inline BookPageBuilder BookPagePbSpec::as_builder(Book_Page* a) {
        return BookPagePbBuilderSpec::wrap(*a);
    }
    std::string BookPagePbSpec::serialize(Book_Page* a){
        return a->SerializeAsString();
    }

    inline void BookPagePbBuilderSpec::lines_pusher(Book_Page* a, std::string value) {
        a->mutable_lines()->Add(std::move(value));
    }
    inline BookPageBuilderVtable::lines_args_tuple_t BookPagePbBuilderSpec::lines_builder_args_factory(Book_Page * a) {
        return { a, (::yazik::compiler::support::VecBuilder<std::string>::push_clbk_t)&BookPagePbBuilderSpec::lines_pusher };
    }
    inline void BookPagePbBuilderSpec::move_initialize_from(Book_Page* a, Book_Page* other) {
        a->Swap(other);
    }
    inline bool BookPagePbBuilderSpec::deserialize(Book_Page* a, std::string_view data) {
        return a->ParseFromArray(data.data(), data.size());
    }
    inline BookPageEntityRef BookPagePbBuilderSpec::as_ref(void* ptr) {
        return create_entity<BookPageEntityRef>(ptr, BookPagePbSpec::vtable());
    }

    inline bool BookFirstOneofPbSpec::is_first_oneof_string(const Book* a) {
        return a->first_oneof_case() == Book::kFirstOneofString;
    }
    inline std::string_view BookFirstOneofPbSpec::first_oneof_string(Book* a) {
        return a->first_oneof_string();
    }
    inline bool BookFirstOneofPbSpec::is_first_oneof_int(const Book* a) {
        return a->first_oneof_case() == Book::kFirstOneofInt;
    }
    inline int64_t BookFirstOneofPbSpec::first_oneof_int(Book* a) {
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
    inline std::string_view BookSecondOneofPbSpec::second_oneof_string(Book* a) {
        return a->second_oneof_string();
    }
    inline bool BookSecondOneofPbSpec::is_second_oneof_int(const Book* a) {
        return a->second_oneof_case() == Book::kSecondOneofInt;
    }
    inline int64_t BookSecondOneofPbSpec::second_oneof_int(Book* a) {
        return a->second_oneof_int();
    }
    inline bool BookSecondOneofPbSpec::is_second_oneof_message(const Book* a) {
        return a->second_oneof_case() == Book::kSecondOneofMessage;
    }
    inline InsideOneOfMessageEntityRef BookSecondOneofPbSpec::second_oneof_message(Book* a) {
        return InsideOneOfMessagePbSpec::wrap(*a->mutable_second_oneof_message());
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

    inline int64_t BookPbSpec::isbn(Book* a) {
        return a->isbn();
    }
    inline std::string_view BookPbSpec::title(Book* a) {
        return a->title();
    }
    inline AuthorEntityRef BookPbSpec::author(Book* a) {
        return AuthorPbSpec::wrap(*a->mutable_author());
    }
    inline ::ranges::any_view<BookPageEntityRef> BookPbSpec::pages(Book* a) {
        return ::ranges::views::all(*a->mutable_pages())
            | ::ranges::views::transform([](Book_Page& x) {
                return BookPagePbSpec::wrap(x);
            });
    }
    inline BookFirstOneofVariantRef BookPbSpec::first_oneof(Book* a) {
        return BookFirstOneofPbSpec::wrap(*a);
    }
    inline BookAvailabilityEnum BookPbSpec::availability(Book* a) {
        return create_enum<BookAvailabilityEnum>((int)a->availability());
    }
    inline CanRecommendEnum BookPbSpec::recommended(Book* a) {
        return create_enum<CanRecommendEnum>((int)a->recommended());
    }
    inline BookSecondOneofVariantRef BookPbSpec::second_oneof(Book* a) {
        return BookSecondOneofPbSpec::wrap(*a);
    }
    inline BookBuilder BookPbSpec::as_builder(Book* a) {
        return BookPbBuilderSpec::wrap(*a);
    }
    std::string BookPbSpec::serialize(Book* a){
        return a->SerializeAsString();
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
    inline void BookPbBuilderSpec::move_initialize_from(Book* a, Book* other) {
        a->Swap(other);
    }
    inline bool BookPbBuilderSpec::deserialize(Book* a, std::string_view data) {
        return a->ParseFromArray(data.data(), data.size());
    }
    inline BookEntityRef BookPbBuilderSpec::as_ref(void* ptr) {
        return create_entity<BookEntityRef>(ptr, BookPbSpec::vtable());
    }

    inline int64_t GetBookRequestPbSpec::isbn(GetBookRequest* a) {
        return a->isbn();
    }
    inline GetBookRequestBuilder GetBookRequestPbSpec::as_builder(GetBookRequest* a) {
        return GetBookRequestPbBuilderSpec::wrap(*a);
    }
    std::string GetBookRequestPbSpec::serialize(GetBookRequest* a){
        return a->SerializeAsString();
    }

    inline void GetBookRequestPbBuilderSpec::set_isbn(GetBookRequest* a, int64_t value) {
        a->set_isbn(std::move(value));
    }
    inline void GetBookRequestPbBuilderSpec::move_initialize_from(GetBookRequest* a, GetBookRequest* other) {
        a->Swap(other);
    }
    inline bool GetBookRequestPbBuilderSpec::deserialize(GetBookRequest* a, std::string_view data) {
        return a->ParseFromArray(data.data(), data.size());
    }
    inline GetBookRequestEntityRef GetBookRequestPbBuilderSpec::as_ref(void* ptr) {
        return create_entity<GetBookRequestEntityRef>(ptr, GetBookRequestPbSpec::vtable());
    }

    inline std::string_view GetBookViaAuthorPbSpec::author(GetBookViaAuthor* a) {
        return a->author();
    }
    inline GetBookViaAuthorBuilder GetBookViaAuthorPbSpec::as_builder(GetBookViaAuthor* a) {
        return GetBookViaAuthorPbBuilderSpec::wrap(*a);
    }
    std::string GetBookViaAuthorPbSpec::serialize(GetBookViaAuthor* a){
        return a->SerializeAsString();
    }

    inline void GetBookViaAuthorPbBuilderSpec::set_author(GetBookViaAuthor* a, std::string value) {
        a->set_author(std::move(value));
    }
    inline void GetBookViaAuthorPbBuilderSpec::move_initialize_from(GetBookViaAuthor* a, GetBookViaAuthor* other) {
        a->Swap(other);
    }
    inline bool GetBookViaAuthorPbBuilderSpec::deserialize(GetBookViaAuthor* a, std::string_view data) {
        return a->ParseFromArray(data.data(), data.size());
    }
    inline GetBookViaAuthorEntityRef GetBookViaAuthorPbBuilderSpec::as_ref(void* ptr) {
        return create_entity<GetBookViaAuthorEntityRef>(ptr, GetBookViaAuthorPbSpec::vtable());
    }

    inline std::string_view BookStorePbSpec::name(BookStore* a) {
        return a->name();
    }
    inline BookStoreBuilder BookStorePbSpec::as_builder(BookStore* a) {
        return BookStorePbBuilderSpec::wrap(*a);
    }
    std::string BookStorePbSpec::serialize(BookStore* a){
        return a->SerializeAsString();
    }

    inline void BookStorePbBuilderSpec::set_name(BookStore* a, std::string value) {
        a->set_name(std::move(value));
    }
    inline void BookStorePbBuilderSpec::move_initialize_from(BookStore* a, BookStore* other) {
        a->Swap(other);
    }
    inline bool BookStorePbBuilderSpec::deserialize(BookStore* a, std::string_view data) {
        return a->ParseFromArray(data.data(), data.size());
    }
    inline BookStoreEntityRef BookStorePbBuilderSpec::as_ref(void* ptr) {
        return create_entity<BookStoreEntityRef>(ptr, BookStorePbSpec::vtable());
    }

    inline CycleDep2EntityRef CycleDep1PbSpec::dep(CycleDep1* a) {
        return CycleDep2PbSpec::wrap(*a->mutable_dep());
    }
    inline CycleDep1Builder CycleDep1PbSpec::as_builder(CycleDep1* a) {
        return CycleDep1PbBuilderSpec::wrap(*a);
    }
    std::string CycleDep1PbSpec::serialize(CycleDep1* a){
        return a->SerializeAsString();
    }

    inline CycleDep1BuilderVtable::dep_args_tuple_t CycleDep1PbBuilderSpec::dep_builder_args_factory(CycleDep1 * a) {
        return { a->mutable_dep(), CycleDep2PbBuilderSpec::vtable() };
    }
    inline void CycleDep1PbBuilderSpec::move_initialize_from(CycleDep1* a, CycleDep1* other) {
        a->Swap(other);
    }
    inline bool CycleDep1PbBuilderSpec::deserialize(CycleDep1* a, std::string_view data) {
        return a->ParseFromArray(data.data(), data.size());
    }
    inline CycleDep1EntityRef CycleDep1PbBuilderSpec::as_ref(void* ptr) {
        return create_entity<CycleDep1EntityRef>(ptr, CycleDep1PbSpec::vtable());
    }

    inline CycleDep1EntityRef CycleDep2PbSpec::dep(CycleDep2* a) {
        return CycleDep1PbSpec::wrap(*a->mutable_dep());
    }
    inline CycleDep2Builder CycleDep2PbSpec::as_builder(CycleDep2* a) {
        return CycleDep2PbBuilderSpec::wrap(*a);
    }
    std::string CycleDep2PbSpec::serialize(CycleDep2* a){
        return a->SerializeAsString();
    }

    inline CycleDep2BuilderVtable::dep_args_tuple_t CycleDep2PbBuilderSpec::dep_builder_args_factory(CycleDep2 * a) {
        return { a->mutable_dep(), CycleDep1PbBuilderSpec::vtable() };
    }
    inline void CycleDep2PbBuilderSpec::move_initialize_from(CycleDep2* a, CycleDep2* other) {
        a->Swap(other);
    }
    inline bool CycleDep2PbBuilderSpec::deserialize(CycleDep2* a, std::string_view data) {
        return a->ParseFromArray(data.data(), data.size());
    }
    inline CycleDep2EntityRef CycleDep2PbBuilderSpec::as_ref(void* ptr) {
        return create_entity<CycleDep2EntityRef>(ptr, CycleDep2PbSpec::vtable());
    }

}
