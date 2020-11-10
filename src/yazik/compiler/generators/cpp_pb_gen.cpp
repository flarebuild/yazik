#include "cpp_pb_gen.hpp"

#include <google/protobuf/descriptor.pb.h>
#include <yazik/compiler/utility/writer.hpp>
#include <yazik/compiler/formatters/yaz_formatter.hpp>

namespace yazik::compiler {

    struct GenPb {
        const gp::FileDescriptor* file;
        FileWriter w;
        Formatter f;

        GenPb(const gp::FileDescriptor* file, gpc::GeneratorContext* generator_context)
        : file { file }
        , w{ file, generator_context, "yaz.pb.h"}
        , f{ file, "" }
        {}

        void type_resolver(const gp::FieldDescriptor* field, const std::string& getter);
        void builder_type_resolver(const gp::FieldDescriptor* field, const std::string& getter, bool is_one_of_proc = false);

        void oneof_desc(const gp::OneofDescriptor* one_of);
        void oneof_desc_pb_spec(const gp::OneofDescriptor* one_of);
        void oneof_desc_pb_builder_spec(const gp::OneofDescriptor* one_of);

        void oneof_impl(const gp::OneofDescriptor* one_of);
        void oneof_impl_pb_spec(const gp::OneofDescriptor* one_of);
        void oneof_impl_pb_builder_spec(const gp::OneofDescriptor* one_of);

        void message_desc(const gp::Descriptor* message);
        void message_desc_nested(const gp::Descriptor* message);
        void message_desc_pb_spec(const gp::Descriptor* message);
        void message_desc_pb_builder_spec(const gp::Descriptor* message);

        void message_impl(const gp::Descriptor* message);
        void message_impl_nested(const gp::Descriptor* message);
        void message_impl_pb_spec(const gp::Descriptor* message);
        void message_impl_pb_builder_spec(const gp::Descriptor* message);

        void exec() {
            w.wl("#pragma once");
            w.l();
            w.wl("#include {}", ab(replace_proto(file->name(), "yaz.h")));
            w.wl("#include {}", ab(replace_proto(file->name(), "pb.h")));
            for (int d = 0; d < file->dependency_count(); ++d) {
                auto* dep = file->dependency(d);

                if (dep->syntax() != gp::FileDescriptor::SYNTAX_PROTO3)
                    continue;

                auto dep_name = replace_proto(dep->name(), "yaz.pb.h");
                w.wl("#include {}", ab(dep_name));
            }
            w.l();
            auto body = [&] {
                for (int m = 0; m < file->message_type_count(); ++m)
                    message_desc(file->message_type(m));
                for (int m = 0; m < file->message_type_count(); ++m)
                    message_impl(file->message_type(m));
            };
            if (file->package().empty()) body();
            else w.w("namespace {} ", to_cpp_ns(file->package())).braced(body);
        }

    };

    void GenPb::type_resolver(const gp::FieldDescriptor* field, const std::string& getter) {
        using gpfd = gp::FieldDescriptor;
        if (field->is_repeated()) {
            if (field->message_type()) {
                w.w("return ::ranges::views::all(*a->mutable_{}())", getter);
            } else {
                w.w("return ::ranges::views::all(a->{}())", getter);
            }
            switch (field->type()) {
            case gpfd::TYPE_MESSAGE:
                w.l().with_indent([&] {
                    std::string msg_pbtype = f.pb_type_name(field->message_type());
                    std::string msg_type = f.type_name(field->message_type());
                    w.w("| ::ranges::views::transform([]({}& x) ", msg_pbtype)
                        .braced_detail([&] {
                            w.wl("return {}PbSpec::wrap(x);", msg_type);
                        }, false)
                        .wl(");");
                });
                break;
            case gpfd::TYPE_ENUM:
                w.l().with_indent([&] {
                    std::string enum_type = f.type_name(field->enum_type());
                    w.w("| ::ranges::views::transform([](int x) ")
                        .braced_detail([&] {
                            w.wl("return create_enum<{}Enum>((int)x);", enum_type);
                        }, false)
                        .wl(");");
                });
                break;
            case gpfd::TYPE_STRING:
            case gpfd::TYPE_BYTES:
                w.l().with_indent([&] {
                    w.w("| ::ranges::views::transform([](const auto& x) ")
                        .braced_detail([&] {
                            w.wl("return std::string_view {{ x }};");
                        }, false)
                        .wl(");");
                });
                break;
            default:
                w.wl(";");
                break;
            }
        } else {
            switch (field->type()) {
            case gpfd::TYPE_MESSAGE:
                w.wl("return {}PbSpec::wrap(*a->mutable_{}());", f.type_name(field->message_type()), getter);
                break;
            case gpfd::TYPE_ENUM:
                w.wl("return create_enum<{}Enum>((int)a->{}());", f.type_name(field->enum_type()), getter);
                break;
            default:
                w.wl("return a->{}();", getter);
                break;
            }
        }
    }

