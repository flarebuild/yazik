#include <google/protobuf/descriptor.pb.h>
#include "generator.hpp"

namespace yazik::compiler {

    bool Generator::Generate(
        const gp::FileDescriptor* file,
        const std::string& parameter,
        gpc::GeneratorContext* generator_context,
        std::string* error
    ) const {
        if (file->syntax() != gp::FileDescriptor::SYNTAX_PROTO3) {
            *error = "only proto3 is supported";
            return false;
        }

        auto gen_res = generate(file, parameter, generator_context);
        if (!gen_res)
            *error = gen_res.error();

        if (!gen_res) {
            L::error(std::move(gen_res.error()));
        }

        return gen_res.value();
    }

} // end of ::yazik::compiler namespace