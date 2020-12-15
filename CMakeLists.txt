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


function(JOIN OUTPUT GLUE)
    set(_TMP_RESULT "")
    set(_GLUE "") # effective glue is empty at the beginning
    foreach(arg ${ARGN})
        set(_TMP_RESULT "${_TMP_RESULT}${_GLUE}${arg}")
        set(_GLUE "${GLUE}")
    endforeach()
    set(${OUTPUT} "${_TMP_RESULT}" PARENT_SCOPE)
endfunction()

set(
    cxx_flags_list

    "-nostdinc++"
    "-fvisibility=hidden"
    "-fvisibility=hidden"
    "-fvisibility-inlines-hidden"
    "-fdata-sections"
    "-ffunction-sections"
    "-I${llvm_toolchain_dir}/include/c++/v1/"
    "-I${llvm_toolchain_dir}/lib/clang/11.0.0/include/"
    "-I${llvm_toolchain_dir}/lib64/clang/11.0.0/include/"
)

SET(BUILD_SHARED_LIBRARIES OFF)

if(UNIX AND NOT APPLE)
    add_definitions(-DFLARE_CMAKE_LINUX_BUILD)

    add_link_options(
        -fuse-ld=lld
        -nostdlib++
        -Wl,--allow-multiple-definition
        -Wl,--no-as-needed
        -Wl,--no-undefined
        -Wl,-z,relro,-z,now

        -L${llvm_toolchain_dir}/lib
        -Wl,-rpath,${llvm_toolchain_dir}/lib
        -L${llvm_toolchain_dir}/lib/clang/11.0.0/lib/linux

        -l:libc++.a
        -l:libc++abi.a
        -l:libunwind.a
        # Compiler runtime features.
        -rtlib=compiler-rt
        # To support libunwind.
        -lpthread
        -ldl
    )
else()
    add_link_options(
        -lc++
        -lc++abi
        -headerpad_max_install_names
        -undefined
        dynamic_lookup
    )
endif()

join(cxx_flags_str " " ${cxx_flags_list} )
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${cxx_flags_str})

add_definitions(-DGRPC_BAZEL_BUILD)
add_compile_options(
    -Wno-ambiguous-reversed-operator
    -Wno-return-stack-address
    -Wno-deprecated-declarations
    -Wno-non-c-typedef-for-linkage
)

include(cmake_gen/all_deps.cmake)
include(cmake_gen/all.cmake)

