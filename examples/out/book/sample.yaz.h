#pragma once

#include <yazik/compiler/support/support.hpp>

namespace com::book {
    class CanRecommendEnum;
    struct AuthorEntityRef;
    class AuthorBuilderVtable;
    class AuthorBuilder;
    template <typename Parent>
    class AuthorLayeredBuilder;
    struct InsideOneOfMessageEntityRef;
    class InsideOneOfMessageBuilderVtable;
    class InsideOneOfMessageBuilder;
    template <typename Parent>
    class InsideOneOfMessageLayeredBuilder;
    struct BookPageEntityRef;
    class BookPageBuilderVtable;
    class BookPageBuilder;
    template <typename Parent>
    class BookPageLayeredBuilder;
    class BookAvailabilityEnum;
    struct BookFirstOneofVariantRef;
    class BookFirstOneofVariantBuilder;
    struct BookSecondOneofVariantRef;
    class BookSecondOneofVariantBuilder;
    struct BookEntityRef;
    class BookBuilderVtable;
    class BookBuilder;
    template <typename Parent>
    class BookLayeredBuilder;
    struct GetBookRequestEntityRef;
    class GetBookRequestBuilderVtable;
    class GetBookRequestBuilder;
    template <typename Parent>
    class GetBookRequestLayeredBuilder;
    struct GetBookViaAuthorEntityRef;
    class GetBookViaAuthorBuilderVtable;
    class GetBookViaAuthorBuilder;
    template <typename Parent>
    class GetBookViaAuthorLayeredBuilder;
    struct BookStoreEntityRef;
    class BookStoreBuilderVtable;
    class BookStoreBuilder;
    template <typename Parent>
    class BookStoreLayeredBuilder;
    struct CycleDep1EntityRef;
    class CycleDep1BuilderVtable;
    class CycleDep1Builder;
    template <typename Parent>
    class CycleDep1LayeredBuilder;
    struct CycleDep2EntityRef;
    class CycleDep2BuilderVtable;
    class CycleDep2Builder;
    template <typename Parent>
    class CycleDep2LayeredBuilder;

    class CanRecommendEnum {
        int _value;
        CanRecommendEnum(int value) noexcept: _value{ value } {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        enum Value {
            Yep = 0,
            Nope = 3
        };

        [[nodiscard]] inline int which() const {
            return _value;
        }
        [[nodiscard]] inline bool is_yep() const {
            return _value == Yep;
        }
        [[nodiscard]] inline bool is_nope() const {
            return _value == Nope;
        }
    };

    struct AuthorVtable {
        using name_fn = std::string_view (*)(const void*);
        name_fn name;
        using as_builder_fn = AuthorBuilder (*)(void*);
        as_builder_fn as_builder;
        using serialize_fn = std::string (*)(void*);
        serialize_fn serialize;
    };

    class AuthorEntityRef {
        void* _ptr;
        const AuthorVtable* _vtable;

        AuthorEntityRef(void* ptr, const AuthorVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class AuthorBuilder;
        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = AuthorVtable;

        [[nodiscard]] std::string_view name() const;
        [[nodiscard]] AuthorBuilder as_builder();
        [[nodiscard]] std::string serialize();
    };

    struct AuthorBuilderVtable {
        using set_name_fn = void (*)(const void*, std::string);
        set_name_fn set_name;
        using move_initialize_from_fn = void (*)(void*, void*);
        move_initialize_from_fn move_initialize_from;
        using deserialize_fn = bool (*)(const void*, std::string_view);
        deserialize_fn deserialize;
        using as_ref_fn = AuthorEntityRef (*)(void*);
        as_ref_fn as_ref;
    };

    class AuthorBuilder : virtual protected ::yazik::compiler::support::Initializer {
    protected:
        void* _ptr;
        const AuthorBuilderVtable* _vtable;

        AuthorBuilder(void* ptr, const AuthorBuilderVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = AuthorBuilderVtable;

        void set_name(std::string);
        void move_initialize_from(AuthorEntityRef&&);
        [[nodiscard]] bool deserialize(std::string_view);
        [[nodiscard]] AuthorEntityRef as_ref();
    };

    template <typename Parent>
    class AuthorLayeredBuilder final
    : public AuthorBuilder
    , virtual protected ::yazik::compiler::support::Initializer {
    public:

        using Base = AuthorBuilder;

    private:

        using Self = AuthorLayeredBuilder<Parent>;
        Parent _parent;

        AuthorLayeredBuilder(void* ptr, const AuthorBuilderVtable* vtable, Parent* parent)
        : Base { ptr, vtable }
        , _parent { std::move(*parent) }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        Self& name(std::string);

        Self& move_from(AuthorEntityRef&&);

        inline Parent done() {
            return std::move(this->_parent);
        }

    };
    struct InsideOneOfMessageVtable {
        using field_fn = std::string_view (*)(const void*);
        field_fn field;
        using as_builder_fn = InsideOneOfMessageBuilder (*)(void*);
        as_builder_fn as_builder;
        using serialize_fn = std::string (*)(void*);
        serialize_fn serialize;
    };

    class InsideOneOfMessageEntityRef {
        void* _ptr;
        const InsideOneOfMessageVtable* _vtable;

        InsideOneOfMessageEntityRef(void* ptr, const InsideOneOfMessageVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class InsideOneOfMessageBuilder;
        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = InsideOneOfMessageVtable;

        [[nodiscard]] std::string_view field() const;
        [[nodiscard]] InsideOneOfMessageBuilder as_builder();
        [[nodiscard]] std::string serialize();
    };

    struct InsideOneOfMessageBuilderVtable {
        using set_field_fn = void (*)(const void*, std::string);
        set_field_fn set_field;
        using move_initialize_from_fn = void (*)(void*, void*);
        move_initialize_from_fn move_initialize_from;
        using deserialize_fn = bool (*)(const void*, std::string_view);
        deserialize_fn deserialize;
        using as_ref_fn = InsideOneOfMessageEntityRef (*)(void*);
        as_ref_fn as_ref;
    };

    class InsideOneOfMessageBuilder : virtual protected ::yazik::compiler::support::Initializer {
    protected:
        void* _ptr;
        const InsideOneOfMessageBuilderVtable* _vtable;

        InsideOneOfMessageBuilder(void* ptr, const InsideOneOfMessageBuilderVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = InsideOneOfMessageBuilderVtable;

        void set_field(std::string);
        void move_initialize_from(InsideOneOfMessageEntityRef&&);
        [[nodiscard]] bool deserialize(std::string_view);
        [[nodiscard]] InsideOneOfMessageEntityRef as_ref();
    };