    void GenPb::builder_type_resolver(const gp::FieldDescriptor* field, const std::string& getter,  bool is_one_of_proc) {
        using gpfd = gp::FieldDescriptor;
        if (field->is_repeated()) {
            auto tname = f.type_name(field->containing_type());
            auto ftype = f.resolve_builder_type(field);
            switch (field->type()) {
                case gpfd::TYPE_MESSAGE:
                    w.wl(
                        "return {{ a, ({}::builder_factory_t)&{}PbBuilderSpec::{}_factory, {}PbBuilderSpec::vtable() }};",
                        ftype,
                        tname,
                        getter,
                        f.type_name(field->message_type())
                    );
                break;
                default:
                    w.wl(
                        "return {{ a, ({}::push_clbk_t)&{}PbBuilderSpec::{}_pusher }};",
                        ftype,
                        tname,
                        getter
                    );
                break;
            }
        } else {
            if (!is_one_of_proc && field->containing_oneof()) {
                w.wl(
                    "return {{ a, {}PbBuilderSpec::vtable() }};",
                    f.type_name(field->containing_oneof())
                );
                return;
            }

            switch (field->type()) {
            case gpfd::TYPE_MESSAGE:
                if (is_one_of_proc) {
                    w.wl(
                        "auto builder = {}PbBuilderSpec::wrap(* a->mutable_{}());",
                        f.type_name(field->message_type()),
                        getter
                    );
                    w.wl("clbk(builder);");
                } else {
                    w.wl(
                        "return {{ a->mutable_{}(), {}PbBuilderSpec::vtable() }};",
                        getter,
                        f.type_name(field->message_type())
                    );
                }
                break;
            case gpfd::TYPE_ENUM:
                w.wl("a->set_{}(({})value);", getter, f.pb_type_name(field->enum_type()));
                break;
            default:
                w.wl("a->set_{}(std::move(value));", getter);
                break;
            }
        }
    }

    void GenPb::oneof_desc(const gp::OneofDescriptor* one_of) {
        oneof_desc_pb_spec(one_of);
        oneof_desc_pb_builder_spec(one_of);
        // TODO: impl layered
    }

