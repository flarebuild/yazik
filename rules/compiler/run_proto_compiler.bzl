load(":utils.bzl", "vimp_path_src_with_ext_replace")

def _to_path(f):
    return f.path

def run_proto_compiler(ctx, proto_info, exts, out_defs, add_args = None, env = None):
    sources = proto_info.direct_sources
    if not sources:
        fail("can't run protoc: no sources")

    spec = struct(
        args = ctx.actions.args(),
        tools = [],
    )
    spec.args.add_all(proto_info.transitive_proto_path, format_each = "--proto_path=%s")
    spec.args.add_all(sources, map_each = _to_path)
    spec.args.add_all(out_defs, format_each = "--%s=" + proto_info.proto_source_root)

    if add_args:
        add_args[0](spec, add_args[1])

    outputs = [
        ctx.actions.declare_file(vimp_path_src_with_ext_replace(ctx, src, ext))
        for src in sources
        for ext in exts
    ]

    ctx.actions.run(
        inputs = depset(
            proto_info.direct_sources,
            transitive = [proto_info.transitive_imports],
        ),
        outputs = outputs,
        executable = ctx.executable._compiler,
        arguments = [ spec.args ],
        env = env,
        tools = spec.tools,
    )

    return outputs