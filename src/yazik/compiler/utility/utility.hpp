#include <yazik/compiler/common.hpp>
#include <yazik/utility/concepts.hpp>
#include <range/v3/all.hpp>

namespace yazik::compiler {

    std::string q(const std::string& str);
    std::string ab(const std::string& str);
    std::string cb(const std::string& str);
    std::string strip_proto(const std::string& filename);
    std::string replace_proto(const std::string& filename, const std::string& ext);
    std::string camel_case(const std::string& str);
    std::string camel_case_upper(const std::string& str);
    std::string snake_case(const std::string& str);

    std::string to_cpp_ns(const std::string& ns);

    inline bool is_supported(const gp::FieldDescriptor* field) {
        // TODO: impl
        return !field->is_map();
    }

    inline bool is_factory_based(const gp::FieldDescriptor* field) {
        return field->message_type()
            || field->is_repeated()
            || field->containing_oneof();
    }

    inline bool need_pass(const gp::FieldDescriptor* field) {
        if (!is_factory_based(field))
            return false;

        if (auto* one_of = field->containing_oneof())
            return one_of->field(0) != field;

        return !is_supported(field);
    }

    inline bool is_supported(const gp::Descriptor* message) {
        // TODO: impl
        return message->map_key() == nullptr;
    }

    inline bool is_supported(const gp::MethodDescriptor* method) {
        // TODO: impl
        return !(method->client_streaming() && method->server_streaming());
    }

    template<typename Container>
    requires ::ranges::range<Container>
    std::string mkstring(
        const Container& cont,
        const std::string& start,
        const std::string& sep,
        const std::string& end
    ) {
        using namespace ranges;
        if (cont.empty())
            return "";

        std::string joined = "";
        for (auto it = cont.begin(); it != cont.end(); ++it) {
            joined += *it;
            auto it_cpy = it;
            ++it_cpy;
            if (it_cpy != cont.end())
                joined += sep;
        }
        return do_sformat(
            "{}{}{}",
            start,
            joined,
            end
        );
    }

    template<typename T>
    concept Descriptable = requires (const T* a) {
        { a->name() } -> concepts::c_just<std::string>;
        { a->file() } -> concepts::c_just<const gp::FileDescriptor*>;
        { a->containing_type() } -> concepts::c_just<const gp::Descriptor*>;
    };

    template<typename T>
    concept FieldHolder = requires (const T* a) {
        { a->name() } -> concepts::c_just<std::string>;
        { a->field(0) } -> concepts::c_just<const gp::FieldDescriptor*>;
    };


} // end of ::yazik::compiler namespace