    void GenPb::oneof_desc_pb_spec(const gp::OneofDescriptor* one_of) {
        std::string tname = f.type_name(one_of);
        std::string parent_pb_type = f.pb_type_name(one_of->containing_type());

        w.w("struct {}PbSpec: protected ::yazik::compiler::support::Initializer ",tname)
            .class_braced([&] {
                for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
                    auto *field = one_of->field(f_idx);

                    if (!is_supported(field))
                        continue;

                    std::string getter = f.getter_inner(field);
                    std::string type = f.resolve_type(field);

                    w.wl("[[nodiscard]] static bool is_{}(const {}*);", getter, parent_pb_type);
                    w.wl("[[nodiscard]] static {} {}({}*);", type, getter, parent_pb_type);
                }

                w.wl("[[nodiscard]] static bool is_null(const {}* a);", parent_pb_type);
                w.l();
                w.w("static inline const {}Vtable* vtable() ", tname)
                    .braced([&]{
                        w.w("const static {}Vtable s_vtable ", tname)
                            .braced_detail([&] {
                                for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
                                    auto *field = one_of->field(f_idx);

                                    if (!is_supported(field))
                                        continue;

                                    std::string getter = f.getter_inner(field);
                                    w.wl(".is_{0} = ({1}Vtable::is_{0}_fn)&{1}PbSpec::is_{0},", getter, tname);
                                    w.wl(".{0} = ({1}Vtable::{0}_fn)&{1}PbSpec::{0},", getter, tname);
                                }
                                w.wl(".is_null = ({0}Vtable::is_null_fn)&{0}PbSpec::is_null,", tname);
                            }, false)
                            .wl(";");
                        w.wl("return &s_vtable;");
                    });

                w.l();
                w.w("static inline {}VariantRef wrap({}& a) ", tname, parent_pb_type)
                    .braced([&] {
                        w.wl("return create_entity<{}VariantRef>(static_cast<void*>(&a), vtable());", tname);
                    });
            });
        w.l();
    }

    void GenPb::oneof_desc_pb_builder_spec(const gp::OneofDescriptor* one_of) {
        std::string tname = f.type_name(one_of);
        std::string parent_pb_type = f.pb_type_name(one_of->containing_type());

        w.w("struct {}PbBuilderSpec: protected ::yazik::compiler::support::Initializer ",tname)
            .class_braced([&] {
                for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
                    auto *field = one_of->field(f_idx);

                    if (!is_supported(field))
                        continue;

                    std::string getter = f.getter_inner(field);
                    std::string type = f.resolve_builder_inner_type(field);

                    if (field->is_repeated() || field->message_type()) {
                        w.wl("static void set_{}({}*, ::folly::Function<void({}&)>);", getter, parent_pb_type, type);
                    } else {
                        w.wl("static void set_{}({}*, {});", getter, parent_pb_type, type);
                    }
                }
                w.l();
                w.w("static inline const {}BuilderVtable* vtable() ", tname)
                    .braced([&]{
                        w.w("const static {}BuilderVtable s_vtable ", tname)
                            .braced_detail([&] {
                                for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
                                    auto *field = one_of->field(f_idx);

                                    if (!is_supported(field))
                                        continue;

                                    std::string getter = f.getter_inner(field);
                                    w.wl(".set_{0} = ({1}BuilderVtable::set_{0}_fn)&{1}PbBuilderSpec::set_{0},", getter, tname);
                                }
                            }, false)
                            .wl(";");
                        w.wl("return &s_vtable;");
                    });
                w.l();
                w.w("static inline {}VariantBuilder wrap({}& a) ", tname, parent_pb_type)
                    .braced([&] {
                        w.wl("return create_builder<{}VariantBuilder>(std::make_tuple(static_cast<void*>(&a), vtable()));", tname);
                    });
            });
        w.l();
    }

    void GenPb::oneof_impl(const gp::OneofDescriptor* one_of) {
        oneof_impl_pb_spec(one_of);
        oneof_impl_pb_builder_spec(one_of);
    }

    void GenPb::oneof_impl_pb_spec(const gp::OneofDescriptor* one_of) {
        std::string tname = f.type_name(one_of);
        std::string parent_pb_type = f.pb_type_name(one_of->containing_type());

        for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
            auto *field = one_of->field(f_idx);

            if (!is_supported(field))
                continue;

            std::string getter = f.getter_inner(field);
            std::string type = f.resolve_type(field);

            w.w("inline bool {}PbSpec::is_{}(const {}* a) ", tname, getter, parent_pb_type)
                .braced([&] {
                    w.wl(
                        "return a->{}_case() == {}::k{};",
                        snake_case(one_of->name()),
                        parent_pb_type,
                        camel_case(field->name())
                    );
                });

            w.w("inline {} {}PbSpec::{}({}* a) ", type, tname, getter, parent_pb_type)
                .braced([&] {
                    type_resolver(field, getter);
                });
        }
        w.w("inline bool {}PbSpec::is_null(const {}* a) ", tname, parent_pb_type)
            .braced([&] {
                w.wl(
                    "return a->{}_case() == {}::{}_NOT_SET;",
                    snake_case(one_of->name()),
                    parent_pb_type,
                    camel_case_upper(one_of->name())
                );
            });
        w.l();
    }

    void GenPb::oneof_impl_pb_builder_spec(const gp::OneofDescriptor* one_of) {
        std::string tname = f.type_name(one_of);
        std::string parent_pb_type = f.pb_type_name(one_of->containing_type());

        for (int f_idx = 0; f_idx < one_of->field_count(); ++f_idx) {
            auto *field = one_of->field(f_idx);

            if (!is_supported(field))
                continue;

            std::string getter = f.getter_inner(field);
            std::string type = f.resolve_builder_inner_type(field);

            if (field->is_repeated() || field->message_type()) {
                w.w("inline void {}PbBuilderSpec::set_{}({}* a, ::folly::Function<void({}&)> clbk) ", tname, getter, parent_pb_type, type)
                    .braced([&]{
                        builder_type_resolver(field, getter, true);
                    });
            } else {
                w.w("inline void {}PbBuilderSpec::set_{}({}* a, {} value) ", tname, getter, parent_pb_type, type)
                    .braced([&]{
                        builder_type_resolver(field, getter, true);
                    });
            }
        }
        w.l();
    }

    void GenPb::message_desc(const gp::Descriptor* message) {
        message_desc_nested(message);
        message_desc_pb_spec(message);
        message_desc_pb_builder_spec(message);
    }

    void GenPb::message_desc_nested(const gp::Descriptor* message) {
        // Process nested messages
        for (int n = 0; n < message->nested_type_count(); ++n) {
            auto *nested = message->nested_type(n);
            if (!is_supported(nested))
                continue;

            message_desc(nested);
        }

        // Process oneofs
        for (int o = 0; o < message->oneof_decl_count(); ++o)
            oneof_desc(message->oneof_decl(o));
    }

    void GenPb::message_desc_pb_spec(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);
        std::string pbtype = f.pb_type_name(message);

        w.w("struct {}PbSpec: protected ::yazik::compiler::support::Initializer ",tname)
            .class_braced([&] {
                for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
                    auto *field = message->field(f_idx);
                    if (auto one_of = field->containing_oneof()) {
                        if (one_of->field(0) != field)
                            continue;

                        std::string type = f.type_name(one_of);
                        std::string getter = f.getter(field);

                        w.wl("[[nodiscard]] static {}VariantRef {}({}*);", type, getter, pbtype);

                        continue;
                    }

                    if (!is_supported(field))
                        continue;

                    std::string getter = f.getter(field);
                    std::string type = f.resolve_type(field);
                    w.wl("[[nodiscard]] static {} {}({}*);", type, getter, pbtype);
                }
                w.wl("static {}Builder as_builder({}*);", tname, pbtype);
                w.wl("static std::string serialize({}*);", pbtype);
                w.l();

                w.w("static inline const {}Vtable* vtable() ", tname)
                    .braced([&]{
                        w.w("const static {}Vtable s_vtable ", tname)
                            .class_braced([&] {
                                for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
                                    auto *field = message->field(f_idx);

                                    if (auto one_of = field->containing_oneof()) {
                                        if (one_of->field(0) != field)
                                            continue;

                                        std::string getter = f.getter(field);
                                        w.wl(".{0} = ({1}Vtable::{0}_fn)&{1}PbSpec::{0},", getter, tname);
                                        continue;
                                    }

                                    if (!is_supported(field))
                                        continue;

                                    std::string getter = f.getter(field);
                                    w.wl(".{0} = ({1}Vtable::{0}_fn)&{1}PbSpec::{0},", getter, tname);
                                }
                                w.wl(".as_builder = ({0}Vtable::as_builder_fn)&{0}PbSpec::as_builder,", tname);
                                w.wl(".serialize = ({0}Vtable::serialize_fn)&{0}PbSpec::serialize,", tname);
                            });
                        w.wl("return &s_vtable;");
                    });
                w.l();
                w.w("static inline {}EntityRef wrap({}& a)", tname, pbtype)
                    .braced([&] {
                        w.wl("return create_entity<{}EntityRef>(static_cast<void*>(&a), vtable());", tname);
                    });
            });
        w.l();
    }

    void GenPb::message_desc_pb_builder_spec(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);
        std::string pbtype = f.pb_type_name(message);

        w.w("struct {}PbBuilderSpec: protected ::yazik::compiler::support::Initializer ",tname)
            .class_braced([&] {
                for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
                    auto *field = message->field(f_idx);

                    if (need_pass(field))
                        continue;

                    std::string getter = f.getter(field);
                    std::string type = f.resolve_builder_type(field);

                    if (is_factory_based(field)) {
                        if (field->is_repeated()) {
                            std::string inner_type = f.resolve_builder_inner_type(field);
                            if (field->message_type()) {
                                std::string iner_pb_type = f.pb_type_name(field->message_type());
                                w.wl("static {}* {}_factory({}*);", iner_pb_type, getter, pbtype);
                            } else {
                                w.wl("static void {}_pusher({}*, {});", getter, pbtype, inner_type);
                            }
                        }
                        w.wl(
                            "static {0}BuilderVtable::{1}_args_tuple_t {1}_builder_args_factory({2} *);",
                            tname,
                            getter,
                            pbtype
                        );
                    } else {
                        w.wl("static void set_{}({}*, {});", getter, pbtype, type);
                    }
                }
                w.wl("static void move_initialize_from({0}*, {0}*);", pbtype);
                w.wl("[[nodiscard]] static bool deserialize({}*, std::string_view);", pbtype);
                w.wl("[[nodiscard]] static {}EntityRef as_ref(void*);", tname);
                w.l();
                w.w("static inline const {}BuilderVtable* vtable() ", tname)
                    .braced([&]{
                        w.w("const static {}BuilderVtable s_vtable ", tname)
                            .braced_detail([&] {
                                for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
                                    auto *field = message->field(f_idx);

                                    if (need_pass(field))
                                        continue;

                                    std::string getter = f.getter(field);

                                    if (is_factory_based(field)) {
                                        w.wl(
                                            ".{0}_builder_args_factory = ({1}BuilderVtable::{0}_builder_args_factory_fn)\n"
                                                "    &{1}PbBuilderSpec::{0}_builder_args_factory,",
                                            getter,
                                            tname
                                        );
                                    } else {
                                        w.wl(
                                            ".set_{0} = ({1}BuilderVtable::set_{0}_fn)&{1}PbBuilderSpec::set_{0},",
                                            getter,
                                            tname
                                        );
                                    }
                                }
                                w.wl(".move_initialize_from = ({0}BuilderVtable::move_initialize_from_fn)&{0}PbBuilderSpec::move_initialize_from,", tname);
                                w.wl(".deserialize = ({0}BuilderVtable::deserialize_fn)&{0}PbBuilderSpec::deserialize,", tname);
                                w.wl(".as_ref = ({0}BuilderVtable::as_ref_fn)&{0}PbBuilderSpec::as_ref,", tname);
                            }, false)
                            .wl(";");
                        w.wl("return &s_vtable;");
                    });
                w.l();
                w.w("static inline {}Builder wrap({}& a)", tname, pbtype)
                    .braced([&] {
                        w.wl("return create_builder<{}Builder>(std::make_tuple(static_cast<void*>(&a), vtable()));", tname);
                    });
                w.l();
                w.wl("template <typename Parent>");
                w.w("static inline {}LayeredBuilder<Parent> wrap_with_parent", tname)
                    .braced_detail([&]{
                        w.wl("{}& pb,", pbtype);
                        w.wl("Parent& parent");
                    }, false, "(", ") ")
                    .braced([&] {
                        w.w("return create_builder_with_parent<{}LayeredBuilder<Parent>>", tname)
                            .braced_detail([&]{
                                w.w("std::make_tuple")
                                    .braced_detail([&]{
                                        w.wl("&pb,");
                                        w.wl("{}PbBuilderSpec::vtable()", tname);
                                    }, true, "(", "),");
                                w.wl("&parent");
                            }, true, "(", ");");
                    });
            });
        w.l();
    }

    void GenPb::message_impl(const gp::Descriptor* message) {
        message_impl_nested(message);
        message_impl_pb_spec(message);
        message_impl_pb_builder_spec(message);
    }

    void GenPb::message_impl_nested(const gp::Descriptor* message) {
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

    void GenPb::message_impl_pb_spec(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);
        std::string pbtype = f.pb_type_name(message);

        for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
            auto *field = message->field(f_idx);
            if (auto one_of = field->containing_oneof()) {
                if (one_of->field(0) != field)
                    continue;

                std::string type = f.type_name(one_of);
                std::string getter = f.getter(field);

                w.w("inline {}VariantRef {}PbSpec::{}({}* a) ", type, tname, getter, pbtype)
                    .braced([&] {
                        w.wl("return {}PbSpec::wrap(*a);", type);
                    });

                continue;
            }

            if (!is_supported(field))
                continue;

            std::string getter = f.getter(field);
            std::string type = f.resolve_type(field);
            w.w("inline {} {}PbSpec::{}({}* a) ", type, tname, getter, pbtype)
                .braced([&] {
                    type_resolver(field, getter);
                });
        }
        w.w("inline {0}Builder {0}PbSpec::as_builder({1}* a) ", tname, pbtype)
            .braced([&] {
                w.wl("return {}PbBuilderSpec::wrap(*a);", tname);
            });
        w.w("std::string {}PbSpec::serialize({}* a)", tname, pbtype)
            .braced([&] {
                w.wl("return a->SerializeAsString();");
            });

        w.l();
    }

    void GenPb::message_impl_pb_builder_spec(const gp::Descriptor* message) {
        std::string tname = f.type_name(message);
        std::string pbtype = f.pb_type_name(message);

        for (int f_idx = 0; f_idx < message->field_count(); ++f_idx) {
            auto *field = message->field(f_idx);

            if (need_pass(field))
                continue;

            std::string getter = f.getter(field);
            std::string type = f.resolve_builder_type(field);

            if (field->is_repeated()) {
                std::string inner_type = f.resolve_builder_inner_type(field);
                if (!field->message_type()) {
                    w.w("inline void {}PbBuilderSpec::{}_pusher({}* a, {} value) ", tname, getter, pbtype, inner_type)
                        .braced([&]{
                            if (field->enum_type()) {
                                w.wl("a->mutable_{}()->Add(({})value);", getter, f.pb_type_name(field->enum_type()));
                            } else {
                                w.wl("a->mutable_{}()->Add(std::move(value));", getter);
                            }
                        });
                } else {
                    std::string iner_pb_type = f.pb_type_name(field->message_type());
                    w.w("inline {}* {}PbBuilderSpec::{}_factory({}* a) ", iner_pb_type, tname, getter, pbtype)
                        .braced([&]{
                            w.wl("return a->mutable_{}()->Add();", getter);
                        });
                }
            }

            if (is_factory_based(field)) {
                w.w(
                    "inline {0}BuilderVtable::{1}_args_tuple_t {0}PbBuilderSpec::{1}_builder_args_factory({2} * a) ",
                    tname,
                    getter,
                    pbtype
                ).braced([&] {
                        builder_type_resolver(field, getter);
                    });
            } else {
                w.w("inline void {}PbBuilderSpec::set_{}({}* a, {} value) ", tname, getter, pbtype, type)
                    .braced([&] {
                        builder_type_resolver(field, getter);
                    });
            }
        }
        w.w("inline void {0}PbBuilderSpec::move_initialize_from({1}* a, {1}* other) ", tname, pbtype)
            .braced([&] {
                w.wl("a->Swap(other);");
            });

        w.w("inline bool {}PbBuilderSpec::deserialize({}* a, std::string_view data) ", tname, pbtype)
            .braced([&] {
                w.wl("return a->ParseFromArray(data.data(), data.size());");
            });

        w.w("inline {0}EntityRef {0}PbBuilderSpec::as_ref(void* ptr) ", tname)
            .braced([&]{
                w.wl("return create_entity<{0}EntityRef>(ptr, {0}PbSpec::vtable());", tname);
            });
        w.l();
    }

    Result<bool> generate_yaz_pb(
        const gp::FileDescriptor* file,
        const std::string& parameter,
        gpc::GeneratorContext* generator_context
    ) noexcept {
        GenPb { file, generator_context }.exec();
        co_return true;
    }

} // end of ::yazik::compiler namespace