    template <typename Parent>
    class InsideOneOfMessageLayeredBuilder final
    : public InsideOneOfMessageBuilder
    , virtual protected ::yazik::compiler::support::Initializer {
    public:

        using Base = InsideOneOfMessageBuilder;

    private:

        using Self = InsideOneOfMessageLayeredBuilder<Parent>;
        Parent _parent;

        InsideOneOfMessageLayeredBuilder(void* ptr, const InsideOneOfMessageBuilderVtable* vtable, Parent* parent)
        : Base { ptr, vtable }
        , _parent { std::move(*parent) }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        Self& field(std::string);

        Self& move_from(InsideOneOfMessageEntityRef&&);

        inline Parent done() {
            return std::move(this->_parent);
        }

    };
    struct BookPageVtable {
        using lines_fn = ::ranges::any_view<std::string_view> (*)(const void*);
        lines_fn lines;
        using as_builder_fn = BookPageBuilder (*)(void*);
        as_builder_fn as_builder;
        using serialize_fn = std::string (*)(void*);
        serialize_fn serialize;
    };

    class BookPageEntityRef {
        void* _ptr;
        const BookPageVtable* _vtable;

        BookPageEntityRef(void* ptr, const BookPageVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class BookPageBuilder;
        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = BookPageVtable;

        [[nodiscard]] ::ranges::any_view<std::string_view> lines() const;
        [[nodiscard]] BookPageBuilder as_builder();
        [[nodiscard]] std::string serialize();
    };

    struct BookPageBuilderVtable {
        using lines_args_tuple_t = std::tuple<
            void*,
            ::yazik::compiler::support::VecBuilder<std::string>::push_clbk_t
        >;
        using lines_builder_args_factory_fn = lines_args_tuple_t (*)(void*);
        lines_builder_args_factory_fn lines_builder_args_factory;
        using move_initialize_from_fn = void (*)(void*, void*);
        move_initialize_from_fn move_initialize_from;
        using deserialize_fn = bool (*)(const void*, std::string_view);
        deserialize_fn deserialize;
        using as_ref_fn = BookPageEntityRef (*)(void*);
        as_ref_fn as_ref;
    };

