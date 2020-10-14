load("//rules/proto:collect_external.bzl", "YazikExternalProtosInfo")

def _select_external_proto_file_impl(ctx):
    sources = ctx.attr.info[YazikExternalProtosInfo].sources
    if not ctx.attr.file_path in sources:
        fail("unknown file path " + ctx.attr.file_path)

    return [ DefaultInfo(
        files = depset([ sources[ctx.attr.file_path] ]),
    ) ]

select_external_proto = rule(
    attrs = {
        "info": attr.label(mandatory = True),
        "file_path": attr.string(mandatory = True),
    },
    implementation = _select_external_proto_file_impl,
)