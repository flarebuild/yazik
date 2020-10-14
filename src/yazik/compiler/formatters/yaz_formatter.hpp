#pragma once

#include <google/protobuf/descriptor.pb.h>
#include <yazik/compiler/utility/utility.hpp>

namespace yazik::compiler {

    class Formatter {
        const gp::FileDescriptor* _cur_fd;
        std::string _type_prefix;

        std::string ns(const gp::FileDescriptor* fd) const;
        std::string ns_pref(const gp::FileDescriptor* fd) const;
        std::string resolve_concept_type_common(const gp::FieldDescriptor* field) const;
        std::string yaz_type_name(std::string name) const;

    public:
        explicit Formatter(
            const gp::FileDescriptor* cur_fd,
            std::string type_prefix
        ) noexcept;

        template <Descriptable T>
        std::string pb_type_name_base(const T* value) const {
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
        std::string pb_type_name(const T* value) const {
            return do_sformat(
                "{}{}",
                ns_pref(value->file()),
                pb_type_name_base(value)
            );
        }

        template <Descriptable T>
        std::string type_name(const T* value) const {
            return do_sformat(
                "{}{}",
                ns_pref(value->file()),
                yaz_type_name(pb_type_name_base(value))
            );
        }

        std::string clbk_concept_name(const gp::FieldDescriptor* field) const;

        std::string resolve_builder_args_tuple(const gp::FieldDescriptor* field) const;
        std::string resolve_builder_inner_type(const gp::FieldDescriptor* field) const;
        std::string resolve_builder_type(const gp::FieldDescriptor* field) const;
        std::string resolve_type(const gp::FieldDescriptor* field) const;

        static const std::set<std::string> s_keywords;

        std::string getter_inner(const gp::FieldDescriptor* field) const;
        std::string getter(const gp::FieldDescriptor* field) const;
    };

} // end of ::yazik::compiler namespace