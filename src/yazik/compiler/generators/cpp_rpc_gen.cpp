#include "cpp_rpc_gen.hpp"
#include <google/protobuf/descriptor.pb.h>
#include <yazik/compiler/support/rpc_support.hpp>
#include <yazik/compiler/utility/writer.hpp>
#include <yazik/compiler/formatters/yaz_formatter.hpp>

#include <chaiscript/chaiscript.hpp>

namespace yazik::compiler {

    class CppGrpcGenerator final
    : public Generator {

        Result<bool> generate(
            const gp::FileDescriptor* file,
            const std::string& parameter,
            gpc::GeneratorContext* generator_context
        ) const noexcept override {
            {
                auto w = FileWriter { file, generator_context, "yaz.rpc.h" };
                chaiscript::ChaiScript script;
                add_decls_to_chai(script, file, w);
                script.eval_file("src/yazik/compiler/generators/cpp_yaz_rpc_gen_h.chai");
            } {
                auto w = FileWriter { file, generator_context, "yaz.grpc.h" };
                chaiscript::ChaiScript script;
                add_decls_to_chai(script, file, w);
                script.eval_file("src/yazik/compiler/generators/cpp_yaz_grpc_gen_h.chai");
            }
            co_return true;
        }
    };

    generator_ptr_t create_cpp_rpc_generator() {
        return std::make_shared<CppGrpcGenerator>();
    }

} // end of ::yazik::compiler namespace