load("@bazel_skylib//lib:paths.bzl", "paths")
load("@bazel_skylib//lib:sets.bzl", "sets")

load("//rules/compiler:yaz_library.bzl", "yaz_library")
load("//rules/compiler:select_external_proto.bzl", "select_external_proto")

YazikConfig = provider(
    fields = [
        "cc",
        "grpc",
        "yaz",
    ],
)

def _proto_config(
    cc = True,
    grpc = True,
    yaz = True,
):
    return YazikConfig(
        cc = cc,
        grpc = grpc,
        yaz = yaz,
    )

def _library_macro(
    name,
    srcs,
    visibility,
    has_rpc,
    deps = [],
    ignore = [],
    strip_import_prefix = None,
    config = _proto_config(),
    tags = [],
    pb_only = False,
):
    not_ignored_deps = sets.to_list(sets.difference(
        sets.make(deps),
        sets.make(ignore),
    ))

    proto_label = "%s_proto" % name

    native.proto_library(
        name = proto_label,
        srcs = srcs,
        deps = [ "%s_proto" % dep for dep in deps ],
        strip_import_prefix = strip_import_prefix,
        visibility = visibility,
    )

    proto_src = ":%s" % proto_label

    if config.cc:
        if config.grpc:
            yaz_library(
                name = name,
                srcs = [ proto_src ],
                deps = deps,
                proto_only = not has_rpc,
                tags = tags,
                pb_only = pb_only,
                visibility = visibility,
            )

yazik = struct(
    config = _proto_config,
    library = _library_macro,
    select_external = select_external_proto,
)