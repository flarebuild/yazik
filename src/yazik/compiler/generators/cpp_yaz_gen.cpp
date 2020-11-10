#include "cpp_yaz_gen.hpp"

#include <google/protobuf/descriptor.pb.h>
#include <yazik/compiler/utility/writer.hpp>
#include <yazik/compiler/formatters/yaz_formatter.hpp>

namespace yazik::compiler {

    struct Gen {
        const gp::FileDescriptor* file;
        FileWriter w;
        Formatter f;

        Gen(const gp::FileDescriptor* file, gpc::GeneratorContext* generator_context)
        : file { file }
        , w{ file, generator_context, "yaz.h"}
        , f{ file, "" }
        {}

        void enum_decl(const gp::EnumDescriptor* enum_d);
        void enum_desc(const gp::EnumDescriptor* enum_d);

        void oneof_decl(const gp::OneofDescriptor* one_of);

        void oneof_desc(const gp::OneofDescriptor* one_of);
        void oneof_desc_vtable(const gp::OneofDescriptor* one_of);
        void oneof_desc_ref(const gp::OneofDescriptor* one_of);
        void oneof_desc_builder_vtable(const gp::OneofDescriptor* one_of);
        void oneof_desc_builder(const gp::OneofDescriptor* one_of);

        void oneof_impl(const gp::OneofDescriptor* one_of);
        void oneof_impl_ref(const gp::OneofDescriptor* one_of);
        void oneof_impl_builder(const gp::OneofDescriptor* one_of);

        void message_decl(const gp::Descriptor* message);

        void message_desc(const gp::Descriptor* message);
        void message_desc_nested(const gp::Descriptor* message);
        void message_desc_vtable(const gp::Descriptor* message);
        void message_desc_ref(const gp::Descriptor* message);
        void message_desc_builder_vtable(const gp::Descriptor* message);
        void message_desc_builder(const gp::Descriptor* message);
        void message_desc_layered_builder(const gp::Descriptor* message);

        void message_impl(const gp::Descriptor* message);
        void message_impl_nested(const gp::Descriptor* message);
        void message_impl_ref(const gp::Descriptor* message);
        void message_impl_builder(const gp::Descriptor* message);
        void message_impl_layered_builder(const gp::Descriptor* message);

        void exec() {
            w.wl("#pragma once");
            w.l();
            w.wl("#include {}", ab("yazik/compiler/support/support.hpp"));
            for (int d = 0; d < file->dependency_count(); ++d) {
                auto* dep = file->dependency(d);
                if (dep->syntax() != gp::FileDescriptor::SYNTAX_PROTO3)
                    continue;

                auto dep_name = replace_proto(dep->name(), "yaz.h");
                w.wl("#include {}", ab(dep_name));
            }
            w.l();
            auto body = [&] {
                for (int e = 0; e < file->enum_type_count(); ++e)
                    enum_decl(file->enum_type(e));
                for (int m = 0; m < file->message_type_count(); ++m)
                    message_decl(file->message_type(m));
                w.l();
                for (int e = 0; e < file->enum_type_count(); ++e)
                    enum_desc(file->enum_type(e));
                for (int m = 0; m < file->message_type_count(); ++m)
                    message_desc(file->message_type(m));
                for (int m = 0; m < file->message_type_count(); ++m)
                    message_impl(file->message_type(m));

            };
            if (file->package().empty()) body();
            else w.w("namespace {} ", to_cpp_ns(file->package())).braced(body);
        }

    };

    Result<bool> generate_yaz(
        const gp::FileDescriptor* file,
        const std::string& parameter,
        gpc::GeneratorContext* generator_context
    ) noexcept {
        Gen { file, generator_context }.exec();
        co_return true;
    }

    void Gen::enum_decl(const gp::EnumDescriptor* enum_d) {
        std::string tname = f.type_name(enum_d);
        w.wl("class {}Enum;",tname);
    }

