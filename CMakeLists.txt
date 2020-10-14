cmake_minimum_required(VERSION 3.15)
project(example)

execute_process(
    COMMAND
    bazel run //:cmake_gen
    RESULT_VARIABLE ret
)
if(NOT ret EQUAL "0")
    message( FATAL_ERROR "Bad bazel exit status")
endif()

execute_process(COMMAND bazel info output_base OUTPUT_VARIABLE output_base OUTPUT_STRIP_TRAILING_WHITESPACE)
set(llvm_toolchain_dir  "${output_base}/external/llvm_toolchain")

set(CMAKE_CXX_STANDARD  20)
set(CMAKE_C_COMPILER    "${llvm_toolchain_dir}/bin/clang")
set(CMAKE_CXX_COMPILER  "${llvm_toolchain_dir}/bin/clang++")
set(CMAKE_AR            "${llvm_toolchain_dir}/bin/llvm-ar")
set(CMAKE_LINKER        "${llvm_toolchain_dir}/bin/llvm-ld")
set(CMAKE_NM            "${llvm_toolchain_dir}/bin/llvm-nm")
set(CMAKE_OBJDUMP       "${llvm_toolchain_dir}/bin/llvm-objdump")
set(CMAKE_RANLIB        "${llvm_toolchain_dir}/bin/llvm-ranlib")

add_definitions(-DDEBUG)
add_definitions(-DGRPC_BAZEL_BUILD)
add_compile_options(-Wno-ambiguous-reversed-operator)

include(cmake_gen/all_deps.cmake)
include(cmake_gen/all.cmake)

