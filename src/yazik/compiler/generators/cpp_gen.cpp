#include "cpp_gen.hpp"

#include <google/protobuf/descriptor.pb.h>
#include <yazik/compiler/utility/utility.hpp>
#include <yazik/compiler/utility/writer.hpp>

namespace yazik::compiler {

    using namespace ranges;

    class CppGenerator final
    : public Generator {

        Result<bool> generate(
            const gp::FileDescriptor* file,
            const std::string& parameter,
            gpc::GeneratorContext* generator_context
        ) const noexcept override {
            {
                chaiscript::ChaiScript script;
                FileWriter w{ file, generator_context, "yaz.h"};
                add_decls_to_chai(script, file, w);
                script.eval_file("src/yazik/compiler/generators/cpp_yaz_gen_h.chai");
            } {
                chaiscript::ChaiScript script;
                FileWriter w{ file, generator_context, "yaz.cc"};
                add_decls_to_chai(script, file, w);
                script.eval_file("src/yazik/compiler/generators/cpp_yaz_gen_cpp.chai");
            } {
                chaiscript::ChaiScript script;
                FileWriter w{ file, generator_context, "yaz.pb.h"};
                add_decls_to_chai(script, file, w);
                script.eval_file("src/yazik/compiler/generators/cpp_yaz_pb_gen_h.chai");
            } {
                chaiscript::ChaiScript script;
                FileWriter w{ file, generator_context, "yaz.pb.cc"};
                add_decls_to_chai(script, file, w);
                script.eval_file("src/yazik/compiler/generators/cpp_yaz_pb_gen_cpp.chai");
            }

            co_return true;
        }
    };

    generator_ptr_t create_cpp_generator() {
        return std::make_shared<CppGenerator>();
    }

} // end of ::yazik::compiler namespace