    void Gen::enum_desc(const gp::EnumDescriptor *enum_d) {
        std::string tname = f.type_name(enum_d);
        w.w("class {}Enum ",tname)
            .braced_detail([&] {
                w.wl("int _value;");
                w.wl("{}Enum(int value) noexcept: _value{{ value }} {{}}", tname);
                w.l();
                w.wl("friend class ::yazik::compiler::support::Initializer;");
                w.l();
                w.wl_outdent("public:");
                w.l();
                w.w("enum Value ")
                    .braced_detail([&] {
                        for (int v = 0; v < enum_d->value_count(); ++v) {
                            auto* value = enum_d->value(v);
                            w.wl(
                                "{} = {}{}",
                                value->name(),
                                value->number(),
                                v == enum_d->value_count() - 1
                                ? ""
                                : ","
                            );
                        }
                    }, false)
                    .wl(";");
                w.l();
                w.w("[[nodiscard]] inline int which() const ")
                    .braced([&]{
                        w.wl("return _value;");
                    });
                for (int v = 0; v < enum_d->value_count(); ++v) {
                    auto* value = enum_d->value(v);
                    w.w("[[nodiscard]] inline bool is_{}() const ", snake_case(value->name()))
                        .braced([&]{
                            w.wl("return _value == {};", value->name());
                        });
                }
            }, false)
            .wl(";").l();
    }

    void Gen::oneof_decl(const gp::OneofDescriptor* one_of) {
        std::string tname = f.type_name(one_of);
        w.wl("struct {}VariantRef;",tname);
        w.wl("class {}VariantBuilder;",tname);

        // TODO: impl
//        w.wl("template<typename Parent>");
//        w.wl("class {}LayeredVariantBuilder;",tname);
    }

    void Gen::oneof_desc(const gp::OneofDescriptor* one_of) {
        oneof_desc_vtable(one_of);
        oneof_desc_ref(one_of);
        oneof_desc_builder_vtable(one_of);
        oneof_desc_builder(one_of);
        // TODO: impl layered
    }

