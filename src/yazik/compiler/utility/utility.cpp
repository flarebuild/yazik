#include <google/protobuf/compiler/cpp/cpp_helpers.h>
#include <boost/algorithm/string.hpp>

#include <chaiscript/chaiscript.hpp>

#include "utility.hpp"
#include <algorithm>

#include <yazik/utility/macro.hpp>

namespace yazik::compiler {

    std::string q(const std::string& str) {
        return do_sformat("\"{}\"", str);
    }

    std::string ab(const std::string& str) {
        return do_sformat("<{}>", str);
    }

    std::string qb(const std::string& str) {
        return do_sformat("<{}>", str);
    }

    std::string strip_proto(const std::string& filename) {
        return gpc::cpp::StripProto(filename);
    }

    std::string replace_proto(const std::string& filename, const std::string& ext) {
        return do_sformat("{}.{}", strip_proto(filename), ext);
    }

    std::string camel_case(const std::string& str) {
        return gpc::cpp::UnderscoresToCamelCase(str, true);
    }

    std::string camel_case_upper(const std::string& str) {
        std::string copy = str;
        for (auto & c: copy) c = std::toupper(c);
        return copy;
    }

    std::string snake_case(const std::string& str) {
        std::string result(1, tolower(str[0]));

        // First place underscores between contiguous lower and upper case letters.
        // For example, `_LowerCamelCase` becomes `_Lower_Camel_Case`.
        for (auto it = str.begin() + 1; it != str.end(); ++it) {
            if (isupper(*it) && *(it-1) != '_' && islower(*(it-1))) {
                result += "_";
            }
            result += *it;
        }

        // Then convert it to lower case.
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);

        return result;
    }

    std::string to_cpp_ns(const std::string& ns) {
        return ::boost::replace_all_copy(ns, ".", "::");
    }

    template <Descriptable T>
    std::string descriptable_ns(T* d, const gp::FileDescriptor* f) {
        std::string pref;
        if (d->file() != f)
            pref = do_sformat("{}::yaz::", to_cpp_ns(d->file()->package()));

        std::string result;
        if (auto* parent = d->containing_type()) {
            do {
                result = do_sformat(
                    "{}::{}",
                    snake_case(parent->name()),
                    result
                );
                parent = parent->containing_type();
            } while (parent);
        }

        return do_sformat(
            "{}{}{}",
            pref,
            result,
            snake_case(d->name())
        );
    }

    std::string ns_pb(
        const gp::FileDescriptor* cur_file,
        const gp::FileDescriptor* fd
    ) {
        if (cur_file == fd || cur_file->package() == fd->package()) return "";
        return do_sformat("{}::", to_cpp_ns(fd->package()));
    }

    template <Descriptable T>
    std::string pb_type_name_base(const T* value) {
        std::string name = value->name();
        if (auto* parent = value->containing_type())
            do {
                name = do_sformat(
                    "{}_{}",
                    parent->name(),
                    name
                );
                parent = parent->containing_type();
            } while (parent);
        return name;
    }

    template <Descriptable T>
    std::string pb_type_name(
        const gp::FileDescriptor* cur_file,
        const T* value
    ) {
        return do_sformat(
            "{}{}",
            ns_pb(cur_file, value->file()),
            pb_type_name_base(value)
        );
    }

    template<typename Descriptor, typename Wrap>
    class DescriptorPtrWrap {
    protected:
        const gp::FileDescriptor* _cur_file;
        const Descriptor* _descriptor;
        FileWriter* _writer;

    public:

        using descriptor_t = Descriptor;

        DescriptorPtrWrap()
        : _cur_file { nullptr }
        , _descriptor { nullptr }
        , _writer { nullptr }
        {}

        DescriptorPtrWrap(
            const gp::FileDescriptor* cur_file,
            const Descriptor* descriptor,
            FileWriter& writer
        )
        : _cur_file { cur_file }
        , _descriptor { descriptor }
        , _writer { &writer }
        {}

        DescriptorPtrWrap(const Wrap& other)
        : _cur_file { other._cur_file }
        , _descriptor { other._descriptable }
        , _writer { other->_writer }
        {}

        template<typename WrapInner>
        std::vector<WrapInner> list_descriptors(
            int (Descriptor::*counter)() const,
            const typename WrapInner::descriptor_t* (Descriptor::*getter)(int) const,
            std::function<bool (const WrapInner&)> filter = nullptr
        ) const {
            return views::ints(0, (_descriptor->*(counter))())
                | views::transform([&](int i) {
                    return WrapInner {
                        _cur_file,
                        (_descriptor->*(getter))(i),
                        *_writer
                    };
                })
                | views::remove_if([_l_move(filter)](const WrapInner& i) {
                    if (!filter) return false;
                    return !filter(i);
                })
                | ranges::to_vector;
        }

        static std::vector<chaiscript::Proxy_Function> default_contructors() {
            using namespace chaiscript;
            return {
                constructor<Wrap()>(),
                constructor<Wrap(const Wrap &)>(),
            };
        }

        static void add_container_types(
            chaiscript::ChaiScript& script,
            const std::string& name
        ) {
            using namespace chaiscript;
            script.add(
                bootstrap::standard_library::vector_type<
                    std::vector<Wrap>
                >(name + "List")
            );
        }

        static void define_chai_class(
            chaiscript::ChaiScript& script,
            const std::string& name,
            const std::vector<std::pair<chaiscript::Proxy_Function, std::string>> & methods
        ) {
            using namespace chaiscript;
            auto m = ModulePtr {new Module };
            ::chaiscript::utility::add_class<Wrap>(
                *m, name,
                default_contructors(),
                methods
            );

            add_container_types(script, name);
            script.add(m);
        }
    };

