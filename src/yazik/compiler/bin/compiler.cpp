#include <google/protobuf/compiler/command_line_interface.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>

#include <yazik/compiler/common.hpp>
#include <yazik/compiler/generators/cpp_gen.hpp>
#include <yazik/compiler/generators/cpp_rpc_gen.hpp>

#include "tools/cpp/runfiles/runfiles.h"

using namespace yazik;
using namespace yazik::compiler;

using ::bazel::tools::cpp::runfiles::Runfiles;

int main(int argc, char* argv[], char **envp) {

    std::string error;
    std::unique_ptr<Runfiles> runfiles(Runfiles::Create(argv[0], &error));

    if (!error.empty()) {
        std::cout << error << std::endl;
        return 1;
    }

    auto cli = gpc::CommandLineInterface {};
    cli.AllowPlugins("protoc-");

    gpc::cpp::CppGenerator cpp_gen;
    cli.RegisterGenerator(
        "--cpp_out",
        "--cpp_opt",
        &cpp_gen,
        "Generate C++ header and source."
    );

    auto yaz_cpp_gen = create_cpp_generator(runfiles.get());
    cli.RegisterGenerator(
        "--yaz_cpp_out",
        "--yaz_cpp_opt",
        yaz_cpp_gen.get(),
        "Generate yazik cpp wrappers"
    );

    auto yaz_cpp_rpc_gen = create_cpp_rpc_generator(runfiles.get());
    cli.RegisterGenerator(
        "--yaz_grpc_cpp_out",
        "--yaz_grpc_cpp_opt",
        yaz_cpp_rpc_gen.get(),
        "Generate yazik grpc cpp wrappers"
    );

    return cli.Run(argc, argv);
}