    void Gen::oneof_desc_vtable(const gp::OneofDescriptor* one_of) {
        std::string tname = f.type_name(one_of);

        w.w("struct {}Vtable ",tname)
            .braced_detail([&] {
                for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
                    auto *field = one_of->field(f_idx);
                    if (!is_supported(field))
                        continue;

                    std::string getter = f.getter_inner(field);
                    std::string type = f.resolve_type(field);
                    w.wl("using is_{}_fn = bool (*)(const void*);", getter);
                    w.wl("is_{0}_fn is_{0};", getter);
                    w.wl("using {}_fn = {} (*)(const void*);", getter, type);
                    w.wl("{0}_fn {0};", getter);
                }
                w.wl("using is_null_fn = bool (*)(const void*);");
                w.wl("is_null_fn is_null;");
            }, false)
            .wl(";");
        w.l();
    }

    void Gen::oneof_desc_ref(const gp::OneofDescriptor* one_of) {
        std::string tname = f.type_name(one_of);
        w.w("class {}VariantRef ",tname)
            .braced_detail([&] {
                w.wl("const void* _ptr;");
                w.wl("const {}Vtable* _vtable;", tname);
                w.l();
                w.wl("{0}VariantRef(const void* ptr, const {0}Vtable* vtable) noexcept", tname);
                w.wl(": _ptr {{ ptr }}");
                w.wl(", _vtable {{ vtable }}");
                w.wl("{{}}");
                w.l();
                w.wl("friend class ::yazik::compiler::support::Initializer;");
                w.l();
                w.wl_outdent("public:");
                w.l();
                w.wl("using Vtable = {}Vtable;", tname);
                w.l();
                for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
                    auto *field = one_of->field(f_idx);
                    if (!is_supported(field))
                        continue;
                    std::string getter = f.getter_inner(field);
                    std::string type = f.resolve_type(field);

                    w.wl("[[nodiscard]] bool is_{}() const;", getter);
                    w.wl("[[nodiscard]] {} {}() const;", type, getter);
                }
                w.w("[[nodiscard]] bool is_null() const;");
            }, false)
            .wl(";");
        w.l();
    }

    void Gen::oneof_desc_builder_vtable(const gp::OneofDescriptor* one_of) {
        std::string tname = f.type_name(one_of);
        w.w("struct {}BuilderVtable ",tname)
            .braced_detail([&] {
                for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
                    auto *field = one_of->field(f_idx);
                    if (!is_supported(field))
                        continue;

                    std::string getter = f.getter_inner(field);
                    std::string type = f.resolve_builder_inner_type(field);

                    if (field->is_repeated() || field->message_type()) {
                        w.wl("using set_{}_fn = void (*)(void*, ::folly::Function<void({}&)>);", getter, type);
                    } else {
                        w.wl("using set_{}_fn = void (*)(void*, {});", getter, type);
                    }
                    w.wl("set_{0}_fn set_{0};", getter);
                }
            }, false)
            .wl(";");
        w.l();
    }

    void Gen::oneof_desc_builder(const gp::OneofDescriptor* one_of) {
        std::string tname = f.type_name(one_of);
        w.w("class {}VariantBuilder ",tname)
            .class_braced([&] {
                w.wl("void* _ptr;");
                w.wl("const {}BuilderVtable* _vtable;", tname);
                w.l();
                w.wl("{0}VariantBuilder(void* ptr, const {0}BuilderVtable* vtable) noexcept", tname);
                w.wl(": _ptr {{ ptr }}");
                w.wl(", _vtable {{ vtable }}");
                w.wl("{{}}");
                w.l();
                w.wl("friend class ::yazik::compiler::support::Initializer;");
                w.l();
                w.wl_outdent("public:");
                w.l();
                w.wl("using Vtable = {}BuilderVtable;", tname);
                w.l();
                for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
                    auto *field = one_of->field(f_idx);
                    if (!is_supported(field))
                        continue;
                    std::string getter = f.getter_inner(field);
                    std::string type = f.resolve_builder_inner_type(field);

                    if (field->is_repeated() || field->message_type()) {
                        w.wl("void set_{}(::folly::Function<void({}&)>);", getter, type);
                    } else {
                        w.wl("void set_{}({});", getter, type);
                    }
                }
            });
        w.l();
    }

    void Gen::oneof_impl(const gp::OneofDescriptor* one_of) {
        oneof_impl_ref(one_of);
        oneof_impl_builder(one_of);
        // TODO: impl layered
    }

    void Gen::oneof_impl_ref(const gp::OneofDescriptor* one_of) {
        std::string tname = f.type_name(one_of);
        for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
            auto *field = one_of->field(f_idx);
            if (!is_supported(field))
                continue;

            std::string getter = f.getter_inner(field);
            std::string type = f.resolve_type(field);

            w.w("inline bool {}VariantRef::is_{}() const ", tname, getter)
                .braced([&] {
                    w.wl("return _vtable->is_{}(_ptr);", getter);
                });

            w.w("inline {} {}VariantRef::{}() const ", type, tname, getter)
                .braced([&] {
                    w.wl("return _vtable->{}(_ptr);", getter);
                });
        }
        w.w("inline bool {}VariantRef::is_null() const ", tname)
            .braced([&] {
                w.wl("return _vtable->is_null(_ptr);");
            });
        w.l();
    }

    void Gen::oneof_impl_builder(const gp::OneofDescriptor* one_of) {
        std::string tname = f.type_name(one_of);
        for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
            auto *field = one_of->field(f_idx);
            if (!is_supported(field))
                continue;

            std::string getter = f.getter_inner(field);
            std::string type = f.resolve_builder_inner_type(field);
            if (field->is_repeated() || field->message_type()) {
                w.w("inline void {}VariantBuilder::set_{}(::folly::Function<void({}&)> clbk) ", tname, getter, type)
                    .braced([&] {
                        w.wl("_vtable->set_{}(_ptr, std::move(clbk));", getter);
                    });
            } else {
                w.w("inline void {}VariantBuilder::set_{}({} value) ", tname, getter, type)
                    .braced([&] {
                        w.wl("_vtable->set_{}(_ptr, std::move(value));", getter);
                    });
            }
        }
        w.l();
    }

    void Gen::message_decl(const gp::Descriptor* message) {
        for (int n = 0; n < message->nested_type_count(); ++n) {
            auto *nested = message->nested_type(n);
            if (!is_supported(nested))
                continue;

            message_decl(nested);
        }

        for (int e = 0; e < message->enum_type_count(); ++e)
            enum_decl(message->enum_type(e));

        for (int o = 0; o < message->oneof_decl_count(); ++o)
            oneof_decl(message->oneof_decl(o));

        std::string tname = f.type_name(message);
        w.wl("struct {}EntityRef;", tname);
        w.wl("class {}BuilderVtable;", tname);
        w.wl("class {}Builder;", tname);
        w.wl("template <typename Parent>");
        w.wl("class {}LayeredBuilder;", tname);
    }

    void Gen::message_desc(const gp::Descriptor* message) {
        message_desc_nested(message);
        message_desc_vtable(message);
        message_desc_ref(message);
        message_desc_builder_vtable(message);
        message_desc_builder(message);
        message_desc_layered_builder(message);
    }

    void Gen::message_desc_nested(const gp::Descriptor* message) {
        // Process nested messages
        for (int n = 0; n < message->nested_type_count(); ++n) {
            auto *nested = message->nested_type(n);
            if (!is_supported(nested))
                continue;

            message_desc(nested);
        }

        // Process nested enums
        for (int e = 0; e < message->enum_type_count(); ++e)
            enum_desc(message->enum_type(e));

        // Process oneofs
        for (int o = 0; o < message->oneof_decl_count(); ++o)
            oneof_desc(message->oneof_decl(o));
    }

    void Gen::message_desc_vtable(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);

        w.w("struct {}Vtable ",tname)
            .class_braced([&] {
                for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
                    auto *field = message->field(f_idx);

                    if (auto one_of = field->containing_oneof()) {
                        if (one_of->field(0) != field)
                            continue;

                        std::string type = f.type_name(one_of);
                        std::string getter = f.getter(field);
                        w.wl("using {}_fn = {}VariantRef (*)(const void*);", getter, type);
                        w.wl("{0}_fn {0};", getter);
                        continue;
                    }

                    if (!is_supported(field))
                        continue;

                    std::string getter = f.getter(field);
                    std::string type = f.resolve_type(field);
                    w.wl("using {}_fn = {} (*)(const void*);", getter, type);
                    w.wl("{0}_fn {0};", getter);
                }
                w.wl("using as_builder_fn = {}Builder (*)(void*);", tname);
                w.wl("as_builder_fn as_builder;");
                w.wl("using serialize_fn = std::string (*)(void*);");
                w.wl("serialize_fn serialize;");
            });
        w.l();
    }

    void Gen::message_desc_ref(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);

        w.w("class {}EntityRef ", tname)
            .class_braced([&] {
                w.wl("void* _ptr;");
                w.wl("const {}Vtable* _vtable;", tname);
                w.l();
                w.wl("{0}EntityRef(void* ptr, const {0}Vtable* vtable) noexcept", tname);
                w.wl(": _ptr {{ ptr }}");
                w.wl(", _vtable {{ vtable }}");
                w.wl("{{}}");
                w.l();
                w.wl("friend class {}Builder;", tname);
                w.wl("friend class ::yazik::compiler::support::Initializer;");
                w.l();
                w.wl_outdent("public:");
                w.l();
                w.wl("using Vtable = {}Vtable;", tname);
                w.l();
                for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
                    auto *field = message->field(f_idx);

                    if (auto one_of = field->containing_oneof()) {
                        if (one_of->field(0) != field)
                            continue;

                        std::string type = f.type_name(one_of);
                        std::string getter = f.getter(field);

                        w.wl("[[nodiscard]] {}VariantRef {}() const;", type, getter);
                        continue;
                    }

                    if (!is_supported(field))
                        continue;

                    std::string getter = f.getter(field);
                    std::string type = f.resolve_type(field);
                    w.wl("[[nodiscard]] {} {}() const;", type, getter);
                }
                w.wl("[[nodiscard]] {}Builder as_builder();", tname);
                w.wl("[[nodiscard]] std::string serialize();");
            });
        w.l();
    }

    void Gen::message_desc_builder_vtable(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);
        w.w("struct {}BuilderVtable ", tname)
            .class_braced([&]{
                for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
                    auto *field = message->field(f_idx);

                    if (need_pass(field))
                        continue;

                    std::string getter = f.getter(field);

                    if (is_factory_based(field)) {
                        w.wl("using {}_args_tuple_t = {};", getter, f.resolve_builder_args_tuple(field));
                        w.wl("using {0}_builder_args_factory_fn = {0}_args_tuple_t (*)(void*);",  getter);
                        w.wl("{0}_builder_args_factory_fn {0}_builder_args_factory;", getter);
                    } else {
                        std::string type = f.resolve_builder_type(field);
                        w.wl("using set_{}_fn = void (*)(const void*, {});", getter, type);
                        w.wl("set_{0}_fn set_{0};", getter);
                    }
                }
                w.wl("using move_initialize_from_fn = void (*)(void*, void*);");
                w.wl("move_initialize_from_fn move_initialize_from;");
                w.wl("using deserialize_fn = bool (*)(const void*, std::string_view);");
                w.wl("deserialize_fn deserialize;");
                w.wl("using as_ref_fn = {0}EntityRef (*)(void*);", tname);
                w.wl("as_ref_fn as_ref;");
            });
        w.l();
    }

    void Gen::message_desc_builder(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);

        for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
            auto *field = message->field(f_idx);

            if (need_pass(field) || !is_factory_based(field))
                continue;

            std::string getter = f.getter(field);
            std::string type = f.resolve_builder_type(field);

            w.wl("template <typename Fn>");
            w.w("concept {} = requires(Fn&& fn, {}& builder)", f.clbk_concept_name(field), type)
                .class_braced([&]{
                    w.w_raw("{ fn(builder) } -> ::yazik::concepts::c_same_as<void>;").l();
                });
            w.l();
        }

        w.w("class {}Builder : virtual protected ::yazik::compiler::support::Initializer ", tname)
            .braced_detail([&]{
                w.wl_outdent("protected:");
                w.wl("void* _ptr;");
                w.wl("const {}BuilderVtable* _vtable;", tname);
                w.l();
                w.wl("{0}Builder(void* ptr, const {0}BuilderVtable* vtable) noexcept", tname);
                w.wl(": _ptr {{ ptr }}");
                w.wl(", _vtable {{ vtable }}");
                w.wl("{{}}");
                w.l();
                w.wl("friend class ::yazik::compiler::support::Initializer;");
                w.l();
                w.wl_outdent("public:");
                w.l();
                w.wl("using Vtable = {}BuilderVtable;", tname);
                w.l();
                for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
                    auto *field = message->field(f_idx);

                    if (need_pass(field))
                        continue;

                    std::string getter = f.getter(field);
                    std::string type = f.resolve_builder_type(field);

                    if (is_factory_based(field)) {
                        w.wl("template <{} Fn>", f.clbk_concept_name(field));
                        w.wl("void set_{}(Fn&&);", getter);
                    } else {
                        w.wl("void set_{}({});", getter, type);
                    }
                }
                w.wl("void move_initialize_from({}EntityRef&&);", tname);
                w.wl("[[nodiscard]] bool deserialize(std::string_view);");
                w.wl("[[nodiscard]] {}EntityRef as_ref();", tname);
            }, false)
            .wl(";");
        w.l();
    }

    void Gen::message_desc_layered_builder(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);
        w.wl("template <typename Parent>");
        w.wl("class {}LayeredBuilder final", tname);
        w.wl(": public {}Builder", tname);
        w.w(", virtual protected ::yazik::compiler::support::Initializer ")
            .class_braced([&]{
                w.wl_outdent("public:");
                w.l();
                w.wl("using Base = {}Builder;", tname);
                w.l();
                w.wl_outdent("private:");
                w.l();
                w.wl("using Self = {}LayeredBuilder<Parent>;", tname);
                w.wl("Parent _parent;");
                w.l();
                w.wl("{0}LayeredBuilder(void* ptr, const {0}BuilderVtable* vtable, Parent* parent)", tname);
                w.wl(": Base {{ ptr, vtable }}");
                w.wl(", _parent {{ std::move(*parent) }}");
                w.wl("{{}}");
                w.l();
                w.wl("friend class ::yazik::compiler::support::Initializer;");
                w.l();
                w.wl_outdent("public:");
                w.l();
                for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
                    auto *field = message->field(f_idx);

                    if (need_pass(field))
                        continue;

                    if (auto one_of = field->containing_oneof()) {
                        // TODO: impl
                        continue;
                    }

                    std::string getter = f.getter(field);
                    std::string type = f.resolve_builder_type(field);

                    if (field->is_repeated()) {
                        if (field->message_type()) {
                            w.wl(
                                "::yazik::compiler::support::VecEntityLayeredBuilder<{0}LayeredBuilder, {0}Builder, Self> {1}();",
                                f.type_name(field->message_type()),
                                getter
                            );
                        } else {
                            w.wl(
                                "::yazik::compiler::support::VecLayeredBuilder<{}, Self> {}();",
                                f.resolve_builder_inner_type(field),
                                getter
                            );
                        }
                    } else if (field->message_type()) {
                        std::string mname = f.type_name(field->message_type());
                        w.wl("{}LayeredBuilder<Self> {}();", mname, getter);
                    } else {
                        w.wl("Self& {}({});", getter, type);
                    }
                }
                w.l();
                w.wl("Self& move_from({}EntityRef&&);", tname);
                w.l();
                w.w("inline Parent done() ")
                    .braced([&] {
                        w.wl("return std::move(this->_parent);");
                    });
                w.l();
            });
    }

    void Gen::message_impl(const gp::Descriptor* message) {
        message_impl_nested(message);
        message_impl_ref(message);
        message_impl_builder(message);
        message_impl_layered_builder(message);
    }

    void Gen::message_impl_nested(const gp::Descriptor* message) {
        // Process nested messages
        for (int n = 0; n < message->nested_type_count(); ++n) {
            auto *nested = message->nested_type(n);
            if (!is_supported(nested))
                continue;

            message_impl(nested);
        }

        // Process oneofs
        for (int o = 0; o < message->oneof_decl_count(); ++o)
            oneof_impl(message->oneof_decl(o));
    }

    void Gen::message_impl_ref(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);

        for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
            auto *field = message->field(f_idx);

            if (auto one_of = field->containing_oneof()) {
                if (one_of->field(0) != field)
                    continue;

                std::string type = f.type_name(one_of);
                std::string getter = f.getter(field);

                w.w("inline {}VariantRef {}EntityRef::{}() const ", type, tname, getter)
                    .braced([&] {
                        w.wl("return _vtable->{}(_ptr);", getter);
                    });
                continue;
            }

            if (!is_supported(field))
                continue;

            std::string getter = f.getter(field);
            std::string type = f.resolve_type(field);
            w.w("inline {} {}EntityRef::{}() const ", type, tname, getter)
                .braced([&] {
                    w.wl("return _vtable->{}(_ptr);", getter);
                });
        }
        w.w("inline {0}Builder {0}EntityRef::as_builder() ", tname)
            .braced([&] {
                w.wl("return _vtable->as_builder(_ptr);");
            });
        w.w("inline std::string {}EntityRef::serialize() ", tname)
            .braced([&] {
                w.wl("return _vtable->serialize(_ptr);");
            });
        w.l();
    }

    void Gen::message_impl_builder(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);

        for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
            auto *field = message->field(f_idx);

            if (need_pass(field))
                continue;

            std::string getter = f.getter(field);
            std::string type = f.resolve_builder_type(field);

            if (is_factory_based(field)) {
                w.wl("template <{} Fn>", f.clbk_concept_name(field), getter);
                w.w("inline void {}Builder::set_{}(Fn&& fn)", tname, getter)
                    .braced([&] {
                        w.wl("auto args = _vtable->{}_builder_args_factory(_ptr);", getter);
                        w.wl("auto builder = create_builder<{}>(std::move(args));", type);
                        w.wl("fn(builder);");
                    });
            } else {
                w.w("inline void {0}Builder::set_{1}({2} value) ", tname, getter, type)
                    .braced([&] {
                        w.wl("_vtable->set_{}(_ptr, std::move(value));", getter);
                    });
            }
        }
        w.wl("inline void {0}Builder::move_initialize_from({0}EntityRef&& e) ", tname)
            .braced([&] {
                w.wl("return _vtable->move_initialize_from(_ptr, e._ptr);");
            });

        w.w("inline bool {}Builder::deserialize(std::string_view data) ", tname)
            .braced([&] {
                w.wl("return _vtable->deserialize(_ptr, std::move(data));");
            });

        w.w("inline {0}EntityRef {0}Builder::as_ref() ", tname)
            .braced([&] {
                w.wl("return _vtable->as_ref(_ptr);");
            });
        w.l();
    }

    void Gen::message_impl_layered_builder(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);
        for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
            auto *field = message->field(f_idx);

            if (need_pass(field))
                continue;

            if (auto one_of = field->containing_oneof()) {
                // TODO: impl
                continue;
            }

            std::string getter = f.getter(field);
            std::string type = f.resolve_builder_type(field);

            if (field->is_repeated()) {
                if (field->message_type()) {
                    std::string mname = f.type_name(field->message_type());
                    std::string layered_type = do_sformat(
                        "::yazik::compiler::support::VecEntityLayeredBuilder<{0}LayeredBuilder, {0}Builder, Self>",
                        mname
                    );
                    w.wl("template<typename Parent>");
                    w.w("inline auto {}LayeredBuilder<Parent>::{}() -> {} ", tname, getter, layered_type)
                        .braced([&] {
                            w.wl("auto args = this->_vtable->{}_builder_args_factory(this->_ptr);", getter);
                            w.wl("return create_builder_with_parent<{}>(std::move(args), this);", layered_type);
                        });
                } else {
                    std::string layered_type = do_sformat(
                        "::yazik::compiler::support::VecLayeredBuilder<{}, Self>",
                        f.resolve_builder_inner_type(field)
                    );
                    w.wl("template<typename Parent>");
                    w.w("inline auto {}LayeredBuilder<Parent>::{}() -> {} ", tname, getter, layered_type)
                        .braced([&] {
                            w.wl("auto args = this->_vtable->{}_builder_args_factory(this->_ptr);", getter);
                            w.wl("return create_builder_with_parent<{}>(std::move(args), this);", layered_type);
                        });
                }
            } else if (field->message_type()) {
                std::string mname = f.type_name(field->message_type());
                w.wl("template<typename Parent>");
                w.w("inline auto {}LayeredBuilder<Parent>::{}() -> {}LayeredBuilder<Self> ", tname, getter, mname)
                    .braced([&]{
                        w.wl("auto args = this->_vtable->{}_builder_args_factory(this->_ptr);", getter);
                        w.wl("return create_builder_with_parent<{}LayeredBuilder<Self>>(std::move(args), this);", mname);
                    });
            } else {
                w.wl("template<typename Parent>");
                w.w("inline auto {}LayeredBuilder<Parent>::{}({} value) -> Self& ", tname, getter, type)
                    .braced([&]{
                        w.wl("this->set_{}(std::move(value));", getter);
                        w.wl("return *this;");
                    });
            }
        }
        w.wl("template<typename Parent>");
        w.wl("auto {0}LayeredBuilder<Parent>::move_from({0}EntityRef&& r) -> Self&", tname)
            .braced([&] {
                w.wl("this->move_initialize_from(std::move(r));");
                w.wl("return *this;");
            });
        w.l();
    }

} // end of ::yazik::compiler namespace