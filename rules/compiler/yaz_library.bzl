load(":run_proto_compiler.bzl", "run_proto_compiler")

_s_cc_proto_exts = [
    ".pb.h",
    ".pb.cc",
]

_s_cc_proto_yaz_exts = [
    ".yaz.h",
    ".yaz.pb.h",
]
_s_cc_out = [
    "cpp_out",
]
_s_cc_yaz_out = [
    "yaz_cpp_out",
]

_s_rpc_cc_proto_exts = [
    ".grpc.pb.h",
    ".grpc.pb.cc",
]
_s_rpc_cc_proto_yaz_exts = [
    ".yaz.rpc.h",
    ".yaz.grpc.h",
]

_s_rpc_cc_out = [
    "grpc_out",
]
_s_rpc_cc_yaz_out = [
    "yaz_grpc_cpp_out",
]

def _grpc_cc_gen_add_args(spec, plugin):
    spec.args.add("--plugin=protoc-gen-grpc=%s" % plugin.path)
    spec.tools.append(plugin)

def _grpc_cc_gen_impl(ctx):
    gen_sources = []

    exts = _s_cc_proto_exts
    outs = _s_cc_out

    if not ctx.attr.pb_only:
        exts = exts + _s_cc_proto_yaz_exts
        outs = outs + _s_cc_yaz_out

    add_args = None
    if not ctx.attr.proto_only:
        exts = exts + _s_rpc_cc_proto_exts
        outs = outs + _s_rpc_cc_out

        if not ctx.attr.pb_only:
            exts = exts + _s_rpc_cc_proto_yaz_exts
            outs = outs + _s_rpc_cc_yaz_out

        add_args = ( _grpc_cc_gen_add_args, ctx.executable._plugin )

    for src in ctx.attr.srcs:
        gen_sources += run_proto_compiler(
            ctx,
            src[ProtoInfo],
            exts,
            outs,
            add_args,
        )

    return DefaultInfo(files = depset(gen_sources))

_grpc_cc_gen = rule(
    attrs = {
        "srcs": attr.label_list (
            mandatory = True,
            providers = [
                ProtoInfo,
            ],
        ),
        "proto_only": attr.bool(mandatory = True),
        "pb_only": attr.bool(mandatory = True),
        "_compiler": attr.label(
            default = "@build_flare_yazik//src/yazik/compiler/bin",
            executable = True,
            cfg = "host",
        ),
        "_plugin": attr.label(
            default = "@com_github_grpc_grpc//src/compiler:grpc_cpp_plugin",
            executable = True,
            cfg = "host",
        ),
    },
    implementation = _grpc_cc_gen_impl,
)

def _remove_cc(name):
    if not name.find("_cc_"):
        fail("%s name not contains _cc_")

    return name.replace("_cc_", "_")

def yaz_library(name, srcs, deps, pb_only, proto_only, visibility, tags):
    gen_srcs_label = "_%s_gen_srcs" % name
    _grpc_cc_gen(
        name = gen_srcs_label,
        srcs = srcs,
        proto_only = proto_only,
        pb_only = pb_only,
    )
    gen_srcs_label_rel = ":%s" % gen_srcs_label

    if proto_only:
        if pb_only:
            runtime_deps = [ "@com_google_protobuf//:protobuf" ]
        else:
            runtime_deps = [ "@build_flare_yazik//src/yazik/compiler/support:support", ]
    else:
        if pb_only:
            runtime_deps = [ "@com_github_grpc_grpc//:grpc++_codegen_proto" ]
        else:
            runtime_deps = [ "@build_flare_yazik//src/yazik/compiler/support:grpc_support", ]

    native.cc_library(
        name = name,
        srcs = [ gen_srcs_label_rel ],
        hdrs = [ gen_srcs_label_rel ],
        deps = deps + runtime_deps,
        includes = [ "_virtual_imports/%s_proto/" % name ],
        tags = tags,
        visibility = visibility,
    )