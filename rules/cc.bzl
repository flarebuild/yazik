def _cc_rule(ruledef, **kwargs):
    ruledef(
        copts = kwargs.pop("copts", []) + [ "-std=c++2a" ],
        **kwargs
    )

def cc_binary(**kwargs):
    _cc_rule(native.cc_binary, **kwargs)

def cc_library(**kwargs):
    _cc_rule(native.cc_library, **kwargs)

cc = struct(
    binary = cc_binary,
    library = cc_library,
)