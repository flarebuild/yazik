#include "cpp_gen.hpp"

#include <google/protobuf/descriptor.pb.h>
#include <yazik/compiler/utility/utility.hpp>
#include <yazik/compiler/utility/writer.hpp>

namespace yazik::compiler {

    using ::bazel::tools::cpp::runfiles::Runfiles;

    class CppGenerator final
    : public Generator {

        Runfiles* _rf;

        Result<bool> generate(
            const gp::FileDescriptor* file,
            const std::string& parameter,
            gpc::GeneratorContext* generator_context
        ) const noexcept override {
            {
                chaiscript::ChaiScript script;
                FileWriter w{ file, generator_context, "yaz.h"};
                add_decls_to_chai(script, file, w);
                auto script_loc = _rf->Rlocation(
                    "build_flare_yazik/src/yazik/compiler/generators/cpp_yaz_gen_h.chai"
                );
                std::cout << script_loc << std::endl;
                script.eval_file(script_loc);
            } {
                chaiscript::ChaiScript script;
                FileWriter w{ file, generator_context, "yaz.cc"};
                add_decls_to_chai(script, file, w);
                auto script_loc = _rf->Rlocation(
                    "build_flare_yazik/src/yazik/compiler/generators/cpp_yaz_gen_cpp.chai"
                );
                script.eval_file(script_loc);
            } {
                chaiscript::ChaiScript script;
                FileWriter w{ file, generator_context, "yaz.pb.h"};
                add_decls_to_chai(script, file, w);
                auto script_loc = _rf->Rlocation(
                    "build_flare_yazik/src/yazik/compiler/generators/cpp_yaz_pb_gen_h.chai"
                );
                script.eval_file(script_loc);
            } {
                chaiscript::ChaiScript script;
                FileWriter w{ file, generator_context, "yaz.pb.cc"};
                add_decls_to_chai(script, file, w);
                auto script_loc = _rf->Rlocation(
                    "build_flare_yazik/src/yazik/compiler/generators/cpp_yaz_pb_gen_cpp.chai"
                );
                script.eval_file(script_loc);
            }

            co_return true;
        }

    public:

        CppGenerator(Runfiles* rf)
        : _rf { rf }
        {}

    };

    generator_ptr_t create_cpp_generator(Runfiles* rf) {
        return std::make_shared<CppGenerator>(rf);
    }

} // end of ::yazik::compiler namespace