#define ChaiMemberFn(fn)  { ::chaiscript::fun(&Self::fn), YAZ_STRINGIFY(fn) }

     struct EnumValueDescriptorWrap
    : DescriptorPtrWrap<
        gp::EnumValueDescriptor,
        EnumValueDescriptorWrap
    > {
        using Self = EnumValueDescriptorWrap;
        using Base = DescriptorPtrWrap<
            gp::EnumValueDescriptor,
            EnumValueDescriptorWrap
        >;

        using Base::Base;

        std::string name() const { return _descriptor->name(); }
        std::string snake_case_name() const { return snake_case(name()); }

        int number() const { return _descriptor->number(); }

        static void add_to_chai(chaiscript::ChaiScript& script);
     };

    struct EnumDescriptorWrap
    : DescriptorPtrWrap<
        gp::EnumDescriptor,
        EnumDescriptorWrap
    > {
        using Self = EnumDescriptorWrap;
        using Base = DescriptorPtrWrap<
            gp::EnumDescriptor,
            EnumDescriptorWrap
        >;

        using Base::Base;

        std::string name() const { return _descriptor->name(); }
        std::string snake_case_name() const { return snake_case(name()); }

        void cpp_namespaced(std::function<void ()> fn) const {
            _writer->w("namespace {} ", snake_case_name())
                .braced(fn);
        }

        std::string ns() const { return descriptable_ns(_descriptor, _cur_file); }

        std::string cpp_t() const { return do_sformat("{}::Enum", ns()); }

        std::string cpp_pb_t() const { return pb_type_name(_cur_file, _descriptor); }

        std::vector<EnumValueDescriptorWrap> values() const {
            return list_descriptors<EnumValueDescriptorWrap>(
                &gp::EnumDescriptor::value_count,
                &gp::EnumDescriptor::value
            );
        }

        static void add_to_chai(chaiscript::ChaiScript& script);
    };

    const std::set<std::string> s_cpp_keywords = {
        "delete"
    };

    struct DescriptorWrap;
    struct OneOfDescriptorWrap;

    struct FieldDescriptorWrap
    : DescriptorPtrWrap<
        gp::FieldDescriptor,
        FieldDescriptorWrap
    > {
        using Self = FieldDescriptorWrap;
        using Base = DescriptorPtrWrap<
            gp::FieldDescriptor,
            FieldDescriptorWrap
        >;

        using Base::Base;

        std::string camel_case_name() const {
            return camel_case(_descriptor->name());
        }

        bool not_oneof_or_first() const {
            auto* one_of = _descriptor->containing_oneof();
            if (!one_of)
                return true;

            return one_of->field(0) == _descriptor;
        }

        bool is_supported() const {
            return ::yazik::compiler::is_supported(_descriptor);
        }

        std::string cpp_in_variant_getter() const {
            std::string result = snake_case(_descriptor->name());
            if (s_cpp_keywords.contains(result))
                result = result + "_";
            return result;
        }

        std::string cpp_in_message_getter() const {
            std::string result = _descriptor->containing_oneof()
                ? snake_case(_descriptor->containing_oneof()->name())
                : snake_case(_descriptor->name());
            if (s_cpp_keywords.contains(result))
                result = result + "_";
            return result;
        }

        void resolve_precondition() const {
            using gpfd = gp::FieldDescriptor;
            if (_descriptor->is_extension())
                throw runtime_fail(
                    "extensions are not supported: {}",
                    _descriptor->DebugString()
                );
            if (_descriptor->is_map())
                throw runtime_fail(
                    "maps are not supported: {}",
                    _descriptor->DebugString()
                );
            if (_descriptor->type() == gpfd::TYPE_GROUP)
                throw runtime_fail(
                    "groups are not supported: {}",
                    _descriptor->DebugString()
                );
        }

        std::string resolve_cpp_type_common() const {
            using gpfd = gp::FieldDescriptor;
            switch (_descriptor->type()) {
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

        void cpp_unwrap_repeated();
        void cpp_unwrap_in_variant();
        void cpp_unwrap_in_message();
        void cpp_unwrap_builder_repeated();
        void cpp_unwrap_builder_in_variant();
        void cpp_unwrap_builder_in_message();
        std::string cpp_to_dynamic_in_variant();
        std::string cpp_to_dynamic_in_message();

        bool is_repeated() const {
            return _descriptor->is_repeated();
        }

        bool is_message() const {
            return _descriptor->type() == gp::FieldDescriptor::TYPE_MESSAGE;
        }

        DescriptorWrap as_message() const;

        bool is_enum() const {
            return _descriptor->type() == gp::FieldDescriptor::TYPE_ENUM;
        }

        EnumDescriptorWrap as_enum() const;

        bool is_variant() const {
            return _descriptor->containing_oneof() != nullptr;
        }

        OneOfDescriptorWrap as_variant() const;

        DescriptorWrap parent() const;

        std::string resolve_cpp_ref_t_in_variant() const;
        std::string resolve_cpp_ref_t_in_message() const;
        std::string resolve_cpp_builder_t_inner() const;
        std::string resolve_cpp_builder_t_in_variant() const;
        std::string resolve_cpp_builder_t_in_message() const;
        std::string resolve_cpp_layered_builder_t() const;
        std::string resolve_cpp_layered_inner_builder_t() const;
        static void add_to_chai(chaiscript::ChaiScript& script);
    };

    struct OneOfDescriptorWrap
    : DescriptorPtrWrap<
        gp::OneofDescriptor,
        OneOfDescriptorWrap
    > {
        using Self = OneOfDescriptorWrap;
        using Base = DescriptorPtrWrap<
            gp::OneofDescriptor,
            OneOfDescriptorWrap
        >;

        using Base::Base;

        std::string name() const { return _descriptor->name(); }
        std::string snake_case_name() const { return snake_case(name()); }
        std::string camel_case_upper_name() const { return camel_case_upper(name()); }

        void cpp_namespaced(std::function<void ()> fn) const {
            _writer->w("namespace {} ", snake_case_name())
                .braced(fn);
        }

        std::string ns() const { return descriptable_ns(_descriptor, _cur_file); }

        std::string cpp_ref_t() const { return do_sformat("{}::Ref", ns()); }
        std::string cpp_builder_t() const { return do_sformat("{}::Builder", ns()); }
        std::string cpp_pb_spec_t() const { return do_sformat("{}::RefPbSpec", ns()); }
        std::string cpp_builder_pb_spec_t() const { return do_sformat("{}::BuilderPbSpec", ns()); }

        DescriptorWrap parent() const;

        std::vector<FieldDescriptorWrap> fields() const {
            return list_descriptors<FieldDescriptorWrap>(
                &gp::OneofDescriptor::field_count,
                &gp::OneofDescriptor::field,
                [] (auto& f) { return f.is_supported(); }
            );
        }

        static void add_to_chai(chaiscript::ChaiScript& script);
    };

    struct DescriptorWrap
    : DescriptorPtrWrap<
        gp::Descriptor,
        DescriptorWrap
    > {
        using Self = DescriptorWrap;
        using Base = DescriptorPtrWrap<
            gp::Descriptor,
            DescriptorWrap
        >;

        using Base::Base;

        std::string name() const { return _descriptor->name(); }
        std::string snake_case_name() const { return snake_case(name()); }

        void cpp_namespaced(std::function<void ()> fn) const {
            _writer->w("namespace {} ", snake_case_name())
                .braced(fn);
        }

        std::string ns() const { return descriptable_ns(_descriptor, _cur_file); }
        std::string cpp_ref_t() const { return do_sformat("{}::Ref", ns()); }
        std::string cpp_builder_t() const { return do_sformat("{}::Builder", ns()); }

        bool is_supported() const {
            return _descriptor->map_key() == nullptr;
        }

        std::string cpp_layered_builder_t(const std::string& templ) const {
            if (templ.empty())
                return do_sformat("{}::LayeredBuilder", ns());
            return do_sformat("{}::LayeredBuilder<{}>", ns(), templ);
        }

        std::string cpp_pb_t() const {
            return pb_type_name(_cur_file, _descriptor);
        }

        std::string cpp_pb_spec_t() const {
            return do_sformat("{}::RefPbSpec", ns());
        }

        std::string cpp_builder_pb_spec_t() const {
            return do_sformat("{}::BuilderPbSpec", ns());
        }

        std::vector<EnumDescriptorWrap> enums() const {
            return list_descriptors<EnumDescriptorWrap>(
                &gp::Descriptor::enum_type_count,
                &gp::Descriptor::enum_type
            );
        }

        std::vector<DescriptorWrap> nested() const {
            return list_descriptors<DescriptorWrap>(
                &gp::Descriptor::nested_type_count,
                &gp::Descriptor::nested_type,
                [] (auto& d) { return d.is_supported(); }
            );
        }

        std::vector<OneOfDescriptorWrap> variants() const {
            return list_descriptors<OneOfDescriptorWrap>(
                &gp::Descriptor::oneof_decl_count,
                &gp::Descriptor::oneof_decl
            );
        }

        std::vector<FieldDescriptorWrap> fields() const {
            return list_descriptors<FieldDescriptorWrap>(
                &gp::Descriptor::field_count,
                &gp::Descriptor::field,
                [] (auto& f) { return f.is_supported() && f.not_oneof_or_first(); }
            );
        }


        static void add_to_chai(chaiscript::ChaiScript& script);
    };

    struct ServiceDescriptorWrap;

    struct MethodDescriptorWrap
    : DescriptorPtrWrap<
        gp::MethodDescriptor,
        MethodDescriptorWrap
    > {
        using Self = MethodDescriptorWrap;
        using Base = DescriptorPtrWrap<
            gp::MethodDescriptor,
            MethodDescriptorWrap
        >;

        using Base::Base;

        std::string name() const { return _descriptor->name(); }
        std::string snake_case_name() const { return snake_case(name()); }

        void cpp_namespaced(std::function<void ()> fn) const {
            _writer->w("namespace {} ", snake_case_name())
                .braced(fn);
        }

        ServiceDescriptorWrap service() const;
        DescriptorWrap input() const {
            return { _cur_file, _descriptor->input_type(), *_writer };
        }
        DescriptorWrap output() const {
            return { _cur_file, _descriptor->output_type(), *_writer };
        }

        bool is_supported() const { return ::yazik::compiler::is_supported(_descriptor); }

        bool is_server_streaming() const { return _descriptor->server_streaming(); }

        bool is_client_streaming() const { return _descriptor->client_streaming(); }

        std::string cpp_rpc_type_sync() const {
            if (is_server_streaming())
                return "::yazik::rpc::RpcUnitType::ServerStreamingSync";
            else if (is_client_streaming())
                return "::yazik::rpc::RpcUnitType::ClientStreamingSync";
            else
                return "::yazik::rpc::RpcUnitType::UnarySync";
        }

        std::string cpp_rpc_call_response_sync() const {
            if (is_server_streaming())
                return "::yazik::rpc::RpcGenerator<context_t::resp_ok_t>";
            else
                return "::yazik::rpc::RpcResult<context_t::resp_ok_t>";
        }

        std::string cpp_rpc_call_cast_as_broken_sts_sync() const {
            return ".as_broken<context_t::resp_ok_t>";
        }

        std::string cpp_rpc_call_request_sync() const {
            if (is_client_streaming())
                return do_sformat(
                    "::yazik::rpc::RpcGenerator<{}>",
                    input().cpp_ref_t()
                );
            else
                return input().cpp_ref_t();
        }


        std::string cpp_rpc_type_async() const {
            if (is_server_streaming())
                return "::yazik::rpc::RpcUnitType::ServerStreamingAsync";
            else if (is_client_streaming())
                return "::yazik::rpc::RpcUnitType::ClientStreamingAsync";
            else
                return "::yazik::rpc::RpcUnitType::UnaryAsync";
        }

        std::string cpp_rpc_call_response_async() const {
            if (is_server_streaming())
                return "::yazik::rpc::RpcChannel<context_t::resp_ok_t>";
            else
                return "::yazik::rpc::RpcTask<context_t::resp_ok_t>";
        }

        std::string cpp_rpc_call_cast_as_broken_sts_async() const {
            if (is_server_streaming())
                return ".as_broken_channel<context_t::resp_ok_t>";
            else
                return ".as_broken_task<context_t::resp_ok_t>";
        }

        std::string cpp_rpc_call_request_async() const {
            if (is_client_streaming())
                return do_sformat(
                    "::yazik::rpc::RpcChannel<{}>",
                    input().cpp_ref_t()
                );
            else
                return input().cpp_ref_t();
        }

        static void add_to_chai(chaiscript::ChaiScript& script);
    };

    struct ServiceDescriptorWrap
    : DescriptorPtrWrap<
        gp::ServiceDescriptor,
        ServiceDescriptorWrap
    > {
        using Self = ServiceDescriptorWrap;
        using Base = DescriptorPtrWrap<
            gp::ServiceDescriptor,
            ServiceDescriptorWrap
        >;

        using Base::Base;

        std::string name() const { return _descriptor->name(); }
        std::string snake_case_name() const { return snake_case(name()); }

        void cpp_namespaced(std::function<void ()> fn) const {
            _writer->w("namespace {} ", snake_case_name())
                .braced(fn);
        }

        std::vector<MethodDescriptorWrap> methods() const {
            return list_descriptors<MethodDescriptorWrap>(
                &gp::ServiceDescriptor::method_count,
                &gp::ServiceDescriptor::method,
                [] (auto& m) { return m.is_supported(); }
            );
        }

        static void add_to_chai(chaiscript::ChaiScript& script);
    };

    struct FileDescriptorWrap
    : DescriptorPtrWrap<
        gp::FileDescriptor,
        FileDescriptorWrap
    > {
        using Self = FileDescriptorWrap;
        using Base = DescriptorPtrWrap<
            gp::FileDescriptor,
            FileDescriptorWrap
        >;

        using Base::Base;

        std::string file_name() const{ return _descriptor->name(); }
        bool has_rpc() const { return _descriptor->service_count() != 0; }
        bool is_supported() const { return _descriptor->syntax() ==  gp::FileDescriptor::SYNTAX_PROTO3; }
        bool has_deps() const { return _descriptor->dependency_count() != 0; }

        std::vector<FileDescriptorWrap> deps() const {
            return list_descriptors<FileDescriptorWrap>(
                &gp::FileDescriptor::dependency_count,
                &gp::FileDescriptor::dependency,
                [] (auto& d) { return d.is_supported(); }
            );
        }

        std::vector<EnumDescriptorWrap> enums() const {
            return list_descriptors<EnumDescriptorWrap>(
                &gp::FileDescriptor::enum_type_count,
                &gp::FileDescriptor::enum_type
            );
        }

        std::vector<DescriptorWrap> messages() const {
            return list_descriptors<DescriptorWrap>(
                &gp::FileDescriptor::message_type_count,
                &gp::FileDescriptor::message_type
            );
        }

        std::vector<ServiceDescriptorWrap> services() const {
            return list_descriptors<ServiceDescriptorWrap>(
                &gp::FileDescriptor::service_count,
                &gp::FileDescriptor::service
            );
        }

        void cpp_namespaced(std::function<void ()> fn) const {
            std::string ns;
            if (!_descriptor->package().empty()) {
                ns = do_sformat("{}::yaz", to_cpp_ns(_descriptor->package()));
            } else {
                ns = "yaz";
            }
            _writer->w("namespace {} ", ns)
                .braced(fn);
        }

        static void add_to_chai(chaiscript::ChaiScript& script);
    };

    DescriptorWrap OneOfDescriptorWrap::parent() const {
        return { _cur_file, _descriptor->containing_type(), *_writer };
    }

    DescriptorWrap FieldDescriptorWrap::as_message() const {
        return { _cur_file, _descriptor->message_type(), *_writer };
    }

    EnumDescriptorWrap FieldDescriptorWrap::as_enum() const {
        return { _cur_file, _descriptor->enum_type(), *_writer };
    }

    OneOfDescriptorWrap FieldDescriptorWrap::as_variant() const {
        return { _cur_file, _descriptor->containing_oneof(), *_writer };
    }

    std::string FieldDescriptorWrap::resolve_cpp_ref_t_in_message() const {
        if (is_variant()) {
            return as_variant().cpp_ref_t();
        }
        return resolve_cpp_ref_t_in_variant();
    }

    std::string FieldDescriptorWrap::resolve_cpp_builder_t_inner() const {
        using gpfd = gp::FieldDescriptor;
        resolve_precondition();

        std::string result;
        switch (_descriptor->type()) {
            case gpfd::TYPE_MESSAGE:
                result = as_message().cpp_builder_t();
                break;
            case gpfd::TYPE_ENUM:
                result = as_enum().cpp_t();
                break;
            case gpfd::TYPE_STRING:
                result = "std::string";
                break;
            case gpfd::TYPE_BYTES:
                result = "std::string";
                break;
            default:
                result = resolve_cpp_type_common();
        }
        return result;
    }

    std::string FieldDescriptorWrap::resolve_cpp_builder_t_in_variant() const {
        std::string result = resolve_cpp_builder_t_inner();
        if (!is_repeated())
            return result;

        if (is_message())
            return do_sformat("::yazik::compiler::support::VecEntityBuilder<{}>", result);
        else
            return do_sformat("::yazik::compiler::support::VecBuilder<{}>", result);
    }

    std::string FieldDescriptorWrap::resolve_cpp_builder_t_in_message() const {
        if (is_variant())
            return as_variant().cpp_builder_t();

        return resolve_cpp_builder_t_in_variant();
    }

    std::string FieldDescriptorWrap::resolve_cpp_layered_builder_t() const {
        if (is_message()) {
            if (is_repeated()) {
                auto message = as_message();
                return do_sformat(
                    "::yazik::compiler::support::VecEntityLayeredBuilder<{}, Self>",
                    message.cpp_layered_builder_t("")
                );
            } else {
                return as_message().cpp_layered_builder_t("Self");
            }
        } else if (is_repeated()) {
            return do_sformat(
                "::yazik::compiler::support::VecLayeredBuilder<{}, Self>",
                resolve_cpp_builder_t_inner()
            );
        }
        throw runtime_fail(
            "layered builder not supported for: {}",
            _descriptor->DebugString()
        );
    }

    std::string FieldDescriptorWrap::resolve_cpp_layered_inner_builder_t() const {
        if (is_message()) {
            return as_message().cpp_layered_builder_t("");
        } else if (is_repeated()) {
            return resolve_cpp_builder_t_inner();
        }
        throw runtime_fail(
            "layered inner builder not supported for: {}",
            _descriptor->DebugString()
        );
    }

    std::string FieldDescriptorWrap::resolve_cpp_ref_t_in_variant() const {
        using gpfd = gp::FieldDescriptor;
        resolve_precondition();

        std::string result;
        switch (_descriptor->type()) {
        case gpfd::TYPE_MESSAGE:
            result = as_message().cpp_ref_t();
            break;
        case gpfd::TYPE_ENUM:
            result = as_enum().cpp_t();
            break;
        case gpfd::TYPE_STRING:
            result = "::yazik::string_view";
            break;
        case gpfd::TYPE_BYTES:
            result = "::yazik::string_view";
            break;
        default:
            result = resolve_cpp_type_common();
        }
        if (!_descriptor->is_repeated()) return result;
        else return do_sformat("::yazik::compiler::support::repeated_type_t<{}>", result);
    }

    ServiceDescriptorWrap MethodDescriptorWrap::service() const {
        return { _cur_file, _descriptor->service(), *_writer };
    }

    void FieldDescriptorWrap::cpp_unwrap_repeated() {
        using gpfd = gp::FieldDescriptor;
        auto* field = _descriptor;
        auto& w = *_writer;
        std::string getter = cpp_in_variant_getter();
        if (field->message_type()) {
            w.w("return ::ranges::views::all(*p->mutable_{}())", getter);
        } else {
            w.w("return ::ranges::views::all(p->{}())", getter);
        }
        switch (field->type()) {
        case gpfd::TYPE_MESSAGE:
            w.l().with_indent([&] {
                auto msg = as_message();
                w.w("| ::ranges::views::transform([]({}& x) ", msg.cpp_pb_t())
                    .braced_detail([&] {
                        w.wl("return {}::wrap(x);", msg.cpp_pb_spec_t());
                    }, false)
                    .wl(");");
            });
            break;
        case gpfd::TYPE_ENUM:
            w.l().with_indent([&] {
                std::string enum_type = as_enum().cpp_t();
                w.w("| ::ranges::views::transform([](int x) ")
                    .braced_detail([&] {
                        w.wl("return create_enum<{}>((int)x);", enum_type);
                    }, false)
                    .wl(");");
            });
            break;
        case gpfd::TYPE_STRING:
        case gpfd::TYPE_BYTES:
            w.l().with_indent([&] {
                w.w("| ::ranges::views::transform([](const auto& x) ")
                    .braced_detail([&] {
                        w.wl("return ::yazik::string_view {{ x }};");
                    }, false)
                    .wl(");");
            });
            break;
        default:
            w.wl(";");
            break;
        }
    }

    void FieldDescriptorWrap::cpp_unwrap_in_variant() {
        using gpfd = gp::FieldDescriptor;
        auto* field = _descriptor;
        auto& w = *_writer;
        std::string getter = cpp_in_variant_getter();
        if (_descriptor->is_repeated()) {
            cpp_unwrap_repeated();
            return;
        }
        switch (field->type()) {
        case gpfd::TYPE_MESSAGE:
            w.wl(
                "return {}::wrap(*(({}*)p)->mutable_{}());",
                as_message().cpp_pb_spec_t(),
                parent().cpp_pb_t(),
                getter
            );
            break;
        case gpfd::TYPE_ENUM:
            w.wl("return create_enum<{}>((int)p->{}());", as_enum().cpp_t(), getter);
            break;
        default:
            w.wl("return p->{}();", getter);
            break;
        }
    }

    void FieldDescriptorWrap::cpp_unwrap_in_message() {
        if (!is_variant()) {
            cpp_unwrap_in_variant();
            return;
        }

        _writer->wl("return {}::wrap(*p);", as_variant().cpp_pb_spec_t());
    }

    DescriptorWrap FieldDescriptorWrap::parent() const {
        return { _cur_file, _descriptor->containing_type(), *_writer };
    }

    void FieldDescriptorWrap::cpp_unwrap_builder_repeated() {
        using gpfd = gp::FieldDescriptor;
        auto* field = _descriptor;
        auto& w = *_writer;
        auto builder = resolve_cpp_builder_t_in_variant();
        auto getter = cpp_in_variant_getter();
        switch (field->type()) {
            case gpfd::TYPE_MESSAGE:
                w.w("return create_builder<{0}>", builder);
                w.braced_detail(
                    [&] {
                        w.wl("p,");
                        w.w("[](void* ptr) -> void* ");
                        w.braced_detail([&]{
                            w.wl(
                                "return (void*)(({}*)ptr)->mutable_{}()->Add();",
                                parent().cpp_pb_t(),
                                getter
                            );
                        }, false).wl(",");
                        w.wl("{}::vtable()", as_message().cpp_builder_pb_spec_t());
                    },
                    false, "(", ")"
                ).wl(";");
            break;
            default:
                w.w("return create_builder<{0}>", builder);
                w.braced_detail(
                    [&] {
                        w.wl("p,");
                        w.w("[](void* ptr, auto value) ", builder);
                        w.braced([&]{
                            if (is_enum()) {
                                w.wl(
                                    "(({}*)ptr)->add_{}(({})value.which());",
                                    parent().cpp_pb_t(),
                                    getter,
                                    as_enum().cpp_pb_t()
                                );
                            } else {
                                w.wl(
                                    "(({}*)ptr)->add_{}(std::move(value));",
                                    parent().cpp_pb_t(),
                                    getter
                                );
                            }
                        });
                    },
                    false, "(", ")"
                ).wl(";");
            break;
        }
    }

    void FieldDescriptorWrap::cpp_unwrap_builder_in_variant() {
        using gpfd = gp::FieldDescriptor;
        auto* field = _descriptor;
        auto& w = *_writer;
        auto builder = resolve_cpp_builder_t_in_message();
        auto getter = cpp_in_variant_getter();
        switch (field->type()) {
        case gpfd::TYPE_MESSAGE:
            w.wl(

                "auto builder = {}::wrap(*(({}*)p)->mutable_{}());",
                as_message().cpp_builder_pb_spec_t(),
                parent().cpp_pb_t(),
                getter
            );
            w.wl("clbk(builder);");
            break;
        case gpfd::TYPE_ENUM:
            w.wl(
                "p->set_{}(({})value);",
                getter,
                as_enum().cpp_pb_t()
            );
            break;
        default:
            w.wl("p->set_{}(std::move(value));", getter);
            break;
        }
    }

    void FieldDescriptorWrap::cpp_unwrap_builder_in_message() {
        using gpfd = gp::FieldDescriptor;
        auto* field = _descriptor;
        auto& w = *_writer;
        if (is_repeated()) {
            cpp_unwrap_builder_repeated();
            return;
        }
        if (is_variant()) {
            w.wl(
                "return {}::wrap(*p);",
                as_variant().cpp_builder_pb_spec_t()
            );
            return;
        }
        auto getter = cpp_in_variant_getter();
        switch (field->type()) {
        case gpfd::TYPE_MESSAGE:
            w.wl(
                "return {}::wrap(*(({}*)p)->mutable_{}());",
                as_message().cpp_builder_pb_spec_t(),
                parent().cpp_pb_t(),
                getter
            );
            break;
        case gpfd::TYPE_ENUM:
            w.wl(
                "p->set_{}(({})value.which());",
                getter,
                as_enum().cpp_pb_t()
            );
            break;
        default:
            w.wl("p->set_{}(std::move(value));", getter);
            break;
        }
    }

    std::string FieldDescriptorWrap::cpp_to_dynamic_in_message() {
        if (!is_repeated()) {
            if (is_message() || is_variant() || is_enum()) {
                return do_sformat("{}().as_dynamic()", cpp_in_message_getter());
            } else {
                return do_sformat("{}()", cpp_in_message_getter());
            }
        } else {
            if (is_message() || is_variant() || is_enum()) {
                return do_sformat(
                    "::yazik::compiler::support::repeated_ref_to_dynamic({}())",
                    cpp_in_message_getter()
                );
            } else {
                return do_sformat(
                    "::yazik::compiler::support::repeated_to_dynamic({}())",
                    cpp_in_message_getter()
                );
            }
        }
    }

    std::string FieldDescriptorWrap::cpp_to_dynamic_in_variant() {
        if (!is_repeated()) {
            if (is_message() || is_enum()) {
                return do_sformat("{}().as_dynamic()", cpp_in_variant_getter());
            } else {
                return do_sformat("{}()", cpp_in_variant_getter());
            }
        } else {
            if (is_message() || is_enum()) {
                return do_sformat(
                    "::yazik::compiler::support::repeated_ref_to_dynamic({}())",
                    cpp_in_variant_getter()
                );
            } else {
                return do_sformat(
                    "::yazik::compiler::support::repeated_to_dynamic({}())",
                    cpp_in_variant_getter()
                );
            }
        }
    }

    void EnumValueDescriptorWrap::add_to_chai(chaiscript::ChaiScript& script) {
        define_chai_class(
            script,
            "EnumValue",
            {
                ChaiMemberFn(name),
                ChaiMemberFn(snake_case_name),
                ChaiMemberFn(number),
            }
        );
    }

    void EnumDescriptorWrap::add_to_chai(chaiscript::ChaiScript& script) {
        define_chai_class(
            script,
            "Enum",
            {
                ChaiMemberFn(name),
                ChaiMemberFn(snake_case_name),
                ChaiMemberFn(ns),
                ChaiMemberFn(cpp_namespaced),
                ChaiMemberFn(values),
            }
        );
    }

    void FieldDescriptorWrap::add_to_chai(chaiscript::ChaiScript& script) {
        define_chai_class(
            script,
            "Field",
            {
                ChaiMemberFn(camel_case_name),
                ChaiMemberFn(cpp_in_variant_getter),
                ChaiMemberFn(cpp_in_message_getter),
                ChaiMemberFn(resolve_cpp_ref_t_in_variant),
                ChaiMemberFn(resolve_cpp_ref_t_in_message),
                ChaiMemberFn(resolve_cpp_builder_t_in_variant),
                ChaiMemberFn(resolve_cpp_builder_t_in_message),
                ChaiMemberFn(resolve_cpp_layered_builder_t),
                ChaiMemberFn(resolve_cpp_layered_inner_builder_t),
                ChaiMemberFn(cpp_unwrap_in_variant),
                ChaiMemberFn(cpp_unwrap_in_message),
                ChaiMemberFn(cpp_unwrap_builder_in_variant),
                ChaiMemberFn(cpp_unwrap_builder_in_message),
                ChaiMemberFn(cpp_to_dynamic_in_message),
                ChaiMemberFn(cpp_to_dynamic_in_variant),
                ChaiMemberFn(is_repeated),
                ChaiMemberFn(is_message),
                ChaiMemberFn(as_message),
                ChaiMemberFn(is_variant),
            }
        );
    }

    void OneOfDescriptorWrap::add_to_chai(chaiscript::ChaiScript& script) {
        define_chai_class(
            script,
            "Variant",
            {
                ChaiMemberFn(name),
                ChaiMemberFn(snake_case_name),
                ChaiMemberFn(camel_case_upper_name),
                ChaiMemberFn(cpp_namespaced),
                ChaiMemberFn(fields),
                ChaiMemberFn(parent),
            }
        );
    }

    void DescriptorWrap::add_to_chai(chaiscript::ChaiScript& script) {
        define_chai_class(
            script,
            "Message",
            {
                ChaiMemberFn(name),
                ChaiMemberFn(snake_case_name),
                ChaiMemberFn(ns),
                ChaiMemberFn(cpp_namespaced),
                ChaiMemberFn(cpp_ref_t),
                ChaiMemberFn(cpp_builder_t),
                ChaiMemberFn(cpp_layered_builder_t),
                ChaiMemberFn(cpp_pb_t),
                ChaiMemberFn(cpp_pb_spec_t),
                ChaiMemberFn(cpp_builder_pb_spec_t),
                ChaiMemberFn(enums),
                ChaiMemberFn(nested),
                ChaiMemberFn(variants),
                ChaiMemberFn(fields),
            }
        );
    }

    void MethodDescriptorWrap::add_to_chai(chaiscript::ChaiScript& script) {
        define_chai_class(
            script,
            "Method",
            {
                ChaiMemberFn(name),
                ChaiMemberFn(snake_case_name),
                ChaiMemberFn(cpp_namespaced),
                ChaiMemberFn(service),
                ChaiMemberFn(input),
                ChaiMemberFn(output),
                ChaiMemberFn(is_supported),
                ChaiMemberFn(is_server_streaming),
                ChaiMemberFn(is_client_streaming),
                ChaiMemberFn(cpp_rpc_type_sync),
                ChaiMemberFn(cpp_rpc_call_response_sync),
                ChaiMemberFn(cpp_rpc_call_cast_as_broken_sts_sync),
                ChaiMemberFn(cpp_rpc_call_request_sync),
                ChaiMemberFn(cpp_rpc_type_async),
                ChaiMemberFn(cpp_rpc_call_response_async),
                ChaiMemberFn(cpp_rpc_call_cast_as_broken_sts_async),
                ChaiMemberFn(cpp_rpc_call_request_async),
            }
        );
    }

    void ServiceDescriptorWrap::add_to_chai(chaiscript::ChaiScript& script) {
        define_chai_class(
            script,
            "Service",
            {
                ChaiMemberFn(name),
                ChaiMemberFn(snake_case_name),
                ChaiMemberFn(cpp_namespaced),
                ChaiMemberFn(methods),
            }
        );
    }

    void FileDescriptorWrap::add_to_chai(chaiscript::ChaiScript& script) {
        define_chai_class(
            script,
            "File",
            {
                ChaiMemberFn(file_name),
                ChaiMemberFn(has_deps),
                ChaiMemberFn(deps),
                ChaiMemberFn(enums),
                ChaiMemberFn(messages),
                ChaiMemberFn(services),
                ChaiMemberFn(is_supported),
                ChaiMemberFn(has_rpc),
                ChaiMemberFn(cpp_namespaced),
            }
        );
    }

    void add_decls_to_chai(
        chaiscript::ChaiScript& script,
        const gp::FileDescriptor* file,
        FileWriter& writer
    ) {
        using namespace chaiscript;
        writer.add_to_chai(script);

        script.add(fun(&q), "q");
        script.add(fun(&ab), "ab");
        script.add(fun(&qb), "qb");
        script.add(fun(&strip_proto), "strip_proto");
        script.add(fun(&replace_proto),"replace_proto");
        script.add(fun(&camel_case), "camel_case");
        script.add(fun(&camel_case_upper), "camel_case_upper");
        script.add(fun(&snake_case), "snake_case");
        script.add(fun(&to_cpp_ns), "to_cpp_ns");

        script.add(bootstrap::standard_library::vector_type<std::vector<std::string>>("std_vector_string"));
	    script.add(chaiscript::vector_conversion<std::vector<std::string>>());

	    script.add_global_const(
            const_var(std::string{"::yazik::compiler::support::Initializer"}),
            "cpp_initializer"
        );
        script.add_global_const(
            const_var(std::string{"::yazik::string_view"}),
            "cpp_string_view"
        );
        script.add_global_const(
            const_var(std::string{"std::string"}),
            "cpp_string"
        );
        script.add(
            fun([](
                const std::string& ret,
                const std::vector<std::string>& args
            ) -> std::string {
                return do_sformat(
                    "::yazik::unique_function<{} ({})>",
                    ret, args
                        | views::join(std::string { ", " })
                        | ranges::to<std::string>()
                );
            }),
            "cpp_fun"
        );
        script.add_global_const(
            const_var(std::string{"::yazik::concurrency::scheduler_ptr_t"}),
            "cpp_scheduler"
        );
        script.add_global_const(
            const_var(std::string{"std::string"}),
            "cpp_serialized_buffer"
        );
        script.add_global_const(
            const_var(std::string{"::yazik::string_view"}),
            "cpp_deserialized_buffer"
        );
        script.add_global_const(
            const_var(std::string{"!boost::di::aux::is_complete<Fn>::value || "}),
            "cpp_rpc_concept_pref"
        );
        script.add_global_const(
            const_var(std::string{"::yazik::rpc::c_stream_writer_sync_unit"}),
            "cpp_rpc_concept_ss_sync"
        );
         script.add_global_const(
            const_var(std::string{"::yazik::rpc::c_stream_writer_async_unit"}),
            "cpp_rpc_concept_ss_async"
        );
        script.add_global_const(
            const_var(std::string{"::yazik::rpc::c_stream_reader_sync_unit"}),
            "cpp_rpc_concept_cs_sync"
        );
         script.add_global_const(
            const_var(std::string{"::yazik::rpc::c_stream_reader_async_unit"}),
            "cpp_rpc_concept_cs_async"
        );
        script.add_global_const(
            const_var(std::string{"::yazik::rpc::c_unary_sync_unit"}),
            "cpp_rpc_concept_unary_sync"
        );
        script.add_global_const(
            const_var(std::string{"::yazik::rpc::c_unary_async_unit"}),
            "cpp_rpc_concept_unary_async"
        );
        script.add_global_const(
            const_var(std::string{"::yazik::rpc::c_stream_reader_client"}),
            "cpp_rpc_concept_ss_client"
        );
        script.add_global_const(
            const_var(std::string{"::yazik::rpc::c_stream_writer_client"}),
            "cpp_rpc_concept_cs_client"
        );
        script.add_global_const(
            const_var(std::string{"::yazik::rpc::c_rpc_unary_client"}),
            "cpp_rpc_concept_unary_client"
        );
        script.add(
            fun([](const std::string& templ) -> std::string {
                return do_sformat("::yazik::rpc::RespOk<{}>", templ);
            }),
            "cpp_rpc_resp_ok"
        );
        script.add_global_const(
            const_var(std::string{"::yazik::rpc::RpcStatus::unimplemenmted()"}),
            "cpp_rpc_status_unimplemented"
        );

        EnumValueDescriptorWrap::add_to_chai(script);
        EnumDescriptorWrap::add_to_chai(script);
        FieldDescriptorWrap::add_to_chai(script);
        OneOfDescriptorWrap::add_to_chai(script);
        DescriptorWrap::add_to_chai(script);
        MethodDescriptorWrap::add_to_chai(script);
        ServiceDescriptorWrap::add_to_chai(script);
        FileDescriptorWrap::add_to_chai(script);
        script.add_global_const(
            const_var(FileDescriptorWrap{file, file, writer}),
            "m"
        );
    }

} // end of ::yazik::compiler namespace