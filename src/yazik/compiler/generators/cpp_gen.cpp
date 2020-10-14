#include "cpp_gen.hpp"
#include "cpp_yaz_gen.hpp"
#include "cpp_pb_gen.hpp"

namespace yazik::compiler {

    using namespace ranges;

    class CppGenerator final
    : public Generator {

        Result<bool> generate(
            const gp::FileDescriptor* file,
            const std::string& parameter,
            gpc::GeneratorContext* generator_context
        ) const noexcept override {
            co_await generate_yaz(file, parameter, generator_context);
            co_await generate_yaz_pb(file, parameter, generator_context);

            co_return true;
        }
    };

    generator_ptr_t create_cpp_generator() {
        return std::make_shared<CppGenerator>();
    }

} // end of ::yazik::compiler namespace