load("@bazel_skylib//lib:paths.bzl", "paths")
load("@bazel_skylib//rules/private:copy_file_private.bzl", "copy_bash")

YazikAllProtoInfos = provider(
    fields = [
        "infos",
        "deps",
    ],
)

def _collect_all_infos_from_deps(deps):
    result = {}
    for dep in deps:
        if not YazikAllProtoInfos in dep:
            continue
        result.update(dep[YazikAllProtoInfos].infos)
    return result

def _collect_all_deps_labels_from_deps(deps):
    result = {}
    for dep in deps:
        if not YazikAllProtoInfos in dep:
            continue
        result.update(dep[YazikAllProtoInfos].deps)
    return result

def _collect_all_proto_infos_aspect_impl(target, ctx):
    if not ProtoInfo in target:
        fail("no ProtoInfo on target: " + target.label)

    deps = { target.label: [ dep.label for dep in ctx.rule.attr.deps ] }
    deps.update(_collect_all_deps_labels_from_deps(ctx.rule.attr.deps))
    infos = { target.label: target[ProtoInfo] }
    infos.update(_collect_all_infos_from_deps(ctx.rule.attr.deps))
    return YazikAllProtoInfos(
        infos = infos,
        deps = deps,
    )

_collect_all_proto_infos_aspect = aspect(
    attr_aspects = [
        "deps",
    ],
    implementation = _collect_all_proto_infos_aspect_impl,
)

YazikExternalProtosInfo = provider(
    fields = [
        "sources",
        "imports",
    ],
)

def _external_protos_impl(ctx):
    infos = _collect_all_infos_from_deps(ctx.attr.deps)
    deps = _collect_all_deps_labels_from_deps(ctx.attr.deps)

    sources = {}
    reverse_sources = {}

    for proto_info in infos.values():
        for source in proto_info.direct_sources:
            if not source.path.startswith(proto_info.proto_source_root):
                fail(source.path + " not related to root " + proto_info.proto_source_root)

            new_path = source.path[len(proto_info.proto_source_root) + 1:]
            copy = ctx.actions.declare_file(new_path)
            copy_bash(ctx, source, copy)
            sources[new_path] = copy
            reverse_sources[source] = new_path

    imports = {}
    for label, proto_info in infos.items():
        imports_list = []

        if not label in deps:
            fail("unknown label %s" % label)

        for dep in deps[label]:
            if not dep in infos:
                fail("unknown dep %s" % dep)

            for source in infos[dep].direct_sources:
                imports_list.append(reverse_sources[source])

        for source in proto_info.direct_sources:
            if not source in reverse_sources:
                fail("unknown source " + source.path)

            imports[reverse_sources[source]] = imports_list

    return [
        DefaultInfo(
            files = depset(sources.values()),
            runfiles = ctx.runfiles(files = sources.values()),
        ),
        YazikExternalProtosInfo(
            sources = sources,
            imports = imports,
        )
    ]

_external_protos = rule(
    attrs = {
        "deps": attr.label_list(
            mandatory = True,
            aspects = [
                _collect_all_proto_infos_aspect,
            ],
            providers = [
                ProtoInfo,
            ],
        ),
    },
    implementation = _external_protos_impl,
)

def _collect_external_protos_script_impl(ctx):
    gen_script = ctx.actions.declare_file("%s.py" % ctx.attr.out_file_name)
    protos = ctx.attr.info[YazikExternalProtosInfo]

    sources = {}
    for rel_path, file in protos.sources.items():
        sources[rel_path] = file.path

    ctx.actions.expand_template(
        template = ctx.file._template,
        output = gen_script,
        substitutions = {
            "__visibility__" : str(ctx.attr.build_files_visibility),
            "__info_target__" : ctx.attr.out_file_name + "_info",
            "__directory_location__" : paths.dirname(ctx.build_file_path),
            "__bin_dir__" : ctx.bin_dir.path,
            "__imports__" : str(protos.imports),
            "__ignore_non_proto3__" : str(ctx.attr.ignore_non_proto3),
            "__pass_tags__" : str(ctx.attr.pass_tags),
        },
    )
    return DefaultInfo(files = depset([gen_script]))

_collect_external_protos_script = rule(
    attrs = {
        "build_files_visibility": attr.string_list(
            mandatory = True,
        ),
        "out_file_name": attr.string(
            mandatory = True,
        ),
        "info": attr.label(
            mandatory = True,
            providers = [
                YazikExternalProtosInfo,
            ],
        ),
        "ignore_non_proto3": attr.bool(
            default = False
        ),
        "pass_tags":  attr.string_list(
            mandatory = True,
        ),
        "_template": attr.label(
            allow_single_file = True,
            default = "@build_flare_yazik//rules/proto/templates:collect_external_protos.py",
        ),
    },
    implementation = _collect_external_protos_script_impl,
)

def collect_external_protos(name, deps, ignore_non_proto3 = False, visibility = None, tags = []):
    _external_protos(
        name = name + "_info",
        deps = deps,
        visibility = visibility,
    )
    _collect_external_protos_script(
        name = name + "_gen_script",
        build_files_visibility = visibility,
        out_file_name = name,
        info = ":%s_info" % name,
        ignore_non_proto3 = ignore_non_proto3,
        pass_tags = tags,
        tags = [ "local", "manual",  ],
    )
    native.py_binary(
        name = name,
        srcs = [":%s_gen_script" % name ],
        data = [ ":%s_info" % name, ],
        tags = [ "local", "manual", ],
    )