#!/bin/bash
set -x
base_dir="$(cd "$(dirname "$0")" && pwd)"
workspace_dir="$base_dir"/../../
pwd
pushd "$workspace_dir"
bazel build @com_github_grpc_grpc//src/compiler:grpc_cpp_plugin --config debug
bazel run //src/yazik/compiler/bin  \
  --config debug \
  -- \
  --proto_path="$base_dir"/../ \
  --cpp_out="$base_dir" \
  --yaz_cpp_out="$base_dir" \
  --grpc_out="$base_dir" \
  --plugin=protoc-gen-grpc="$workspace_dir"/bazel-bin/external/com_github_grpc_grpc/src/compiler/grpc_cpp_plugin \
  --yaz_grpc_cpp_out="$base_dir" \
  "$base_dir"/../book/sample.proto
popd