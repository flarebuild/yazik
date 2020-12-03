#pragma once

#include <yazik/compiler/support/support.hpp>

namespace com::book::yaz {
    namespace can_recommend {
        class Enum;
    }
    namespace author {
        struct RefVtable;
        class Ref;
        struct BuilderVtable;
        class Builder;
        template <typename Parent>
        class LayeredBuilder;
    }
    namespace inside_one_of_message {
        struct RefVtable;
        class Ref;
        struct BuilderVtable;
        class Builder;
        template <typename Parent>
        class LayeredBuilder;
    }
    namespace book {
        namespace page {
            struct RefVtable;
            class Ref;
            struct BuilderVtable;
            class Builder;
            template <typename Parent>
            class LayeredBuilder;
        }
        namespace availability {
            class Enum;
        }
        namespace first_oneof {
            struct RefVtable;
            class Ref;
            struct BuilderVtable;
            class Builder;
        }
        namespace second_oneof {
            struct RefVtable;
            class Ref;
            struct BuilderVtable;
            class Builder;
        }
        struct RefVtable;
        class Ref;
        struct BuilderVtable;
        class Builder;
        template <typename Parent>
        class LayeredBuilder;
    }
    namespace get_book_request {
        struct RefVtable;
        class Ref;
        struct BuilderVtable;
        class Builder;
        template <typename Parent>
        class LayeredBuilder;
    }
    namespace get_book_via_author {
        struct RefVtable;
        class Ref;
        struct BuilderVtable;
        class Builder;
        template <typename Parent>
        class LayeredBuilder;
    }
    namespace book_store {
        struct RefVtable;
        class Ref;
        struct BuilderVtable;
        class Builder;
        template <typename Parent>
        class LayeredBuilder;
    }
    namespace cycle_dep1 {
        struct RefVtable;
        class Ref;
        struct BuilderVtable;
        class Builder;
        template <typename Parent>
        class LayeredBuilder;
    }
    namespace cycle_dep2 {
        struct RefVtable;
        class Ref;
        struct BuilderVtable;
        class Builder;
        template <typename Parent>
        class LayeredBuilder;
    }
    namespace can_recommend {
        class Enum {
            int _value;
        public:
            Enum(int value);
            enum Value {
                Yep = 0,
                Nope = 3,
            };
            [[nodiscard]] int which() const noexcept;
            [[nodiscard]] bool is_yep() const noexcept;
            [[nodiscard]] bool is_nope() const noexcept;
        };
    }
    namespace author {
        struct RefVtable {
            using name_fn = std::string_view (*)(const void*);
            name_fn name;
            using as_builder_fn = Builder (*)(void*);
            as_builder_fn as_builder;
            using serialize_fn = std::string (*)(void*);
            serialize_fn serialize;
        };
        class Ref {
            void* _ptr;
            const RefVtable* _vtable;
            Ref(
                void* ptr,
                const RefVtable* vtable
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            using vtable_t = RefVtable;
            [[nodiscard]] std::string_view name() const;
            [[nodiscard]] Builder as_builder() const;
            [[nodiscard]] std::string serialize() const;
        };
        struct BuilderVtable {
            using set_name_fn = void (*)(const void*, std::string);
            set_name_fn set_name;
            using move_initialize_from_fn = void (*)(void*, void*);
            move_initialize_from_fn move_initialize_from;
            using deserialize_fn = bool (*)(const void*, std::string_view);
            deserialize_fn deserialize;
            using as_ref_fn = Ref (*)(void*);
            as_ref_fn as_ref;
        };
        class Builder {
        protected:
            void* _ptr;
            const BuilderVtable* _vtable;
            Builder(
                void* ptr,
                const BuilderVtable* vtable
            ) noexcept;
            friend class ::yazik::compiler::support::Initializer;
        public:
            template <typename Parent>
            using layered_t = LayeredBuilder<Parent>;
            using vtable_t = BuilderVtable;
            void set_name(std::string);
            void move_initialize_from(Ref&&);
            [[nodiscard]] bool deserialize(::yazik::string_view);
            [[nodiscard]] Ref as_ref();
            template <typename Parent>
            [[nodiscard]] LayeredBuilder<Parent> layered(Parent*);
        };
        template <typename Parent>
        class LayeredBuilder final
        : public Builder
        , virtual protected ::yazik::compiler::support::Initializer {
        public:
            using Base = Builder;
            using base_builder_t = Builder;
            using parent_t = Parent;
        private:
            using Self = LayeredBuilder<Parent>;
            Parent* _parent;
            LayeredBuilder(
                void* ptr,
                const BuilderVtable* vtable,
                Parent* parent
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            Self& name(std::string);
            Self& move_from(author::Ref&&);
            Parent done();
        };
    }
    namespace inside_one_of_message {
        struct RefVtable {
            using field_fn = std::string_view (*)(const void*);
            field_fn field;
            using as_builder_fn = Builder (*)(void*);
            as_builder_fn as_builder;
            using serialize_fn = std::string (*)(void*);
            serialize_fn serialize;
        };
        class Ref {
            void* _ptr;
            const RefVtable* _vtable;
            Ref(
                void* ptr,
                const RefVtable* vtable
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            using vtable_t = RefVtable;
            [[nodiscard]] std::string_view field() const;
            [[nodiscard]] Builder as_builder() const;
            [[nodiscard]] std::string serialize() const;
        };
        struct BuilderVtable {
            using set_field_fn = void (*)(const void*, std::string);
            set_field_fn set_field;
            using move_initialize_from_fn = void (*)(void*, void*);
            move_initialize_from_fn move_initialize_from;
            using deserialize_fn = bool (*)(const void*, std::string_view);
            deserialize_fn deserialize;
            using as_ref_fn = Ref (*)(void*);
            as_ref_fn as_ref;
        };
        class Builder {
        protected:
            void* _ptr;
            const BuilderVtable* _vtable;
            Builder(
                void* ptr,
                const BuilderVtable* vtable
            ) noexcept;
            friend class ::yazik::compiler::support::Initializer;
        public:
            template <typename Parent>
            using layered_t = LayeredBuilder<Parent>;
            using vtable_t = BuilderVtable;
            void set_field(std::string);
            void move_initialize_from(Ref&&);
            [[nodiscard]] bool deserialize(::yazik::string_view);
            [[nodiscard]] Ref as_ref();
            template <typename Parent>
            [[nodiscard]] LayeredBuilder<Parent> layered(Parent*);
        };
        template <typename Parent>
        class LayeredBuilder final
        : public Builder
        , virtual protected ::yazik::compiler::support::Initializer {
        public:
            using Base = Builder;
            using base_builder_t = Builder;
            using parent_t = Parent;
        private:
            using Self = LayeredBuilder<Parent>;
            Parent* _parent;
            LayeredBuilder(
                void* ptr,
                const BuilderVtable* vtable,
                Parent* parent
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            Self& field(std::string);
            Self& move_from(inside_one_of_message::Ref&&);
            Parent done();
        };
    }
    namespace book {
        namespace page {
            struct RefVtable {
                using lines_fn = ::yazik::compiler::support::repeated_type_t<std::string_view> (*)(const void*);
                lines_fn lines;
                using as_builder_fn = Builder (*)(void*);
                as_builder_fn as_builder;
                using serialize_fn = std::string (*)(void*);
                serialize_fn serialize;
            };
            class Ref {
                void* _ptr;
                const RefVtable* _vtable;
                Ref(
                    void* ptr,
                    const RefVtable* vtable
                ) noexcept;
                friend class Builder;
                friend class ::yazik::compiler::support::Initializer;
            public:
                using vtable_t = RefVtable;
                [[nodiscard]] ::yazik::compiler::support::repeated_type_t<std::string_view> lines() const;
                [[nodiscard]] Builder as_builder() const;
                [[nodiscard]] std::string serialize() const;
            };
            struct BuilderVtable {
                using construct_lines_builder_fn = ::yazik::compiler::support::VecBuilder<std::string> (*)(const void*);
                construct_lines_builder_fn construct_lines_builder;
                using move_initialize_from_fn = void (*)(void*, void*);
                move_initialize_from_fn move_initialize_from;
                using deserialize_fn = bool (*)(const void*, std::string_view);
                deserialize_fn deserialize;
                using as_ref_fn = Ref (*)(void*);
                as_ref_fn as_ref;
            };
            template <typename Fn>
            concept s_page_lines_clbk = requires(
                Fn&& fn,
                ::yazik::compiler::support::VecBuilder<std::string>& builder
            ) {
                { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
            };
            class Builder {
            protected:
                void* _ptr;
                const BuilderVtable* _vtable;
                Builder(
                    void* ptr,
                    const BuilderVtable* vtable
                ) noexcept;
                friend class ::yazik::compiler::support::Initializer;
            public:
                template <typename Parent>
                using layered_t = LayeredBuilder<Parent>;
                using vtable_t = BuilderVtable;
                template <s_page_lines_clbk Fn>
                inline void set_lines(Fn&&);
                void move_initialize_from(Ref&&);
                [[nodiscard]] bool deserialize(::yazik::string_view);
                [[nodiscard]] Ref as_ref();
                template <typename Parent>
                [[nodiscard]] LayeredBuilder<Parent> layered(Parent*);
            };
            template <typename Parent>
            class LayeredBuilder final
            : public Builder
            , virtual protected ::yazik::compiler::support::Initializer {
            public:
                using Base = Builder;
                using base_builder_t = Builder;
                using parent_t = Parent;
            private:
                using Self = LayeredBuilder<Parent>;
                Parent* _parent;
                LayeredBuilder(
                    void* ptr,
                    const BuilderVtable* vtable,
                    Parent* parent
                ) noexcept;
                friend class Builder;
                friend class ::yazik::compiler::support::Initializer;
            public:
                ::yazik::compiler::support::VecLayeredBuilder<std::string, Self> lines();
                Self& move_from(book::page::Ref&&);
                Parent done();
            };
        }
        namespace availability {
            class Enum {
                int _value;
            public:
                Enum(int value);
                enum Value {
                    Available = 0,
                    NotAvailable = 1,
                };
                [[nodiscard]] int which() const noexcept;
                [[nodiscard]] bool is_available() const noexcept;
                [[nodiscard]] bool is_not_available() const noexcept;
            };
        }
        namespace first_oneof {
            struct RefVtable {
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
            class Ref {
                void* _ptr;
                const RefVtable* _vtable;
                Ref(
                    void* ptr,
                    const RefVtable* vtable
                ) noexcept;
                friend class ::yazik::compiler::support::Initializer;
            public:
                using vtable_t = RefVtable;
                [[nodiscard]] bool is_first_oneof_string() const;
                [[nodiscard]] std::string_view first_oneof_string() const;
                [[nodiscard]] bool is_first_oneof_int() const;
                [[nodiscard]] int64_t first_oneof_int() const;
                [[nodiscard]] bool is_null() const;
            };
            struct BuilderVtable {
                using set_first_oneof_string_fn = void (*)(void*, std::string);
                set_first_oneof_string_fn set_first_oneof_string;
                using set_first_oneof_int_fn = void (*)(void*, int64_t);
                set_first_oneof_int_fn set_first_oneof_int;
                using is_null_fn = bool (*)(const void*);
                is_null_fn is_null;
            };
            class Builder {
                void* _ptr;
                const BuilderVtable* _vtable;
                Builder(
                    void* ptr,
                    const BuilderVtable* vtable
                ) noexcept;
                friend class ::yazik::compiler::support::Initializer;
            public:
                using vtable_t = BuilderVtable;
                void set_first_oneof_string(std::string);
                void set_first_oneof_int(int64_t);
            };
        }
        namespace second_oneof {
            struct RefVtable {
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
                using second_oneof_message_fn = inside_one_of_message::Ref (*)(const void*);
                second_oneof_message_fn second_oneof_message;
                using is_null_fn = bool (*)(const void*);
                is_null_fn is_null;
            };
            class Ref {
                void* _ptr;
                const RefVtable* _vtable;
                Ref(
                    void* ptr,
                    const RefVtable* vtable
                ) noexcept;
                friend class ::yazik::compiler::support::Initializer;
            public:
                using vtable_t = RefVtable;
                [[nodiscard]] bool is_second_oneof_string() const;
                [[nodiscard]] std::string_view second_oneof_string() const;
                [[nodiscard]] bool is_second_oneof_int() const;
                [[nodiscard]] int64_t second_oneof_int() const;
                [[nodiscard]] bool is_second_oneof_message() const;
                [[nodiscard]] inside_one_of_message::Ref second_oneof_message() const;
                [[nodiscard]] bool is_null() const;
            };
            struct BuilderVtable {
                using set_second_oneof_string_fn = void (*)(void*, std::string);
                set_second_oneof_string_fn set_second_oneof_string;
                using set_second_oneof_int_fn = void (*)(void*, int64_t);
                set_second_oneof_int_fn set_second_oneof_int;
                using set_second_oneof_message_fn = void (*)(void*, ::yazik::unique_function<void (inside_one_of_message::Builder&)>);
                set_second_oneof_message_fn set_second_oneof_message;
                using is_null_fn = bool (*)(const void*);
                is_null_fn is_null;
            };
            class Builder {
                void* _ptr;
                const BuilderVtable* _vtable;
                Builder(
                    void* ptr,
                    const BuilderVtable* vtable
                ) noexcept;
                friend class ::yazik::compiler::support::Initializer;
            public:
                using vtable_t = BuilderVtable;
                void set_second_oneof_string(std::string);
                void set_second_oneof_int(int64_t);
                void set_second_oneof_message(::yazik::unique_function<void (inside_one_of_message::Builder&)>);
            };
        }
        struct RefVtable {
            using isbn_fn = int64_t (*)(const void*);
            isbn_fn isbn;
            using title_fn = std::string_view (*)(const void*);
            title_fn title;
            using author_fn = author::Ref (*)(const void*);
            author_fn author;
            using pages_fn = ::yazik::compiler::support::repeated_type_t<book::page::Ref> (*)(const void*);
            pages_fn pages;
            using first_oneof_fn = book::first_oneof::Ref (*)(const void*);
            first_oneof_fn first_oneof;
            using availability_fn = book::availability::Enum (*)(const void*);
            availability_fn availability;
            using recommended_fn = can_recommend::Enum (*)(const void*);
            recommended_fn recommended;
            using second_oneof_fn = book::second_oneof::Ref (*)(const void*);
            second_oneof_fn second_oneof;
            using as_builder_fn = Builder (*)(void*);
            as_builder_fn as_builder;
            using serialize_fn = std::string (*)(void*);
            serialize_fn serialize;
        };
        class Ref {
            void* _ptr;
            const RefVtable* _vtable;
            Ref(
                void* ptr,
                const RefVtable* vtable
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            using vtable_t = RefVtable;
            [[nodiscard]] int64_t isbn() const;
            [[nodiscard]] std::string_view title() const;
            [[nodiscard]] author::Ref author() const;
            [[nodiscard]] ::yazik::compiler::support::repeated_type_t<book::page::Ref> pages() const;
            [[nodiscard]] book::first_oneof::Ref first_oneof() const;
            [[nodiscard]] book::availability::Enum availability() const;
            [[nodiscard]] can_recommend::Enum recommended() const;
            [[nodiscard]] book::second_oneof::Ref second_oneof() const;
            [[nodiscard]] Builder as_builder() const;
            [[nodiscard]] std::string serialize() const;
        };
        struct BuilderVtable {
            using set_isbn_fn = void (*)(const void*, int64_t);
            set_isbn_fn set_isbn;
            using set_title_fn = void (*)(const void*, std::string);
            set_title_fn set_title;
            using construct_author_builder_fn = author::Builder (*)(const void*);
            construct_author_builder_fn construct_author_builder;
            using construct_pages_builder_fn = ::yazik::compiler::support::VecEntityBuilder<book::page::Builder> (*)(const void*);
            construct_pages_builder_fn construct_pages_builder;
            using construct_first_oneof_builder_fn = book::first_oneof::Builder (*)(const void*);
            construct_first_oneof_builder_fn construct_first_oneof_builder;
            using set_availability_fn = void (*)(const void*, book::availability::Enum);
            set_availability_fn set_availability;
            using set_recommended_fn = void (*)(const void*, can_recommend::Enum);
            set_recommended_fn set_recommended;
            using construct_second_oneof_builder_fn = book::second_oneof::Builder (*)(const void*);
            construct_second_oneof_builder_fn construct_second_oneof_builder;
            using move_initialize_from_fn = void (*)(void*, void*);
            move_initialize_from_fn move_initialize_from;
            using deserialize_fn = bool (*)(const void*, std::string_view);
            deserialize_fn deserialize;
            using as_ref_fn = Ref (*)(void*);
            as_ref_fn as_ref;
        };
        template <typename Fn>
        concept s_book_author_clbk = requires(
            Fn&& fn,
            author::Builder& builder
        ) {
            { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
        };
        template <typename Fn>
        concept s_book_pages_clbk = requires(
            Fn&& fn,
            ::yazik::compiler::support::VecEntityBuilder<book::page::Builder>& builder
        ) {
            { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
        };
        template <typename Fn>
        concept s_book_first_oneof_clbk = requires(
            Fn&& fn,
            book::first_oneof::Builder& builder
        ) {
            { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
        };
        template <typename Fn>
        concept s_book_second_oneof_clbk = requires(
            Fn&& fn,
            book::second_oneof::Builder& builder
        ) {
            { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
        };
        class Builder {
        protected:
            void* _ptr;
            const BuilderVtable* _vtable;
            Builder(
                void* ptr,
                const BuilderVtable* vtable
            ) noexcept;
            friend class ::yazik::compiler::support::Initializer;
        public:
            template <typename Parent>
            using layered_t = LayeredBuilder<Parent>;
            using vtable_t = BuilderVtable;
            void set_isbn(int64_t);
            void set_title(std::string);
            template <s_book_author_clbk Fn>
            inline void set_author(Fn&&);
            template <s_book_pages_clbk Fn>
            inline void set_pages(Fn&&);
            template <s_book_first_oneof_clbk Fn>
            inline void set_first_oneof(Fn&&);
            void set_availability(book::availability::Enum);
            void set_recommended(can_recommend::Enum);
            template <s_book_second_oneof_clbk Fn>
            inline void set_second_oneof(Fn&&);
            void move_initialize_from(Ref&&);
            [[nodiscard]] bool deserialize(::yazik::string_view);
            [[nodiscard]] Ref as_ref();
            template <typename Parent>
            [[nodiscard]] LayeredBuilder<Parent> layered(Parent*);
        };
        template <typename Parent>
        class LayeredBuilder final
        : public Builder
        , virtual protected ::yazik::compiler::support::Initializer {
        public:
            using Base = Builder;
            using base_builder_t = Builder;
            using parent_t = Parent;
        private:
            using Self = LayeredBuilder<Parent>;
            Parent* _parent;
            LayeredBuilder(
                void* ptr,
                const BuilderVtable* vtable,
                Parent* parent
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            Self& isbn(int64_t);
            Self& title(std::string);
            author::LayeredBuilder<Self> author();
            ::yazik::compiler::support::VecEntityLayeredBuilder<book::page::LayeredBuilder, Self> pages();
            Self& availability(book::availability::Enum);
            Self& recommended(can_recommend::Enum);
            Self& move_from(book::Ref&&);
            Parent done();
        };
    }
    namespace get_book_request {
        struct RefVtable {
            using isbn_fn = int64_t (*)(const void*);
            isbn_fn isbn;
            using as_builder_fn = Builder (*)(void*);
            as_builder_fn as_builder;
            using serialize_fn = std::string (*)(void*);
            serialize_fn serialize;
        };
        class Ref {
            void* _ptr;
            const RefVtable* _vtable;
            Ref(
                void* ptr,
                const RefVtable* vtable
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            using vtable_t = RefVtable;
            [[nodiscard]] int64_t isbn() const;
            [[nodiscard]] Builder as_builder() const;
            [[nodiscard]] std::string serialize() const;
        };
        struct BuilderVtable {
            using set_isbn_fn = void (*)(const void*, int64_t);
            set_isbn_fn set_isbn;
            using move_initialize_from_fn = void (*)(void*, void*);
            move_initialize_from_fn move_initialize_from;
            using deserialize_fn = bool (*)(const void*, std::string_view);
            deserialize_fn deserialize;
            using as_ref_fn = Ref (*)(void*);
            as_ref_fn as_ref;
        };
        class Builder {
        protected:
            void* _ptr;
            const BuilderVtable* _vtable;
            Builder(
                void* ptr,
                const BuilderVtable* vtable
            ) noexcept;
            friend class ::yazik::compiler::support::Initializer;
        public:
            template <typename Parent>
            using layered_t = LayeredBuilder<Parent>;
            using vtable_t = BuilderVtable;
            void set_isbn(int64_t);
            void move_initialize_from(Ref&&);
            [[nodiscard]] bool deserialize(::yazik::string_view);
            [[nodiscard]] Ref as_ref();
            template <typename Parent>
            [[nodiscard]] LayeredBuilder<Parent> layered(Parent*);
        };
        template <typename Parent>
        class LayeredBuilder final
        : public Builder
        , virtual protected ::yazik::compiler::support::Initializer {
        public:
            using Base = Builder;
            using base_builder_t = Builder;
            using parent_t = Parent;
        private:
            using Self = LayeredBuilder<Parent>;
            Parent* _parent;
            LayeredBuilder(
                void* ptr,
                const BuilderVtable* vtable,
                Parent* parent
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            Self& isbn(int64_t);
            Self& move_from(get_book_request::Ref&&);
            Parent done();
        };
    }
    namespace get_book_via_author {
        struct RefVtable {
            using author_fn = std::string_view (*)(const void*);
            author_fn author;
            using as_builder_fn = Builder (*)(void*);
            as_builder_fn as_builder;
            using serialize_fn = std::string (*)(void*);
            serialize_fn serialize;
        };
        class Ref {
            void* _ptr;
            const RefVtable* _vtable;
            Ref(
                void* ptr,
                const RefVtable* vtable
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            using vtable_t = RefVtable;
            [[nodiscard]] std::string_view author() const;
            [[nodiscard]] Builder as_builder() const;
            [[nodiscard]] std::string serialize() const;
        };
        struct BuilderVtable {
            using set_author_fn = void (*)(const void*, std::string);
            set_author_fn set_author;
            using move_initialize_from_fn = void (*)(void*, void*);
            move_initialize_from_fn move_initialize_from;
            using deserialize_fn = bool (*)(const void*, std::string_view);
            deserialize_fn deserialize;
            using as_ref_fn = Ref (*)(void*);
            as_ref_fn as_ref;
        };
        class Builder {
        protected:
            void* _ptr;
            const BuilderVtable* _vtable;
            Builder(
                void* ptr,
                const BuilderVtable* vtable
            ) noexcept;
            friend class ::yazik::compiler::support::Initializer;
        public:
            template <typename Parent>
            using layered_t = LayeredBuilder<Parent>;
            using vtable_t = BuilderVtable;
            void set_author(std::string);
            void move_initialize_from(Ref&&);
            [[nodiscard]] bool deserialize(::yazik::string_view);
            [[nodiscard]] Ref as_ref();
            template <typename Parent>
            [[nodiscard]] LayeredBuilder<Parent> layered(Parent*);
        };
        template <typename Parent>
        class LayeredBuilder final
        : public Builder
        , virtual protected ::yazik::compiler::support::Initializer {
        public:
            using Base = Builder;
            using base_builder_t = Builder;
            using parent_t = Parent;
        private:
            using Self = LayeredBuilder<Parent>;
            Parent* _parent;
            LayeredBuilder(
                void* ptr,
                const BuilderVtable* vtable,
                Parent* parent
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            Self& author(std::string);
            Self& move_from(get_book_via_author::Ref&&);
            Parent done();
        };
    }
    namespace book_store {
        struct RefVtable {
            using name_fn = std::string_view (*)(const void*);
            name_fn name;
            using as_builder_fn = Builder (*)(void*);
            as_builder_fn as_builder;
            using serialize_fn = std::string (*)(void*);
            serialize_fn serialize;
        };
        class Ref {
            void* _ptr;
            const RefVtable* _vtable;
            Ref(
                void* ptr,
                const RefVtable* vtable
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            using vtable_t = RefVtable;
            [[nodiscard]] std::string_view name() const;
            [[nodiscard]] Builder as_builder() const;
            [[nodiscard]] std::string serialize() const;
        };
        struct BuilderVtable {
            using set_name_fn = void (*)(const void*, std::string);
            set_name_fn set_name;
            using move_initialize_from_fn = void (*)(void*, void*);
            move_initialize_from_fn move_initialize_from;
            using deserialize_fn = bool (*)(const void*, std::string_view);
            deserialize_fn deserialize;
            using as_ref_fn = Ref (*)(void*);
            as_ref_fn as_ref;
        };
        class Builder {
        protected:
            void* _ptr;
            const BuilderVtable* _vtable;
            Builder(
                void* ptr,
                const BuilderVtable* vtable
            ) noexcept;
            friend class ::yazik::compiler::support::Initializer;
        public:
            template <typename Parent>
            using layered_t = LayeredBuilder<Parent>;
            using vtable_t = BuilderVtable;
            void set_name(std::string);
            void move_initialize_from(Ref&&);
            [[nodiscard]] bool deserialize(::yazik::string_view);
            [[nodiscard]] Ref as_ref();
            template <typename Parent>
            [[nodiscard]] LayeredBuilder<Parent> layered(Parent*);
        };
        template <typename Parent>
        class LayeredBuilder final
        : public Builder
        , virtual protected ::yazik::compiler::support::Initializer {
        public:
            using Base = Builder;
            using base_builder_t = Builder;
            using parent_t = Parent;
        private:
            using Self = LayeredBuilder<Parent>;
            Parent* _parent;
            LayeredBuilder(
                void* ptr,
                const BuilderVtable* vtable,
                Parent* parent
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            Self& name(std::string);
            Self& move_from(book_store::Ref&&);
            Parent done();
        };
    }
    namespace cycle_dep1 {
        struct RefVtable {
            using dep_fn = cycle_dep2::Ref (*)(const void*);
            dep_fn dep;
            using as_builder_fn = Builder (*)(void*);
            as_builder_fn as_builder;
            using serialize_fn = std::string (*)(void*);
            serialize_fn serialize;
        };
        class Ref {
            void* _ptr;
            const RefVtable* _vtable;
            Ref(
                void* ptr,
                const RefVtable* vtable
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            using vtable_t = RefVtable;
            [[nodiscard]] cycle_dep2::Ref dep() const;
            [[nodiscard]] Builder as_builder() const;
            [[nodiscard]] std::string serialize() const;
        };
        struct BuilderVtable {
            using construct_dep_builder_fn = cycle_dep2::Builder (*)(const void*);
            construct_dep_builder_fn construct_dep_builder;
            using move_initialize_from_fn = void (*)(void*, void*);
            move_initialize_from_fn move_initialize_from;
            using deserialize_fn = bool (*)(const void*, std::string_view);
            deserialize_fn deserialize;
            using as_ref_fn = Ref (*)(void*);
            as_ref_fn as_ref;
        };
        template <typename Fn>
        concept s_cycle_dep1_dep_clbk = requires(
            Fn&& fn,
            cycle_dep2::Builder& builder
        ) {
            { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
        };
        class Builder {
        protected:
            void* _ptr;
            const BuilderVtable* _vtable;
            Builder(
                void* ptr,
                const BuilderVtable* vtable
            ) noexcept;
            friend class ::yazik::compiler::support::Initializer;
        public:
            template <typename Parent>
            using layered_t = LayeredBuilder<Parent>;
            using vtable_t = BuilderVtable;
            template <s_cycle_dep1_dep_clbk Fn>
            inline void set_dep(Fn&&);
            void move_initialize_from(Ref&&);
            [[nodiscard]] bool deserialize(::yazik::string_view);
            [[nodiscard]] Ref as_ref();
            template <typename Parent>
            [[nodiscard]] LayeredBuilder<Parent> layered(Parent*);
        };
        template <typename Parent>
        class LayeredBuilder final
        : public Builder
        , virtual protected ::yazik::compiler::support::Initializer {
        public:
            using Base = Builder;
            using base_builder_t = Builder;
            using parent_t = Parent;
        private:
            using Self = LayeredBuilder<Parent>;
            Parent* _parent;
            LayeredBuilder(
                void* ptr,
                const BuilderVtable* vtable,
                Parent* parent
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            cycle_dep2::LayeredBuilder<Self> dep();
            Self& move_from(cycle_dep1::Ref&&);
            Parent done();
        };
    }
    namespace cycle_dep2 {
        struct RefVtable {
            using dep_fn = cycle_dep1::Ref (*)(const void*);
            dep_fn dep;
            using as_builder_fn = Builder (*)(void*);
            as_builder_fn as_builder;
            using serialize_fn = std::string (*)(void*);
            serialize_fn serialize;
        };
        class Ref {
            void* _ptr;
            const RefVtable* _vtable;
            Ref(
                void* ptr,
                const RefVtable* vtable
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            using vtable_t = RefVtable;
            [[nodiscard]] cycle_dep1::Ref dep() const;
            [[nodiscard]] Builder as_builder() const;
            [[nodiscard]] std::string serialize() const;
        };
        struct BuilderVtable {
            using construct_dep_builder_fn = cycle_dep1::Builder (*)(const void*);
            construct_dep_builder_fn construct_dep_builder;
            using move_initialize_from_fn = void (*)(void*, void*);
            move_initialize_from_fn move_initialize_from;
            using deserialize_fn = bool (*)(const void*, std::string_view);
            deserialize_fn deserialize;
            using as_ref_fn = Ref (*)(void*);
            as_ref_fn as_ref;
        };
        template <typename Fn>
        concept s_cycle_dep2_dep_clbk = requires(
            Fn&& fn,
            cycle_dep1::Builder& builder
        ) {
            { fn(builder) } -> ::yazik::concepts::c_same_as<void>;
        };
        class Builder {
        protected:
            void* _ptr;
            const BuilderVtable* _vtable;
            Builder(
                void* ptr,
                const BuilderVtable* vtable
            ) noexcept;
            friend class ::yazik::compiler::support::Initializer;
        public:
            template <typename Parent>
            using layered_t = LayeredBuilder<Parent>;
            using vtable_t = BuilderVtable;
            template <s_cycle_dep2_dep_clbk Fn>
            inline void set_dep(Fn&&);
            void move_initialize_from(Ref&&);
            [[nodiscard]] bool deserialize(::yazik::string_view);
            [[nodiscard]] Ref as_ref();
            template <typename Parent>
            [[nodiscard]] LayeredBuilder<Parent> layered(Parent*);
        };
        template <typename Parent>
        class LayeredBuilder final
        : public Builder
        , virtual protected ::yazik::compiler::support::Initializer {
        public:
            using Base = Builder;
            using base_builder_t = Builder;
            using parent_t = Parent;
        private:
            using Self = LayeredBuilder<Parent>;
            Parent* _parent;
            LayeredBuilder(
                void* ptr,
                const BuilderVtable* vtable,
                Parent* parent
            ) noexcept;
            friend class Builder;
            friend class ::yazik::compiler::support::Initializer;
        public:
            cycle_dep1::LayeredBuilder<Self> dep();
            Self& move_from(cycle_dep2::Ref&&);
            Parent done();
        };
    }
    namespace author {
        template <typename Parent>
        [[nodiscard]] LayeredBuilder<Parent> Builder::layered(Parent* parent) {
            return { _ptr, _vtable, parent };
        }
        template <typename Parent>
        LayeredBuilder<Parent>::LayeredBuilder(
            void* ptr,
            const BuilderVtable* vtable,
            Parent* parent
        ) noexcept
        : Base { ptr, vtable }
        , _parent { parent }
        {}
        template <typename Parent>
        auto LayeredBuilder<Parent>::name(std::string value) -> Self& {
            this->set_name(std::move(value));
            return *this;
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::move_from(author::Ref&& r) -> Self& {
            this->move_initialize_from(std::move(r));
            return *this;
        }
        template <typename Parent>
        Parent LayeredBuilder<Parent>::done() {
            return std::move(*this->_parent);
        }
    }
    namespace inside_one_of_message {
        template <typename Parent>
        [[nodiscard]] LayeredBuilder<Parent> Builder::layered(Parent* parent) {
            return { _ptr, _vtable, parent };
        }
        template <typename Parent>
        LayeredBuilder<Parent>::LayeredBuilder(
            void* ptr,
            const BuilderVtable* vtable,
            Parent* parent
        ) noexcept
        : Base { ptr, vtable }
        , _parent { parent }
        {}
        template <typename Parent>
        auto LayeredBuilder<Parent>::field(std::string value) -> Self& {
            this->set_field(std::move(value));
            return *this;
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::move_from(inside_one_of_message::Ref&& r) -> Self& {
            this->move_initialize_from(std::move(r));
            return *this;
        }
        template <typename Parent>
        Parent LayeredBuilder<Parent>::done() {
            return std::move(*this->_parent);
        }
    }
    namespace book {
        namespace page {
            template <s_page_lines_clbk Fn>
            inline void Builder::set_lines(Fn&& fn) {
                auto builder = _vtable->construct_lines_builder(_ptr);
                fn(builder);
            }
            template <typename Parent>
            [[nodiscard]] LayeredBuilder<Parent> Builder::layered(Parent* parent) {
                return { _ptr, _vtable, parent };
            }
            template <typename Parent>
            LayeredBuilder<Parent>::LayeredBuilder(
                void* ptr,
                const BuilderVtable* vtable,
                Parent* parent
            ) noexcept
            : Base { ptr, vtable }
            , _parent { parent }
            {}
            template <typename Parent>
            auto LayeredBuilder<Parent>::lines() -> ::yazik::compiler::support::VecLayeredBuilder<std::string, Self> {
                return _vtable->construct_lines_builder(_ptr).layered(this);
            }
            template <typename Parent>
            auto LayeredBuilder<Parent>::move_from(book::page::Ref&& r) -> Self& {
                this->move_initialize_from(std::move(r));
                return *this;
            }
            template <typename Parent>
            Parent LayeredBuilder<Parent>::done() {
                return std::move(*this->_parent);
            }
        }
        template <s_book_author_clbk Fn>
        inline void Builder::set_author(Fn&& fn) {
            auto builder = _vtable->construct_author_builder(_ptr);
            fn(builder);
        }
        template <s_book_pages_clbk Fn>
        inline void Builder::set_pages(Fn&& fn) {
            auto builder = _vtable->construct_pages_builder(_ptr);
            fn(builder);
        }
        template <s_book_first_oneof_clbk Fn>
        inline void Builder::set_first_oneof(Fn&& fn) {
            auto builder = _vtable->construct_first_oneof_builder(_ptr);
            fn(builder);
        }
        template <s_book_second_oneof_clbk Fn>
        inline void Builder::set_second_oneof(Fn&& fn) {
            auto builder = _vtable->construct_second_oneof_builder(_ptr);
            fn(builder);
        }
        template <typename Parent>
        [[nodiscard]] LayeredBuilder<Parent> Builder::layered(Parent* parent) {
            return { _ptr, _vtable, parent };
        }
        template <typename Parent>
        LayeredBuilder<Parent>::LayeredBuilder(
            void* ptr,
            const BuilderVtable* vtable,
            Parent* parent
        ) noexcept
        : Base { ptr, vtable }
        , _parent { parent }
        {}
        template <typename Parent>
        auto LayeredBuilder<Parent>::isbn(int64_t value) -> Self& {
            this->set_isbn(std::move(value));
            return *this;
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::title(std::string value) -> Self& {
            this->set_title(std::move(value));
            return *this;
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::author() -> author::LayeredBuilder<Self> {
            return _vtable->construct_author_builder(_ptr).layered(this);
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::pages() -> ::yazik::compiler::support::VecEntityLayeredBuilder<book::page::LayeredBuilder, Self> {
            return _vtable->construct_pages_builder(_ptr).template layered<book::page::LayeredBuilder>(this);
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::availability(book::availability::Enum value) -> Self& {
            this->set_availability(std::move(value));
            return *this;
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::recommended(can_recommend::Enum value) -> Self& {
            this->set_recommended(std::move(value));
            return *this;
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::move_from(book::Ref&& r) -> Self& {
            this->move_initialize_from(std::move(r));
            return *this;
        }
        template <typename Parent>
        Parent LayeredBuilder<Parent>::done() {
            return std::move(*this->_parent);
        }
    }
    namespace get_book_request {
        template <typename Parent>
        [[nodiscard]] LayeredBuilder<Parent> Builder::layered(Parent* parent) {
            return { _ptr, _vtable, parent };
        }
        template <typename Parent>
        LayeredBuilder<Parent>::LayeredBuilder(
            void* ptr,
            const BuilderVtable* vtable,
            Parent* parent
        ) noexcept
        : Base { ptr, vtable }
        , _parent { parent }
        {}
        template <typename Parent>
        auto LayeredBuilder<Parent>::isbn(int64_t value) -> Self& {
            this->set_isbn(std::move(value));
            return *this;
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::move_from(get_book_request::Ref&& r) -> Self& {
            this->move_initialize_from(std::move(r));
            return *this;
        }
        template <typename Parent>
        Parent LayeredBuilder<Parent>::done() {
            return std::move(*this->_parent);
        }
    }
    namespace get_book_via_author {
        template <typename Parent>
        [[nodiscard]] LayeredBuilder<Parent> Builder::layered(Parent* parent) {
            return { _ptr, _vtable, parent };
        }
        template <typename Parent>
        LayeredBuilder<Parent>::LayeredBuilder(
            void* ptr,
            const BuilderVtable* vtable,
            Parent* parent
        ) noexcept
        : Base { ptr, vtable }
        , _parent { parent }
        {}
        template <typename Parent>
        auto LayeredBuilder<Parent>::author(std::string value) -> Self& {
            this->set_author(std::move(value));
            return *this;
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::move_from(get_book_via_author::Ref&& r) -> Self& {
            this->move_initialize_from(std::move(r));
            return *this;
        }
        template <typename Parent>
        Parent LayeredBuilder<Parent>::done() {
            return std::move(*this->_parent);
        }
    }
    namespace book_store {
        template <typename Parent>
        [[nodiscard]] LayeredBuilder<Parent> Builder::layered(Parent* parent) {
            return { _ptr, _vtable, parent };
        }
        template <typename Parent>
        LayeredBuilder<Parent>::LayeredBuilder(
            void* ptr,
            const BuilderVtable* vtable,
            Parent* parent
        ) noexcept
        : Base { ptr, vtable }
        , _parent { parent }
        {}
        template <typename Parent>
        auto LayeredBuilder<Parent>::name(std::string value) -> Self& {
            this->set_name(std::move(value));
            return *this;
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::move_from(book_store::Ref&& r) -> Self& {
            this->move_initialize_from(std::move(r));
            return *this;
        }
        template <typename Parent>
        Parent LayeredBuilder<Parent>::done() {
            return std::move(*this->_parent);
        }
    }
    namespace cycle_dep1 {
        template <s_cycle_dep1_dep_clbk Fn>
        inline void Builder::set_dep(Fn&& fn) {
            auto builder = _vtable->construct_dep_builder(_ptr);
            fn(builder);
        }
        template <typename Parent>
        [[nodiscard]] LayeredBuilder<Parent> Builder::layered(Parent* parent) {
            return { _ptr, _vtable, parent };
        }
        template <typename Parent>
        LayeredBuilder<Parent>::LayeredBuilder(
            void* ptr,
            const BuilderVtable* vtable,
            Parent* parent
        ) noexcept
        : Base { ptr, vtable }
        , _parent { parent }
        {}
        template <typename Parent>
        auto LayeredBuilder<Parent>::dep() -> cycle_dep2::LayeredBuilder<Self> {
            return _vtable->construct_dep_builder(_ptr).layered(this);
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::move_from(cycle_dep1::Ref&& r) -> Self& {
            this->move_initialize_from(std::move(r));
            return *this;
        }
        template <typename Parent>
        Parent LayeredBuilder<Parent>::done() {
            return std::move(*this->_parent);
        }
    }
    namespace cycle_dep2 {
        template <s_cycle_dep2_dep_clbk Fn>
        inline void Builder::set_dep(Fn&& fn) {
            auto builder = _vtable->construct_dep_builder(_ptr);
            fn(builder);
        }
        template <typename Parent>
        [[nodiscard]] LayeredBuilder<Parent> Builder::layered(Parent* parent) {
            return { _ptr, _vtable, parent };
        }
        template <typename Parent>
        LayeredBuilder<Parent>::LayeredBuilder(
            void* ptr,
            const BuilderVtable* vtable,
            Parent* parent
        ) noexcept
        : Base { ptr, vtable }
        , _parent { parent }
        {}
        template <typename Parent>
        auto LayeredBuilder<Parent>::dep() -> cycle_dep1::LayeredBuilder<Self> {
            return _vtable->construct_dep_builder(_ptr).layered(this);
        }
        template <typename Parent>
        auto LayeredBuilder<Parent>::move_from(cycle_dep2::Ref&& r) -> Self& {
            this->move_initialize_from(std::move(r));
            return *this;
        }
        template <typename Parent>
        Parent LayeredBuilder<Parent>::done() {
            return std::move(*this->_parent);
        }
    }
}
