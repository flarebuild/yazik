#include <book/sample.yaz.h>

namespace com::book::yaz {
    namespace can_recommend {
        Enum::Enum(int value): _value{value} {}
        bool Enum::is_yep() const noexcept {
            return _value == Yep;
        }
        bool Enum::is_nope() const noexcept {
            return _value == Nope;
        }
        int Enum::which() const noexcept {
            return _value;
        }
    }
    namespace author {
        Ref::Ref(
            void* ptr,
            const RefVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        std::string_view Ref::name() const {
            return _vtable->name(_ptr);
        }
        Builder Ref::as_builder() const {
            return _vtable->as_builder(_ptr);
        }
        std::string Ref::serialize() const {
            return _vtable->serialize(_ptr);
        }
        Builder::Builder(
            void* ptr,
            const BuilderVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        void Builder::set_name (std::string value) {
            _vtable->set_name(_ptr, std::move(value));
        }
        void Builder::move_initialize_from(Ref&& r) {
            _vtable->move_initialize_from(_ptr, r._ptr);
        }
        bool Builder::deserialize(::yazik::string_view data) {
            return _vtable->deserialize(_ptr, data);
        }
        Ref Builder::as_ref() {
            return _vtable->as_ref(_ptr);
        }
    }
    namespace inside_one_of_message {
        Ref::Ref(
            void* ptr,
            const RefVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        std::string_view Ref::field() const {
            return _vtable->field(_ptr);
        }
        Builder Ref::as_builder() const {
            return _vtable->as_builder(_ptr);
        }
        std::string Ref::serialize() const {
            return _vtable->serialize(_ptr);
        }
        Builder::Builder(
            void* ptr,
            const BuilderVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        void Builder::set_field (std::string value) {
            _vtable->set_field(_ptr, std::move(value));
        }
        void Builder::move_initialize_from(Ref&& r) {
            _vtable->move_initialize_from(_ptr, r._ptr);
        }
        bool Builder::deserialize(::yazik::string_view data) {
            return _vtable->deserialize(_ptr, data);
        }
        Ref Builder::as_ref() {
            return _vtable->as_ref(_ptr);
        }
    }
    namespace book {
        namespace page {
            Ref::Ref(
                void* ptr,
                const RefVtable* vtable
            ) noexcept
            : _ptr { ptr }
            , _vtable { vtable }
            {}
            ::yazik::compiler::support::repeated_type_t<std::string_view> Ref::lines() const {
                return _vtable->lines(_ptr);
            }
            Builder Ref::as_builder() const {
                return _vtable->as_builder(_ptr);
            }
            std::string Ref::serialize() const {
                return _vtable->serialize(_ptr);
            }
            Builder::Builder(
                void* ptr,
                const BuilderVtable* vtable
            ) noexcept
            : _ptr { ptr }
            , _vtable { vtable }
            {}
            void Builder::move_initialize_from(Ref&& r) {
                _vtable->move_initialize_from(_ptr, r._ptr);
            }
            bool Builder::deserialize(::yazik::string_view data) {
                return _vtable->deserialize(_ptr, data);
            }
            Ref Builder::as_ref() {
                return _vtable->as_ref(_ptr);
            }
        }
        namespace availability {
            Enum::Enum(int value): _value{value} {}
            bool Enum::is_available() const noexcept {
                return _value == Available;
            }
            bool Enum::is_not_available() const noexcept {
                return _value == NotAvailable;
            }
            int Enum::which() const noexcept {
                return _value;
            }
        }
        namespace first_oneof {
            Ref::Ref(
                void* ptr,
                const RefVtable* vtable
            ) noexcept
            : _ptr { ptr }
            , _vtable { vtable }
            {}
            bool Ref::is_first_oneof_string() const {
                return _vtable->is_first_oneof_string(_ptr);
            }
            std::string_view Ref::first_oneof_string() const {
                return _vtable->first_oneof_string(_ptr);
            }
            bool Ref::is_first_oneof_int() const {
                return _vtable->is_first_oneof_int(_ptr);
            }
            int64_t Ref::first_oneof_int() const {
                return _vtable->first_oneof_int(_ptr);
            }
            bool Ref::is_null() const {
                return _vtable->is_null(_ptr);
            }
            Builder::Builder(
                void* ptr,
                const BuilderVtable* vtable
            ) noexcept
            : _ptr { ptr }
            , _vtable { vtable }
            {}
            void Builder::set_first_oneof_string(std::string value) {
                _vtable->set_first_oneof_string(_ptr, std::move(value));
            }
            void Builder::set_first_oneof_int(int64_t value) {
                _vtable->set_first_oneof_int(_ptr, std::move(value));
            }
        }
        namespace second_oneof {
            Ref::Ref(
                void* ptr,
                const RefVtable* vtable
            ) noexcept
            : _ptr { ptr }
            , _vtable { vtable }
            {}
            bool Ref::is_second_oneof_string() const {
                return _vtable->is_second_oneof_string(_ptr);
            }
            std::string_view Ref::second_oneof_string() const {
                return _vtable->second_oneof_string(_ptr);
            }
            bool Ref::is_second_oneof_int() const {
                return _vtable->is_second_oneof_int(_ptr);
            }
            int64_t Ref::second_oneof_int() const {
                return _vtable->second_oneof_int(_ptr);
            }
            bool Ref::is_second_oneof_message() const {
                return _vtable->is_second_oneof_message(_ptr);
            }
            inside_one_of_message::Ref Ref::second_oneof_message() const {
                return _vtable->second_oneof_message(_ptr);
            }
            bool Ref::is_null() const {
                return _vtable->is_null(_ptr);
            }
            Builder::Builder(
                void* ptr,
                const BuilderVtable* vtable
            ) noexcept
            : _ptr { ptr }
            , _vtable { vtable }
            {}
            void Builder::set_second_oneof_string(std::string value) {
                _vtable->set_second_oneof_string(_ptr, std::move(value));
            }
            void Builder::set_second_oneof_int(int64_t value) {
                _vtable->set_second_oneof_int(_ptr, std::move(value));
            }
            void Builder::set_second_oneof_message(::yazik::unique_function<void (inside_one_of_message::Builder&)> clbk) {
                _vtable->set_second_oneof_message(_ptr, std::move(clbk));
            }
        }
        Ref::Ref(
            void* ptr,
            const RefVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        int64_t Ref::isbn() const {
            return _vtable->isbn(_ptr);
        }
        std::string_view Ref::title() const {
            return _vtable->title(_ptr);
        }
        author::Ref Ref::author() const {
            return _vtable->author(_ptr);
        }
        ::yazik::compiler::support::repeated_type_t<book::page::Ref> Ref::pages() const {
            return _vtable->pages(_ptr);
        }
        book::first_oneof::Ref Ref::first_oneof() const {
            return _vtable->first_oneof(_ptr);
        }
        book::availability::Enum Ref::availability() const {
            return _vtable->availability(_ptr);
        }
        can_recommend::Enum Ref::recommended() const {
            return _vtable->recommended(_ptr);
        }
        book::second_oneof::Ref Ref::second_oneof() const {
            return _vtable->second_oneof(_ptr);
        }
        Builder Ref::as_builder() const {
            return _vtable->as_builder(_ptr);
        }
        std::string Ref::serialize() const {
            return _vtable->serialize(_ptr);
        }
        Builder::Builder(
            void* ptr,
            const BuilderVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        void Builder::set_isbn (int64_t value) {
            _vtable->set_isbn(_ptr, std::move(value));
        }
        void Builder::set_title (std::string value) {
            _vtable->set_title(_ptr, std::move(value));
        }
        void Builder::set_availability (book::availability::Enum value) {
            _vtable->set_availability(_ptr, std::move(value));
        }
        void Builder::set_recommended (can_recommend::Enum value) {
            _vtable->set_recommended(_ptr, std::move(value));
        }
        void Builder::move_initialize_from(Ref&& r) {
            _vtable->move_initialize_from(_ptr, r._ptr);
        }
        bool Builder::deserialize(::yazik::string_view data) {
            return _vtable->deserialize(_ptr, data);
        }
        Ref Builder::as_ref() {
            return _vtable->as_ref(_ptr);
        }
    }
    namespace get_book_request {
        Ref::Ref(
            void* ptr,
            const RefVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        int64_t Ref::isbn() const {
            return _vtable->isbn(_ptr);
        }
        Builder Ref::as_builder() const {
            return _vtable->as_builder(_ptr);
        }
        std::string Ref::serialize() const {
            return _vtable->serialize(_ptr);
        }
        Builder::Builder(
            void* ptr,
            const BuilderVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        void Builder::set_isbn (int64_t value) {
            _vtable->set_isbn(_ptr, std::move(value));
        }
        void Builder::move_initialize_from(Ref&& r) {
            _vtable->move_initialize_from(_ptr, r._ptr);
        }
        bool Builder::deserialize(::yazik::string_view data) {
            return _vtable->deserialize(_ptr, data);
        }
        Ref Builder::as_ref() {
            return _vtable->as_ref(_ptr);
        }
    }
    namespace get_book_via_author {
        Ref::Ref(
            void* ptr,
            const RefVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        std::string_view Ref::author() const {
            return _vtable->author(_ptr);
        }
        Builder Ref::as_builder() const {
            return _vtable->as_builder(_ptr);
        }
        std::string Ref::serialize() const {
            return _vtable->serialize(_ptr);
        }
        Builder::Builder(
            void* ptr,
            const BuilderVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        void Builder::set_author (std::string value) {
            _vtable->set_author(_ptr, std::move(value));
        }
        void Builder::move_initialize_from(Ref&& r) {
            _vtable->move_initialize_from(_ptr, r._ptr);
        }
        bool Builder::deserialize(::yazik::string_view data) {
            return _vtable->deserialize(_ptr, data);
        }
        Ref Builder::as_ref() {
            return _vtable->as_ref(_ptr);
        }
    }
    namespace book_store {
        Ref::Ref(
            void* ptr,
            const RefVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        std::string_view Ref::name() const {
            return _vtable->name(_ptr);
        }
        Builder Ref::as_builder() const {
            return _vtable->as_builder(_ptr);
        }
        std::string Ref::serialize() const {
            return _vtable->serialize(_ptr);
        }
        Builder::Builder(
            void* ptr,
            const BuilderVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        void Builder::set_name (std::string value) {
            _vtable->set_name(_ptr, std::move(value));
        }
        void Builder::move_initialize_from(Ref&& r) {
            _vtable->move_initialize_from(_ptr, r._ptr);
        }
        bool Builder::deserialize(::yazik::string_view data) {
            return _vtable->deserialize(_ptr, data);
        }
        Ref Builder::as_ref() {
            return _vtable->as_ref(_ptr);
        }
    }
    namespace cycle_dep1 {
        Ref::Ref(
            void* ptr,
            const RefVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        cycle_dep2::Ref Ref::dep() const {
            return _vtable->dep(_ptr);
        }
        Builder Ref::as_builder() const {
            return _vtable->as_builder(_ptr);
        }
        std::string Ref::serialize() const {
            return _vtable->serialize(_ptr);
        }
        Builder::Builder(
            void* ptr,
            const BuilderVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        void Builder::move_initialize_from(Ref&& r) {
            _vtable->move_initialize_from(_ptr, r._ptr);
        }
        bool Builder::deserialize(::yazik::string_view data) {
            return _vtable->deserialize(_ptr, data);
        }
        Ref Builder::as_ref() {
            return _vtable->as_ref(_ptr);
        }
    }
    namespace cycle_dep2 {
        Ref::Ref(
            void* ptr,
            const RefVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        cycle_dep1::Ref Ref::dep() const {
            return _vtable->dep(_ptr);
        }
        Builder Ref::as_builder() const {
            return _vtable->as_builder(_ptr);
        }
        std::string Ref::serialize() const {
            return _vtable->serialize(_ptr);
        }
        Builder::Builder(
            void* ptr,
            const BuilderVtable* vtable
        ) noexcept
        : _ptr { ptr }
        , _vtable { vtable }
        {}
        void Builder::move_initialize_from(Ref&& r) {
            _vtable->move_initialize_from(_ptr, r._ptr);
        }
        bool Builder::deserialize(::yazik::string_view data) {
            return _vtable->deserialize(_ptr, data);
        }
        Ref Builder::as_ref() {
            return _vtable->as_ref(_ptr);
        }
    }
}
