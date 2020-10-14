load("@bazel_skylib//lib:paths.bzl", "paths")

def vimp_path_src_with_ext_replace(ctx, src, ext):
    return paths.replace_extension(src.short_path, ext)[ len(ctx.label.package) + 1: ]