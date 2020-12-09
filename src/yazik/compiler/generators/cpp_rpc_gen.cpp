#include "cpp_rpc_gen.hpp"
#include <google/protobuf/descriptor.pb.h>
#include <yazik/compiler/support/rpc_support.hpp>
#include <yazik/compiler/utility/utility.hpp>
#include <yazik/compiler/utility/writer.hpp>

#include <chaiscript/chaiscript.hpp>

namespace yazik::compiler {

    using ::bazel::tools::cpp::runfiles::Runfiles;

    class CppGrpcGenerator final
    : public Generator {

        Runfiles* _rf;

        Result<bool> generate(
            const gp::FileDescriptor* file,
            const std::string& parameter,
            gpc::GeneratorContext* generator_context
        ) const noexcept override {
            {
                auto w = FileWriter { file, generator_context, "yaz.rpc.h" };
                chaiscript::ChaiScript script;
                add_decls_to_chai(script, file, w);
                auto script_loc = _rf->Rlocation(
                    "build_flare_yazik/src/yazik/compiler/generators/cpp_yaz_rpc_gen_h.chai"
                );
                script.eval_file(script_loc);
            } {
                auto w = FileWriter { file, generator_context, "yaz.rpc.client.h" };
                chaiscript::ChaiScript script;
                add_decls_to_chai(script, file, w);
                auto script_loc = _rf->Rlocation(
                    "build_flare_yazik/src/yazik/compiler/generators/cpp_yaz_rpc_client_gen_h.chai"
                );
                script.eval_file(script_loc);
            } {
                auto w = FileWriter { file, generator_context, "yaz.grpc.h" };
                chaiscript::ChaiScript script;
                add_decls_to_chai(script, file, w);
                auto script_loc = _rf->Rlocation(
                    "build_flare_yazik/src/yazik/compiler/generators/cpp_yaz_grpc_gen_h.chai"
                );
                script.eval_file(script_loc);
            } {
                auto w = FileWriter { file, generator_context, "yaz.grpc.client.h" };
                chaiscript::ChaiScript script;
                add_decls_to_chai(script, file, w);
                auto script_loc = _rf->Rlocation(
                    "build_flare_yazik/src/yazik/compiler/generators/cpp_yaz_grpc_client_gen_h.chai"
                );
                script.eval_file(script_loc);
            }
            co_return true;
        }

    public:

        CppGrpcGenerator(Runfiles* rf)
        : _rf { rf }
        {}

    };

    generator_ptr_t create_cpp_rpc_generator(Runfiles* rf) {
        return std::make_shared<CppGrpcGenerator>(rf);
    }

} // end of ::yazik::compiler namespace