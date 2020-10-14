#pragma once

#include <yazik/compiler/common.hpp>

namespace yazik::compiler {

    Result<bool> generate_yaz_pb(
        const gp::FileDescriptor* file,
        const std::string& parameter,
        gpc::GeneratorContext* generator_context
    ) noexcept;

} // end of ::yazik::compiler namespace