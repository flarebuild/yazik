#pragma once

#include <yazik/compiler/utility/generator.hpp>

#include "tools/cpp/runfiles/runfiles.h"

namespace yazik::compiler {

    generator_ptr_t create_cpp_rpc_generator(
        ::bazel::tools::cpp::runfiles::Runfiles* rf
    );

} // end of ::yazik::compiler namespace