    template <typename Fn>
    concept c_book_page_lines_clbk = requires(Fn&& fn, ::yazik::compiler::support::VecBuilder<std::string>& builder){
        { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
    };

    class BookPageBuilder : virtual protected ::yazik::compiler::support::Initializer {
    protected:
        void* _ptr;
        const BookPageBuilderVtable* _vtable;

        BookPageBuilder(void* ptr, const BookPageBuilderVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = BookPageBuilderVtable;

        template <c_book_page_lines_clbk Fn>
        void set_lines(Fn&&);
        void move_initialize_from(BookPageEntityRef&&);
        [[nodiscard]] bool deserialize(std::string_view);
        [[nodiscard]] BookPageEntityRef as_ref();
    };

    template <typename Parent>
    class BookPageLayeredBuilder final
    : public BookPageBuilder
    , virtual protected ::yazik::compiler::support::Initializer {
    public:

        using Base = BookPageBuilder;

    private:

        using Self = BookPageLayeredBuilder<Parent>;
        Parent _parent;

        BookPageLayeredBuilder(void* ptr, const BookPageBuilderVtable* vtable, Parent* parent)
        : Base { ptr, vtable }
        , _parent { std::move(*parent) }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        ::yazik::compiler::support::VecLayeredBuilder<std::string, Self> lines();

        Self& move_from(BookPageEntityRef&&);

        inline Parent done() {
            return std::move(this->_parent);
        }

    };
    class BookAvailabilityEnum {
        int _value;
        BookAvailabilityEnum(int value) noexcept: _value{ value } {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        enum Value {
            Available = 0,
            NotAvailable = 1
        };

        [[nodiscard]] inline int which() const {
            return _value;
        }
        [[nodiscard]] inline bool is_available() const {
            return _value == Available;
        }
        [[nodiscard]] inline bool is_not_available() const {
            return _value == NotAvailable;
        }
    };

    struct BookFirstOneofVtable {
        using is_first_oneof_string_fn = bool (*)(const void*);
        is_first_oneof_string_fn is_first_oneof_string;
        using first_oneof_string_fn = std::string_view (*)(const void*);
        first_oneof_string_fn first_oneof_string;
        using is_first_oneof_int_fn = bool (*)(const void*);
        is_first_oneof_int_fn is_first_oneof_int;
        using first_oneof_int_fn = int64_t (*)(const void*);
        first_oneof_int_fn first_oneof_int;
        using is_null_fn = bool (*)(const void*);
        is_null_fn is_null;
    };

    class BookFirstOneofVariantRef {
        const void* _ptr;
        const BookFirstOneofVtable* _vtable;

        BookFirstOneofVariantRef(const void* ptr, const BookFirstOneofVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = BookFirstOneofVtable;

        [[nodiscard]] bool is_first_oneof_string() const;
        [[nodiscard]] std::string_view first_oneof_string() const;
        [[nodiscard]] bool is_first_oneof_int() const;
        [[nodiscard]] int64_t first_oneof_int() const;
        [[nodiscard]] bool is_null() const;};

    struct BookFirstOneofBuilderVtable {
        using set_first_oneof_string_fn = void (*)(void*, std::string);
        set_first_oneof_string_fn set_first_oneof_string;
        using set_first_oneof_int_fn = void (*)(void*, int64_t);
        set_first_oneof_int_fn set_first_oneof_int;
    };

    class BookFirstOneofVariantBuilder {
        void* _ptr;
        const BookFirstOneofBuilderVtable* _vtable;

        BookFirstOneofVariantBuilder(void* ptr, const BookFirstOneofBuilderVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = BookFirstOneofBuilderVtable;

        void set_first_oneof_string(std::string);
        void set_first_oneof_int(int64_t);
    };

    struct BookSecondOneofVtable {
        using is_second_oneof_string_fn = bool (*)(const void*);
        is_second_oneof_string_fn is_second_oneof_string;
        using second_oneof_string_fn = std::string_view (*)(const void*);
        second_oneof_string_fn second_oneof_string;
        using is_second_oneof_int_fn = bool (*)(const void*);
        is_second_oneof_int_fn is_second_oneof_int;
        using second_oneof_int_fn = int64_t (*)(const void*);
        second_oneof_int_fn second_oneof_int;
        using is_second_oneof_message_fn = bool (*)(const void*);
        is_second_oneof_message_fn is_second_oneof_message;
        using second_oneof_message_fn = InsideOneOfMessageEntityRef (*)(const void*);
        second_oneof_message_fn second_oneof_message;
        using is_null_fn = bool (*)(const void*);
        is_null_fn is_null;
    };

    class BookSecondOneofVariantRef {
        const void* _ptr;
        const BookSecondOneofVtable* _vtable;

        BookSecondOneofVariantRef(const void* ptr, const BookSecondOneofVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = BookSecondOneofVtable;

        [[nodiscard]] bool is_second_oneof_string() const;
        [[nodiscard]] std::string_view second_oneof_string() const;
        [[nodiscard]] bool is_second_oneof_int() const;
        [[nodiscard]] int64_t second_oneof_int() const;
        [[nodiscard]] bool is_second_oneof_message() const;
        [[nodiscard]] InsideOneOfMessageEntityRef second_oneof_message() const;
        [[nodiscard]] bool is_null() const;};

    struct BookSecondOneofBuilderVtable {
        using set_second_oneof_string_fn = void (*)(void*, std::string);
        set_second_oneof_string_fn set_second_oneof_string;
        using set_second_oneof_int_fn = void (*)(void*, int64_t);
        set_second_oneof_int_fn set_second_oneof_int;
        using set_second_oneof_message_fn = void (*)(void*, ::folly::Function<void(InsideOneOfMessageBuilder&)>);
        set_second_oneof_message_fn set_second_oneof_message;
    };

    class BookSecondOneofVariantBuilder {
        void* _ptr;
        const BookSecondOneofBuilderVtable* _vtable;

        BookSecondOneofVariantBuilder(void* ptr, const BookSecondOneofBuilderVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = BookSecondOneofBuilderVtable;

        void set_second_oneof_string(std::string);
        void set_second_oneof_int(int64_t);
        void set_second_oneof_message(::folly::Function<void(InsideOneOfMessageBuilder&)>);
    };

    struct BookVtable {
        using isbn_fn = int64_t (*)(const void*);
        isbn_fn isbn;
        using title_fn = std::string_view (*)(const void*);
        title_fn title;
        using author_fn = AuthorEntityRef (*)(const void*);
        author_fn author;
        using pages_fn = ::ranges::any_view<BookPageEntityRef> (*)(const void*);
        pages_fn pages;
        using first_oneof_fn = BookFirstOneofVariantRef (*)(const void*);
        first_oneof_fn first_oneof;
        using availability_fn = BookAvailabilityEnum (*)(const void*);
        availability_fn availability;
        using recommended_fn = CanRecommendEnum (*)(const void*);
        recommended_fn recommended;
        using second_oneof_fn = BookSecondOneofVariantRef (*)(const void*);
        second_oneof_fn second_oneof;
        using as_builder_fn = BookBuilder (*)(void*);
        as_builder_fn as_builder;
        using serialize_fn = std::string (*)(void*);
        serialize_fn serialize;
    };

    class BookEntityRef {
        void* _ptr;
        const BookVtable* _vtable;

        BookEntityRef(void* ptr, const BookVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class BookBuilder;
        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = BookVtable;

        [[nodiscard]] int64_t isbn() const;
        [[nodiscard]] std::string_view title() const;
        [[nodiscard]] AuthorEntityRef author() const;
        [[nodiscard]] ::ranges::any_view<BookPageEntityRef> pages() const;
        [[nodiscard]] BookFirstOneofVariantRef first_oneof() const;
        [[nodiscard]] BookAvailabilityEnum availability() const;
        [[nodiscard]] CanRecommendEnum recommended() const;
        [[nodiscard]] BookSecondOneofVariantRef second_oneof() const;
        [[nodiscard]] BookBuilder as_builder();
        [[nodiscard]] std::string serialize();
    };

    struct BookBuilderVtable {
        using set_isbn_fn = void (*)(const void*, int64_t);
        set_isbn_fn set_isbn;
        using set_title_fn = void (*)(const void*, std::string);
        set_title_fn set_title;
        using author_args_tuple_t = std::tuple<
            void*,
            const AuthorBuilderVtable*
        >;
        using author_builder_args_factory_fn = author_args_tuple_t (*)(void*);
        author_builder_args_factory_fn author_builder_args_factory;
        using pages_args_tuple_t = std::tuple<
            void*,
            ::yazik::compiler::support::VecEntityBuilder<BookPageBuilder, BookPageBuilderVtable>::builder_factory_t,
            const BookPageBuilderVtable*
        >;
        using pages_builder_args_factory_fn = pages_args_tuple_t (*)(void*);
        pages_builder_args_factory_fn pages_builder_args_factory;
        using first_oneof_args_tuple_t = std::tuple<
            void*,
            const BookFirstOneofBuilderVtable*
        >;
        using first_oneof_builder_args_factory_fn = first_oneof_args_tuple_t (*)(void*);
        first_oneof_builder_args_factory_fn first_oneof_builder_args_factory;
        using set_availability_fn = void (*)(const void*, BookAvailabilityEnum::Value);
        set_availability_fn set_availability;
        using set_recommended_fn = void (*)(const void*, CanRecommendEnum::Value);
        set_recommended_fn set_recommended;
        using second_oneof_args_tuple_t = std::tuple<
            void*,
            const BookSecondOneofBuilderVtable*
        >;
        using second_oneof_builder_args_factory_fn = second_oneof_args_tuple_t (*)(void*);
        second_oneof_builder_args_factory_fn second_oneof_builder_args_factory;
        using move_initialize_from_fn = void (*)(void*, void*);
        move_initialize_from_fn move_initialize_from;
        using deserialize_fn = bool (*)(const void*, std::string_view);
        deserialize_fn deserialize;
        using as_ref_fn = BookEntityRef (*)(void*);
        as_ref_fn as_ref;
    };

    template <typename Fn>
    concept c_book_author_clbk = requires(Fn&& fn, AuthorBuilder& builder){
        { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
    };

    template <typename Fn>
    concept c_book_pages_clbk = requires(Fn&& fn, ::yazik::compiler::support::VecEntityBuilder<BookPageBuilder, BookPageBuilderVtable>& builder){
        { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
    };

    template <typename Fn>
    concept c_book_first_oneof_clbk = requires(Fn&& fn, BookFirstOneofVariantBuilder& builder){
        { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
    };

    template <typename Fn>
    concept c_book_second_oneof_clbk = requires(Fn&& fn, BookSecondOneofVariantBuilder& builder){
        { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
    };

    class BookBuilder : virtual protected ::yazik::compiler::support::Initializer {
    protected:
        void* _ptr;
        const BookBuilderVtable* _vtable;

        BookBuilder(void* ptr, const BookBuilderVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = BookBuilderVtable;

        void set_isbn(int64_t);
        void set_title(std::string);
        template <c_book_author_clbk Fn>
        void set_author(Fn&&);
        template <c_book_pages_clbk Fn>
        void set_pages(Fn&&);
        template <c_book_first_oneof_clbk Fn>
        void set_first_oneof(Fn&&);
        void set_availability(BookAvailabilityEnum::Value);
        void set_recommended(CanRecommendEnum::Value);
        template <c_book_second_oneof_clbk Fn>
        void set_second_oneof(Fn&&);
        void move_initialize_from(BookEntityRef&&);
        [[nodiscard]] bool deserialize(std::string_view);
        [[nodiscard]] BookEntityRef as_ref();
    };

    template <typename Parent>
    class BookLayeredBuilder final
    : public BookBuilder
    , virtual protected ::yazik::compiler::support::Initializer {
    public:

        using Base = BookBuilder;

    private:

        using Self = BookLayeredBuilder<Parent>;
        Parent _parent;

        BookLayeredBuilder(void* ptr, const BookBuilderVtable* vtable, Parent* parent)
        : Base { ptr, vtable }
        , _parent { std::move(*parent) }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        Self& isbn(int64_t);
        Self& title(std::string);
        AuthorLayeredBuilder<Self> author();
        ::yazik::compiler::support::VecEntityLayeredBuilder<BookPageLayeredBuilder, BookPageBuilder, Self> pages();
        Self& availability(BookAvailabilityEnum::Value);
        Self& recommended(CanRecommendEnum::Value);

        Self& move_from(BookEntityRef&&);

        inline Parent done() {
            return std::move(this->_parent);
        }

    };
    struct GetBookRequestVtable {
        using isbn_fn = int64_t (*)(const void*);
        isbn_fn isbn;
        using as_builder_fn = GetBookRequestBuilder (*)(void*);
        as_builder_fn as_builder;
        using serialize_fn = std::string (*)(void*);
        serialize_fn serialize;
    };

    class GetBookRequestEntityRef {
        void* _ptr;
        const GetBookRequestVtable* _vtable;

        GetBookRequestEntityRef(void* ptr, const GetBookRequestVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class GetBookRequestBuilder;
        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = GetBookRequestVtable;

        [[nodiscard]] int64_t isbn() const;
        [[nodiscard]] GetBookRequestBuilder as_builder();
        [[nodiscard]] std::string serialize();
    };

    struct GetBookRequestBuilderVtable {
        using set_isbn_fn = void (*)(const void*, int64_t);
        set_isbn_fn set_isbn;
        using move_initialize_from_fn = void (*)(void*, void*);
        move_initialize_from_fn move_initialize_from;
        using deserialize_fn = bool (*)(const void*, std::string_view);
        deserialize_fn deserialize;
        using as_ref_fn = GetBookRequestEntityRef (*)(void*);
        as_ref_fn as_ref;
    };

    class GetBookRequestBuilder : virtual protected ::yazik::compiler::support::Initializer {
    protected:
        void* _ptr;
        const GetBookRequestBuilderVtable* _vtable;

        GetBookRequestBuilder(void* ptr, const GetBookRequestBuilderVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = GetBookRequestBuilderVtable;

        void set_isbn(int64_t);
        void move_initialize_from(GetBookRequestEntityRef&&);
        [[nodiscard]] bool deserialize(std::string_view);
        [[nodiscard]] GetBookRequestEntityRef as_ref();
    };

    template <typename Parent>
    class GetBookRequestLayeredBuilder final
    : public GetBookRequestBuilder
    , virtual protected ::yazik::compiler::support::Initializer {
    public:

        using Base = GetBookRequestBuilder;

    private:

        using Self = GetBookRequestLayeredBuilder<Parent>;
        Parent _parent;

        GetBookRequestLayeredBuilder(void* ptr, const GetBookRequestBuilderVtable* vtable, Parent* parent)
        : Base { ptr, vtable }
        , _parent { std::move(*parent) }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        Self& isbn(int64_t);

        Self& move_from(GetBookRequestEntityRef&&);

        inline Parent done() {
            return std::move(this->_parent);
        }

    };
    struct GetBookViaAuthorVtable {
        using author_fn = std::string_view (*)(const void*);
        author_fn author;
        using as_builder_fn = GetBookViaAuthorBuilder (*)(void*);
        as_builder_fn as_builder;
        using serialize_fn = std::string (*)(void*);
        serialize_fn serialize;
    };

    class GetBookViaAuthorEntityRef {
        void* _ptr;
        const GetBookViaAuthorVtable* _vtable;

        GetBookViaAuthorEntityRef(void* ptr, const GetBookViaAuthorVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class GetBookViaAuthorBuilder;
        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = GetBookViaAuthorVtable;

        [[nodiscard]] std::string_view author() const;
        [[nodiscard]] GetBookViaAuthorBuilder as_builder();
        [[nodiscard]] std::string serialize();
    };

    struct GetBookViaAuthorBuilderVtable {
        using set_author_fn = void (*)(const void*, std::string);
        set_author_fn set_author;
        using move_initialize_from_fn = void (*)(void*, void*);
        move_initialize_from_fn move_initialize_from;
        using deserialize_fn = bool (*)(const void*, std::string_view);
        deserialize_fn deserialize;
        using as_ref_fn = GetBookViaAuthorEntityRef (*)(void*);
        as_ref_fn as_ref;
    };

    class GetBookViaAuthorBuilder : virtual protected ::yazik::compiler::support::Initializer {
    protected:
        void* _ptr;
        const GetBookViaAuthorBuilderVtable* _vtable;

        GetBookViaAuthorBuilder(void* ptr, const GetBookViaAuthorBuilderVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = GetBookViaAuthorBuilderVtable;

        void set_author(std::string);
        void move_initialize_from(GetBookViaAuthorEntityRef&&);
        [[nodiscard]] bool deserialize(std::string_view);
        [[nodiscard]] GetBookViaAuthorEntityRef as_ref();
    };

    template <typename Parent>
    class GetBookViaAuthorLayeredBuilder final
    : public GetBookViaAuthorBuilder
    , virtual protected ::yazik::compiler::support::Initializer {
    public:

        using Base = GetBookViaAuthorBuilder;

    private:

        using Self = GetBookViaAuthorLayeredBuilder<Parent>;
        Parent _parent;

        GetBookViaAuthorLayeredBuilder(void* ptr, const GetBookViaAuthorBuilderVtable* vtable, Parent* parent)
        : Base { ptr, vtable }
        , _parent { std::move(*parent) }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        Self& author(std::string);

        Self& move_from(GetBookViaAuthorEntityRef&&);

        inline Parent done() {
            return std::move(this->_parent);
        }

    };
    struct BookStoreVtable {
        using name_fn = std::string_view (*)(const void*);
        name_fn name;
        using as_builder_fn = BookStoreBuilder (*)(void*);
        as_builder_fn as_builder;
        using serialize_fn = std::string (*)(void*);
        serialize_fn serialize;
    };

    class BookStoreEntityRef {
        void* _ptr;
        const BookStoreVtable* _vtable;

        BookStoreEntityRef(void* ptr, const BookStoreVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class BookStoreBuilder;
        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = BookStoreVtable;

        [[nodiscard]] std::string_view name() const;
        [[nodiscard]] BookStoreBuilder as_builder();
        [[nodiscard]] std::string serialize();
    };

    struct BookStoreBuilderVtable {
        using set_name_fn = void (*)(const void*, std::string);
        set_name_fn set_name;
        using move_initialize_from_fn = void (*)(void*, void*);
        move_initialize_from_fn move_initialize_from;
        using deserialize_fn = bool (*)(const void*, std::string_view);
        deserialize_fn deserialize;
        using as_ref_fn = BookStoreEntityRef (*)(void*);
        as_ref_fn as_ref;
    };

    class BookStoreBuilder : virtual protected ::yazik::compiler::support::Initializer {
    protected:
        void* _ptr;
        const BookStoreBuilderVtable* _vtable;

        BookStoreBuilder(void* ptr, const BookStoreBuilderVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = BookStoreBuilderVtable;

        void set_name(std::string);
        void move_initialize_from(BookStoreEntityRef&&);
        [[nodiscard]] bool deserialize(std::string_view);
        [[nodiscard]] BookStoreEntityRef as_ref();
    };

    template <typename Parent>
    class BookStoreLayeredBuilder final
    : public BookStoreBuilder
    , virtual protected ::yazik::compiler::support::Initializer {
    public:

        using Base = BookStoreBuilder;

    private:

        using Self = BookStoreLayeredBuilder<Parent>;
        Parent _parent;

        BookStoreLayeredBuilder(void* ptr, const BookStoreBuilderVtable* vtable, Parent* parent)
        : Base { ptr, vtable }
        , _parent { std::move(*parent) }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        Self& name(std::string);

        Self& move_from(BookStoreEntityRef&&);

        inline Parent done() {
            return std::move(this->_parent);
        }

    };
    struct CycleDep1Vtable {
        using dep_fn = CycleDep2EntityRef (*)(const void*);
        dep_fn dep;
        using as_builder_fn = CycleDep1Builder (*)(void*);
        as_builder_fn as_builder;
        using serialize_fn = std::string (*)(void*);
        serialize_fn serialize;
    };

    class CycleDep1EntityRef {
        void* _ptr;
        const CycleDep1Vtable* _vtable;

        CycleDep1EntityRef(void* ptr, const CycleDep1Vtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class CycleDep1Builder;
        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = CycleDep1Vtable;

        [[nodiscard]] CycleDep2EntityRef dep() const;
        [[nodiscard]] CycleDep1Builder as_builder();
        [[nodiscard]] std::string serialize();
    };

    struct CycleDep1BuilderVtable {
        using dep_args_tuple_t = std::tuple<
            void*,
            const CycleDep2BuilderVtable*
        >;
        using dep_builder_args_factory_fn = dep_args_tuple_t (*)(void*);
        dep_builder_args_factory_fn dep_builder_args_factory;
        using move_initialize_from_fn = void (*)(void*, void*);
        move_initialize_from_fn move_initialize_from;
        using deserialize_fn = bool (*)(const void*, std::string_view);
        deserialize_fn deserialize;
        using as_ref_fn = CycleDep1EntityRef (*)(void*);
        as_ref_fn as_ref;
    };

    template <typename Fn>
    concept c_cycle_dep1_dep_clbk = requires(Fn&& fn, CycleDep2Builder& builder){
        { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
    };

    class CycleDep1Builder : virtual protected ::yazik::compiler::support::Initializer {
    protected:
        void* _ptr;
        const CycleDep1BuilderVtable* _vtable;

        CycleDep1Builder(void* ptr, const CycleDep1BuilderVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = CycleDep1BuilderVtable;

        template <c_cycle_dep1_dep_clbk Fn>
        void set_dep(Fn&&);
        void move_initialize_from(CycleDep1EntityRef&&);
        [[nodiscard]] bool deserialize(std::string_view);
        [[nodiscard]] CycleDep1EntityRef as_ref();
    };

    template <typename Parent>
    class CycleDep1LayeredBuilder final
    : public CycleDep1Builder
    , virtual protected ::yazik::compiler::support::Initializer {
    public:

        using Base = CycleDep1Builder;

    private:

        using Self = CycleDep1LayeredBuilder<Parent>;
        Parent _parent;

        CycleDep1LayeredBuilder(void* ptr, const CycleDep1BuilderVtable* vtable, Parent* parent)
        : Base { ptr, vtable }
        , _parent { std::move(*parent) }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        CycleDep2LayeredBuilder<Self> dep();

        Self& move_from(CycleDep1EntityRef&&);

        inline Parent done() {
            return std::move(this->_parent);
        }

    };
    struct CycleDep2Vtable {
        using dep_fn = CycleDep1EntityRef (*)(const void*);
        dep_fn dep;
        using as_builder_fn = CycleDep2Builder (*)(void*);
        as_builder_fn as_builder;
        using serialize_fn = std::string (*)(void*);
        serialize_fn serialize;
    };

    class CycleDep2EntityRef {
        void* _ptr;
        const CycleDep2Vtable* _vtable;

        CycleDep2EntityRef(void* ptr, const CycleDep2Vtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class CycleDep2Builder;
        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = CycleDep2Vtable;

        [[nodiscard]] CycleDep1EntityRef dep() const;
        [[nodiscard]] CycleDep2Builder as_builder();
        [[nodiscard]] std::string serialize();
    };

    struct CycleDep2BuilderVtable {
        using dep_args_tuple_t = std::tuple<
            void*,
            const CycleDep1BuilderVtable*
        >;
        using dep_builder_args_factory_fn = dep_args_tuple_t (*)(void*);
        dep_builder_args_factory_fn dep_builder_args_factory;
        using move_initialize_from_fn = void (*)(void*, void*);
        move_initialize_from_fn move_initialize_from;
        using deserialize_fn = bool (*)(const void*, std::string_view);
        deserialize_fn deserialize;
        using as_ref_fn = CycleDep2EntityRef (*)(void*);
        as_ref_fn as_ref;
    };

    template <typename Fn>
    concept c_cycle_dep2_dep_clbk = requires(Fn&& fn, CycleDep1Builder& builder){
        { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
    };

    class CycleDep2Builder : virtual protected ::yazik::compiler::support::Initializer {
    protected:
        void* _ptr;
        const CycleDep2BuilderVtable* _vtable;

        CycleDep2Builder(void* ptr, const CycleDep2BuilderVtable* vtable) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        using Vtable = CycleDep2BuilderVtable;

        template <c_cycle_dep2_dep_clbk Fn>
        void set_dep(Fn&&);
        void move_initialize_from(CycleDep2EntityRef&&);
        [[nodiscard]] bool deserialize(std::string_view);
        [[nodiscard]] CycleDep2EntityRef as_ref();
    };

    template <typename Parent>
    class CycleDep2LayeredBuilder final
    : public CycleDep2Builder
    , virtual protected ::yazik::compiler::support::Initializer {
    public:

        using Base = CycleDep2Builder;

    private:

        using Self = CycleDep2LayeredBuilder<Parent>;
        Parent _parent;

        CycleDep2LayeredBuilder(void* ptr, const CycleDep2BuilderVtable* vtable, Parent* parent)
        : Base { ptr, vtable }
        , _parent { std::move(*parent) }
        {}

        friend class ::yazik::compiler::support::Initializer;

    public:

        CycleDep1LayeredBuilder<Self> dep();

        Self& move_from(CycleDep2EntityRef&&);

        inline Parent done() {
            return std::move(this->_parent);
        }

    };
    inline std::string_view AuthorEntityRef::name() const {
        return _vtable->name(_ptr);
    }
    AuthorBuilder AuthorEntityRef::as_builder() {
        return _vtable->as_builder(_ptr);
    }
    [[nodiscard]] std::string AuthorEntityRef::serialize() {
        return _vtable->serialize(_ptr);
    }

    inline void AuthorBuilder::set_name(std::string value) {
        _vtable->set_name(_ptr, std::move(value));
    }
    inline void AuthorBuilder::move_initialize_from(AuthorEntityRef&& e) 
    {
        return _vtable->move_initialize_from(_ptr, e._ptr);
    }
    inline bool AuthorBuilder::deserialize(std::string_view data) {
        return _vtable->deserialize(_ptr, std::move(data));
    }
    inline AuthorEntityRef AuthorBuilder::as_ref() {
        return _vtable->as_ref(_ptr);
    }

    template<typename Parent>
    inline auto AuthorLayeredBuilder<Parent>::name(std::string value) -> Self& {
        this->set_name(std::move(value));
        return *this;
    }
    template<typename Parent>
    auto AuthorLayeredBuilder<Parent>::move_from(AuthorEntityRef&& r) -> Self&
    {
        this->move_initialize_from(std::move(r));
        return *this;
    }

    inline std::string_view InsideOneOfMessageEntityRef::field() const {
        return _vtable->field(_ptr);
    }
    InsideOneOfMessageBuilder InsideOneOfMessageEntityRef::as_builder() {
        return _vtable->as_builder(_ptr);
    }
    [[nodiscard]] std::string InsideOneOfMessageEntityRef::serialize() {
        return _vtable->serialize(_ptr);
    }

    inline void InsideOneOfMessageBuilder::set_field(std::string value) {
        _vtable->set_field(_ptr, std::move(value));
    }
    inline void InsideOneOfMessageBuilder::move_initialize_from(InsideOneOfMessageEntityRef&& e) 
    {
        return _vtable->move_initialize_from(_ptr, e._ptr);
    }
    inline bool InsideOneOfMessageBuilder::deserialize(std::string_view data) {
        return _vtable->deserialize(_ptr, std::move(data));
    }
    inline InsideOneOfMessageEntityRef InsideOneOfMessageBuilder::as_ref() {
        return _vtable->as_ref(_ptr);
    }

    template<typename Parent>
    inline auto InsideOneOfMessageLayeredBuilder<Parent>::field(std::string value) -> Self& {
        this->set_field(std::move(value));
        return *this;
    }
    template<typename Parent>
    auto InsideOneOfMessageLayeredBuilder<Parent>::move_from(InsideOneOfMessageEntityRef&& r) -> Self&
    {
        this->move_initialize_from(std::move(r));
        return *this;
    }

    inline ::ranges::any_view<std::string_view> BookPageEntityRef::lines() const {
        return _vtable->lines(_ptr);
    }
    BookPageBuilder BookPageEntityRef::as_builder() {
        return _vtable->as_builder(_ptr);
    }
    [[nodiscard]] std::string BookPageEntityRef::serialize() {
        return _vtable->serialize(_ptr);
    }

    template <c_book_page_lines_clbk Fn>
    inline void BookPageBuilder::set_lines(Fn&& fn){
        auto args = _vtable->lines_builder_args_factory(_ptr);
        auto builder = create_builder<::yazik::compiler::support::VecBuilder<std::string>>(std::move(args));
        fn(builder);
    }
    inline void BookPageBuilder::move_initialize_from(BookPageEntityRef&& e) 
    {
        return _vtable->move_initialize_from(_ptr, e._ptr);
    }
    inline bool BookPageBuilder::deserialize(std::string_view data) {
        return _vtable->deserialize(_ptr, std::move(data));
    }
    inline BookPageEntityRef BookPageBuilder::as_ref() {
        return _vtable->as_ref(_ptr);
    }

    template<typename Parent>
    inline auto BookPageLayeredBuilder<Parent>::lines() -> ::yazik::compiler::support::VecLayeredBuilder<std::string, Self> {
        auto args = this->_vtable->lines_builder_args_factory(this->_ptr);
        return create_builder_with_parent<::yazik::compiler::support::VecLayeredBuilder<std::string, Self>>(std::move(args), this);
    }
    template<typename Parent>
    auto BookPageLayeredBuilder<Parent>::move_from(BookPageEntityRef&& r) -> Self&
    {
        this->move_initialize_from(std::move(r));
        return *this;
    }

    inline bool BookFirstOneofVariantRef::is_first_oneof_string() const {
        return _vtable->is_first_oneof_string(_ptr);
    }
    inline std::string_view BookFirstOneofVariantRef::first_oneof_string() const {
        return _vtable->first_oneof_string(_ptr);
    }
    inline bool BookFirstOneofVariantRef::is_first_oneof_int() const {
        return _vtable->is_first_oneof_int(_ptr);
    }
    inline int64_t BookFirstOneofVariantRef::first_oneof_int() const {
        return _vtable->first_oneof_int(_ptr);
    }
    inline bool BookFirstOneofVariantRef::is_null() const {
        return _vtable->is_null(_ptr);
    }

    inline void BookFirstOneofVariantBuilder::set_first_oneof_string(std::string value) {
        _vtable->set_first_oneof_string(_ptr, std::move(value));
    }
    inline void BookFirstOneofVariantBuilder::set_first_oneof_int(int64_t value) {
        _vtable->set_first_oneof_int(_ptr, std::move(value));
    }

    inline bool BookSecondOneofVariantRef::is_second_oneof_string() const {
        return _vtable->is_second_oneof_string(_ptr);
    }
    inline std::string_view BookSecondOneofVariantRef::second_oneof_string() const {
        return _vtable->second_oneof_string(_ptr);
    }
    inline bool BookSecondOneofVariantRef::is_second_oneof_int() const {
        return _vtable->is_second_oneof_int(_ptr);
    }
    inline int64_t BookSecondOneofVariantRef::second_oneof_int() const {
        return _vtable->second_oneof_int(_ptr);
    }
    inline bool BookSecondOneofVariantRef::is_second_oneof_message() const {
        return _vtable->is_second_oneof_message(_ptr);
    }
    inline InsideOneOfMessageEntityRef BookSecondOneofVariantRef::second_oneof_message() const {
        return _vtable->second_oneof_message(_ptr);
    }
    inline bool BookSecondOneofVariantRef::is_null() const {
        return _vtable->is_null(_ptr);
    }

    inline void BookSecondOneofVariantBuilder::set_second_oneof_string(std::string value) {
        _vtable->set_second_oneof_string(_ptr, std::move(value));
    }
    inline void BookSecondOneofVariantBuilder::set_second_oneof_int(int64_t value) {
        _vtable->set_second_oneof_int(_ptr, std::move(value));
    }
    inline void BookSecondOneofVariantBuilder::set_second_oneof_message(::folly::Function<void(InsideOneOfMessageBuilder&)> clbk) {
        _vtable->set_second_oneof_message(_ptr, std::move(clbk));
    }

    inline int64_t BookEntityRef::isbn() const {
        return _vtable->isbn(_ptr);
    }
    inline std::string_view BookEntityRef::title() const {
        return _vtable->title(_ptr);
    }
    inline AuthorEntityRef BookEntityRef::author() const {
        return _vtable->author(_ptr);
    }
    inline ::ranges::any_view<BookPageEntityRef> BookEntityRef::pages() const {
        return _vtable->pages(_ptr);
    }
    inline BookFirstOneofVariantRef BookEntityRef::first_oneof() const {
        return _vtable->first_oneof(_ptr);
    }
    inline BookAvailabilityEnum BookEntityRef::availability() const {
        return _vtable->availability(_ptr);
    }
    inline CanRecommendEnum BookEntityRef::recommended() const {
        return _vtable->recommended(_ptr);
    }
    inline BookSecondOneofVariantRef BookEntityRef::second_oneof() const {
        return _vtable->second_oneof(_ptr);
    }
    BookBuilder BookEntityRef::as_builder() {
        return _vtable->as_builder(_ptr);
    }
    [[nodiscard]] std::string BookEntityRef::serialize() {
        return _vtable->serialize(_ptr);
    }

    inline void BookBuilder::set_isbn(int64_t value) {
        _vtable->set_isbn(_ptr, std::move(value));
    }
    inline void BookBuilder::set_title(std::string value) {
        _vtable->set_title(_ptr, std::move(value));
    }
    template <c_book_author_clbk Fn>
    inline void BookBuilder::set_author(Fn&& fn){
        auto args = _vtable->author_builder_args_factory(_ptr);
        auto builder = create_builder<AuthorBuilder>(std::move(args));
        fn(builder);
    }
    template <c_book_pages_clbk Fn>
    inline void BookBuilder::set_pages(Fn&& fn){
        auto args = _vtable->pages_builder_args_factory(_ptr);
        auto builder = create_builder<::yazik::compiler::support::VecEntityBuilder<BookPageBuilder, BookPageBuilderVtable>>(std::move(args));
        fn(builder);
    }
    template <c_book_first_oneof_clbk Fn>
    inline void BookBuilder::set_first_oneof(Fn&& fn){
        auto args = _vtable->first_oneof_builder_args_factory(_ptr);
        auto builder = create_builder<BookFirstOneofVariantBuilder>(std::move(args));
        fn(builder);
    }
    inline void BookBuilder::set_availability(BookAvailabilityEnum::Value value) {
        _vtable->set_availability(_ptr, std::move(value));
    }
    inline void BookBuilder::set_recommended(CanRecommendEnum::Value value) {
        _vtable->set_recommended(_ptr, std::move(value));
    }
    template <c_book_second_oneof_clbk Fn>
    inline void BookBuilder::set_second_oneof(Fn&& fn){
        auto args = _vtable->second_oneof_builder_args_factory(_ptr);
        auto builder = create_builder<BookSecondOneofVariantBuilder>(std::move(args));
        fn(builder);
    }
    inline void BookBuilder::move_initialize_from(BookEntityRef&& e) 
    {
        return _vtable->move_initialize_from(_ptr, e._ptr);
    }
    inline bool BookBuilder::deserialize(std::string_view data) {
        return _vtable->deserialize(_ptr, std::move(data));
    }
    inline BookEntityRef BookBuilder::as_ref() {
        return _vtable->as_ref(_ptr);
    }

    template<typename Parent>
    inline auto BookLayeredBuilder<Parent>::isbn(int64_t value) -> Self& {
        this->set_isbn(std::move(value));
        return *this;
    }
    template<typename Parent>
    inline auto BookLayeredBuilder<Parent>::title(std::string value) -> Self& {
        this->set_title(std::move(value));
        return *this;
    }
    template<typename Parent>
    inline auto BookLayeredBuilder<Parent>::author() -> AuthorLayeredBuilder<Self> {
        auto args = this->_vtable->author_builder_args_factory(this->_ptr);
        return create_builder_with_parent<AuthorLayeredBuilder<Self>>(std::move(args), this);
    }
    template<typename Parent>
    inline auto BookLayeredBuilder<Parent>::pages() -> ::yazik::compiler::support::VecEntityLayeredBuilder<BookPageLayeredBuilder, BookPageBuilder, Self> {
        auto args = this->_vtable->pages_builder_args_factory(this->_ptr);
        return create_builder_with_parent<::yazik::compiler::support::VecEntityLayeredBuilder<BookPageLayeredBuilder, BookPageBuilder, Self>>(std::move(args), this);
    }
    template<typename Parent>
    inline auto BookLayeredBuilder<Parent>::availability(BookAvailabilityEnum::Value value) -> Self& {
        this->set_availability(std::move(value));
        return *this;
    }
    template<typename Parent>
    inline auto BookLayeredBuilder<Parent>::recommended(CanRecommendEnum::Value value) -> Self& {
        this->set_recommended(std::move(value));
        return *this;
    }
    template<typename Parent>
    auto BookLayeredBuilder<Parent>::move_from(BookEntityRef&& r) -> Self&
    {
        this->move_initialize_from(std::move(r));
        return *this;
    }

    inline int64_t GetBookRequestEntityRef::isbn() const {
        return _vtable->isbn(_ptr);
    }
    GetBookRequestBuilder GetBookRequestEntityRef::as_builder() {
        return _vtable->as_builder(_ptr);
    }
    [[nodiscard]] std::string GetBookRequestEntityRef::serialize() {
        return _vtable->serialize(_ptr);
    }

    inline void GetBookRequestBuilder::set_isbn(int64_t value) {
        _vtable->set_isbn(_ptr, std::move(value));
    }
    inline void GetBookRequestBuilder::move_initialize_from(GetBookRequestEntityRef&& e) 
    {
        return _vtable->move_initialize_from(_ptr, e._ptr);
    }
    inline bool GetBookRequestBuilder::deserialize(std::string_view data) {
        return _vtable->deserialize(_ptr, std::move(data));
    }
    inline GetBookRequestEntityRef GetBookRequestBuilder::as_ref() {
        return _vtable->as_ref(_ptr);
    }

    template<typename Parent>
    inline auto GetBookRequestLayeredBuilder<Parent>::isbn(int64_t value) -> Self& {
        this->set_isbn(std::move(value));
        return *this;
    }
    template<typename Parent>
    auto GetBookRequestLayeredBuilder<Parent>::move_from(GetBookRequestEntityRef&& r) -> Self&
    {
        this->move_initialize_from(std::move(r));
        return *this;
    }

    inline std::string_view GetBookViaAuthorEntityRef::author() const {
        return _vtable->author(_ptr);
    }
    GetBookViaAuthorBuilder GetBookViaAuthorEntityRef::as_builder() {
        return _vtable->as_builder(_ptr);
    }
    [[nodiscard]] std::string GetBookViaAuthorEntityRef::serialize() {
        return _vtable->serialize(_ptr);
    }

    inline void GetBookViaAuthorBuilder::set_author(std::string value) {
        _vtable->set_author(_ptr, std::move(value));
    }
    inline void GetBookViaAuthorBuilder::move_initialize_from(GetBookViaAuthorEntityRef&& e) 
    {
        return _vtable->move_initialize_from(_ptr, e._ptr);
    }
    inline bool GetBookViaAuthorBuilder::deserialize(std::string_view data) {
        return _vtable->deserialize(_ptr, std::move(data));
    }
    inline GetBookViaAuthorEntityRef GetBookViaAuthorBuilder::as_ref() {
        return _vtable->as_ref(_ptr);
    }

    template<typename Parent>
    inline auto GetBookViaAuthorLayeredBuilder<Parent>::author(std::string value) -> Self& {
        this->set_author(std::move(value));
        return *this;
    }
    template<typename Parent>
    auto GetBookViaAuthorLayeredBuilder<Parent>::move_from(GetBookViaAuthorEntityRef&& r) -> Self&
    {
        this->move_initialize_from(std::move(r));
        return *this;
    }

    inline std::string_view BookStoreEntityRef::name() const {
        return _vtable->name(_ptr);
    }
    BookStoreBuilder BookStoreEntityRef::as_builder() {
        return _vtable->as_builder(_ptr);
    }
    [[nodiscard]] std::string BookStoreEntityRef::serialize() {
        return _vtable->serialize(_ptr);
    }

    inline void BookStoreBuilder::set_name(std::string value) {
        _vtable->set_name(_ptr, std::move(value));
    }
    inline void BookStoreBuilder::move_initialize_from(BookStoreEntityRef&& e) 
    {
        return _vtable->move_initialize_from(_ptr, e._ptr);
    }
    inline bool BookStoreBuilder::deserialize(std::string_view data) {
        return _vtable->deserialize(_ptr, std::move(data));
    }
    inline BookStoreEntityRef BookStoreBuilder::as_ref() {
        return _vtable->as_ref(_ptr);
    }

    template<typename Parent>
    inline auto BookStoreLayeredBuilder<Parent>::name(std::string value) -> Self& {
        this->set_name(std::move(value));
        return *this;
    }
    template<typename Parent>
    auto BookStoreLayeredBuilder<Parent>::move_from(BookStoreEntityRef&& r) -> Self&
    {
        this->move_initialize_from(std::move(r));
        return *this;
    }

    inline CycleDep2EntityRef CycleDep1EntityRef::dep() const {
        return _vtable->dep(_ptr);
    }
    CycleDep1Builder CycleDep1EntityRef::as_builder() {
        return _vtable->as_builder(_ptr);
    }
    [[nodiscard]] std::string CycleDep1EntityRef::serialize() {
        return _vtable->serialize(_ptr);
    }

    template <c_cycle_dep1_dep_clbk Fn>
    inline void CycleDep1Builder::set_dep(Fn&& fn){
        auto args = _vtable->dep_builder_args_factory(_ptr);
        auto builder = create_builder<CycleDep2Builder>(std::move(args));
        fn(builder);
    }
    inline void CycleDep1Builder::move_initialize_from(CycleDep1EntityRef&& e) 
    {
        return _vtable->move_initialize_from(_ptr, e._ptr);
    }
    inline bool CycleDep1Builder::deserialize(std::string_view data) {
        return _vtable->deserialize(_ptr, std::move(data));
    }
    inline CycleDep1EntityRef CycleDep1Builder::as_ref() {
        return _vtable->as_ref(_ptr);
    }

    template<typename Parent>
    inline auto CycleDep1LayeredBuilder<Parent>::dep() -> CycleDep2LayeredBuilder<Self> {
        auto args = this->_vtable->dep_builder_args_factory(this->_ptr);
        return create_builder_with_parent<CycleDep2LayeredBuilder<Self>>(std::move(args), this);
    }
    template<typename Parent>
    auto CycleDep1LayeredBuilder<Parent>::move_from(CycleDep1EntityRef&& r) -> Self&
    {
        this->move_initialize_from(std::move(r));
        return *this;
    }

    inline CycleDep1EntityRef CycleDep2EntityRef::dep() const {
        return _vtable->dep(_ptr);
    }
    CycleDep2Builder CycleDep2EntityRef::as_builder() {
        return _vtable->as_builder(_ptr);
    }
    [[nodiscard]] std::string CycleDep2EntityRef::serialize() {
        return _vtable->serialize(_ptr);
    }

    template <c_cycle_dep2_dep_clbk Fn>
    inline void CycleDep2Builder::set_dep(Fn&& fn){
        auto args = _vtable->dep_builder_args_factory(_ptr);
        auto builder = create_builder<CycleDep1Builder>(std::move(args));
        fn(builder);
    }
    inline void CycleDep2Builder::move_initialize_from(CycleDep2EntityRef&& e) 
    {
        return _vtable->move_initialize_from(_ptr, e._ptr);
    }
    inline bool CycleDep2Builder::deserialize(std::string_view data) {
        return _vtable->deserialize(_ptr, std::move(data));
    }
    inline CycleDep2EntityRef CycleDep2Builder::as_ref() {
        return _vtable->as_ref(_ptr);
    }

    template<typename Parent>
    inline auto CycleDep2LayeredBuilder<Parent>::dep() -> CycleDep1LayeredBuilder<Self> {
        auto args = this->_vtable->dep_builder_args_factory(this->_ptr);
        return create_builder_with_parent<CycleDep1LayeredBuilder<Self>>(std::move(args), this);
    }
    template<typename Parent>
    auto CycleDep2LayeredBuilder<Parent>::move_from(CycleDep2EntityRef&& r) -> Self&
    {
        this->move_initialize_from(std::move(r));
        return *this;
    }

}
