#pragma once

#include <google/protobuf/compiler/code_generator.h>

#include <yazik/utility/result.hpp>
#include <yazik/utility/lambda_continuation.hpp>

namespace yazik::compiler {

    namespace gp = ::google::protobuf;
    namespace gpc = gp::compiler;

} // end of ::yazik::compiler namespace