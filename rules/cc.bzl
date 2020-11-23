load("@bazel_skylib//lib:paths.bzl", "paths")

def _cc_rule(ruledef, **kwargs):
    ruledef(
        copts = kwargs.pop("copts", []) + [ "-std=c++2a" ],
        **kwargs
    )

def cc_binary(**kwargs):
    _cc_rule(native.cc_binary, **kwargs)

def cc_library(**kwargs):
    _cc_rule(native.cc_library, **kwargs)

def cc_catch_test(**kwargs):
     _cc_rule(
        native.cc_test,
        deps = kwargs.pop("deps", []) + [
            "@com_github_catchorg_catch//:catch2_main",
            "@build_flare_yazik//src/yazik/testlib",
        ],
        defines = [
            "YAZ_TEST",
        ],
        copts = kwargs.pop("copts", []) + [ "-std=c++2a" ],
        **kwargs
    )

def cc_catch_test_suite(**kwargs):
    for src in kwargs.pop("srcs", []):
        cc_catch_test(
            name = paths.replace_extension(paths.basename(src), ""),
            srcs = [ src ],
            **kwargs
        )

cc = struct(
    binary = cc_binary,
    library = cc_library,
    catch_test = cc_catch_test,
    catch_test_suite = cc_catch_test_suite,
)