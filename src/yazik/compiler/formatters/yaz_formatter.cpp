#include "yaz_formatter.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <yazik/utility/result.hpp>

namespace yazik::compiler {

    Formatter::Formatter(
        const gp::FileDescriptor* cur_fd,
        std::string type_prefix
    ) noexcept
        : _cur_fd { cur_fd }
        , _type_prefix { std::move(type_prefix) }
    {}

    std::string Formatter::ns(const gp::FileDescriptor* fd) const {
        if (_cur_fd == fd || _cur_fd->package() == fd->package()) return "";
        return do_sformat("{}::", to_cpp_ns(fd->package()));
    }

    std::string Formatter::ns_pref(const gp::FileDescriptor* fd) const {
        std::string nsstr = ns(fd);
        return nsstr;
    }

    std::string Formatter::yaz_type_name(std::string name) const {
        return ::boost::replace_all_copy(camel_case(name), "_", "");
    }

    void resolve_precondition(const gp::FieldDescriptor* field) {
        using gpfd = gp::FieldDescriptor;
        if (field->is_extension())
            throw runtime_fail(
                "extensions are not supported: {}",
                field->DebugString()
            );
        if (field->is_map())
            throw runtime_fail(
                "maps are not supported: {}",
                field->DebugString()
            );
        if (field->type() == gpfd::TYPE_GROUP)
            throw runtime_fail(
                "groups are not supported: {}",
                field->DebugString()
            );
    }

    std::string Formatter::resolve_concept_type_common(const gp::FieldDescriptor* field) const {
        using gpfd = gp::FieldDescriptor;
        switch (field->type()) {
            case gpfd::TYPE_DOUBLE:
                return "double";
            case gpfd::TYPE_FLOAT:
                return "float";
            case gpfd::TYPE_INT64:
                return "int64_t";
            case gpfd::TYPE_UINT64:
                return "uint64_t";
            case gpfd::TYPE_INT32:
                return "int32_t";
            case gpfd::TYPE_FIXED64:
                return "uint64_t";
            case gpfd::TYPE_FIXED32:
                return "uint32_t";
            case gpfd::TYPE_BOOL:
                return "bool";
            case gpfd::TYPE_UINT32:
                return "uint32_t";
            case gpfd::TYPE_SFIXED32:
                return "int32_t";
            case gpfd::TYPE_SFIXED64:
                return "int64_t";
            case gpfd::TYPE_SINT32:
                return "int32_t";
            case gpfd::TYPE_SINT64:
                return "int64_t";
            default:
                __builtin_unreachable();
        }
    }

    std::string Formatter::clbk_concept_name(const gp::FieldDescriptor* field) const {
        std::string tname = snake_case(type_name(field->containing_type()));
        std::string fn_name = getter(field);
        return do_sformat("c_{}_{}_clbk", tname, fn_name);
    }

    std::string Formatter::resolve_builder_args_tuple(const gp::FieldDescriptor* field) const {
        if (auto one_of = field->containing_oneof()) {
            return do_sformat(
                "std::tuple<\n    void*,\n    const {}BuilderVtable*\n>",
                type_name(one_of)
            );
        } else if (field->message_type()) {
            if (field->is_repeated()) {
                return do_sformat(
                    "std::tuple<\n    void*,\n    {}::builder_factory_t,\n    const {}BuilderVtable*\n>",
                    resolve_builder_type(field),
                    type_name(field->message_type())
                );
            } else {
                return do_sformat(
                    "std::tuple<\n    void*,\n    const {}BuilderVtable*\n>",
                    type_name(field->message_type())
                );
            }
        } else {
            return do_sformat(
                "std::tuple<\n    void*,\n    {}::push_clbk_t\n>",
                resolve_builder_type(field)
            );
        }
        return "";
    }

    std::string Formatter::resolve_builder_inner_type(const gp::FieldDescriptor* field) const {
        using gpfd = gp::FieldDescriptor;
        resolve_precondition(field);
        std::string result;
        switch (field->type()) {
            case gpfd::TYPE_MESSAGE:
                result = type_name(field->message_type()) + "Builder";
                break;
            case gpfd::TYPE_ENUM:
                result = type_name(field->enum_type()) + "Enum::Value";
                break;
            case gpfd::TYPE_STRING:
                result = "std::string";
                break;
            case gpfd::TYPE_BYTES:
                result = "std::string";
                break;
            default:
                result = resolve_concept_type_common(field);
        }
        return result;
    }

    std::string Formatter::resolve_builder_type(const gp::FieldDescriptor* field) const {
        using gpfd = gp::FieldDescriptor;
        resolve_precondition(field);
        if (auto* one_of = field->containing_oneof()) {
            return do_sformat("{}VariantBuilder", type_name(one_of));
        }
        std::string result = resolve_builder_inner_type(field);
        if (!field->is_repeated())
            return result;
        else if (field->type() == gpfd::TYPE_MESSAGE)
            return do_sformat("::yazik::compiler::support::VecEntityBuilder<{0}, {0}Vtable>", result);
        else
            return do_sformat("::yazik::compiler::support::VecBuilder<{}>", result);

    }

    std::string Formatter::resolve_type(const gp::FieldDescriptor* field) const {
        using gpfd = gp::FieldDescriptor;
        resolve_precondition(field);
        std::string result;
        switch (field->type()) {
        case gpfd::TYPE_MESSAGE:
            result = type_name(field->message_type()) + "EntityRef";
            break;
        case gpfd::TYPE_ENUM:
            result = type_name(field->enum_type()) + "Enum";
            break;
        case gpfd::TYPE_STRING:
            result = "std::string_view";
            break;
        case gpfd::TYPE_BYTES:
            result = "std::string_view";
            break;
        default:
            result = resolve_concept_type_common(field);
        }
        if (!field->is_repeated()) return result;
        else return do_sformat("::ranges::any_view<{}>", result);
    }

    std::string Formatter::getter_inner(const gp::FieldDescriptor* field) const {
        std::string result = snake_case(field->name());
        if (s_keywords.contains(result))
            result = result + "_";
        return result;
    }

    std::string Formatter::getter(const gp::FieldDescriptor* field) const {
        std::string result = field->containing_oneof()
            ? snake_case(field->containing_oneof()->name())
            : snake_case(field->name());
        if (s_keywords.contains(result))
            result = result + "_";
        return result;
    }

    const std::set<std::string> Formatter::s_keywords = {
        "delete"
    };

} // end of ::yazik::compiler namespace