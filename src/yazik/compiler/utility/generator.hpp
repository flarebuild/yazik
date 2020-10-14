#pragma once

#include <yazik/compiler/common.hpp>

namespace yazik::compiler {

    class Generator
    : public gpc::CodeGenerator {
    public:

        bool Generate(
            const gp::FileDescriptor* file,
            const std::string& parameter,
            gpc::GeneratorContext* generator_context,
            std::string* error
        ) const final;

        virtual Result<bool> generate(
            const gp::FileDescriptor* file,
            const std::string& parameter,
            gpc::GeneratorContext* generator_context
        ) const noexcept = 0;

    };

    using generator_ptr_t = std::shared_ptr<gpc::CodeGenerator>;

} // end of ::yazik::compiler namespace