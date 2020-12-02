#include <google/protobuf/descriptor.pb.h>
#include <yazik/compiler/utility/utility.hpp>

#include "writer.hpp"

#include <chaiscript/chaiscript.hpp>

namespace yazik::compiler {

    FileWriter::FileWriter(
        const gp::FileDescriptor* file,
        gpc::GeneratorContext* generator_context,
        const std::string& extension
    )
    : _file { file }
    , _filename { replace_proto(file->name(), extension) }
    , _output { generator_context->Open(_filename) }
    , _printer { new gp::io::Printer { _output.get(), '$'} }
    {}

    const gp::FileDescriptor* FileWriter::file() const {
        return _file;
    }

    FileWriter& FileWriter::w_raw(const char* msg) {
        _printer->Print(msg);
        return *this;
    }

    FileWriter& FileWriter::w_raw(const std::string& msg) {
        _printer->Print(msg.c_str());
        return *this;
    }

    FileWriter& FileWriter::l() {
        return w_raw("\n");
    }

    FileWriter& FileWriter::braced_detail(
        std::function<void()> fn,
        bool nl,
        const std::string& start_brace,
        const std::string& end_brace
    ) {
        w("{}\n", start_brace);
        _printer->Indent();
        _printer->Indent();
        yaz_defer {
            _printer->Outdent();
            _printer->Outdent();
            w("{}{}", end_brace, nl ? "\n" : "");
        };
        fn();
        return *this;
    }

    FileWriter& FileWriter::braced(std::function<void()> fn) {
        return braced_detail(std::move(fn));
    }

    FileWriter& FileWriter::class_braced(std::function<void()> fn) {
        return braced_detail(std::move(fn), false).wl(";");
    }

    FileWriter& FileWriter::with_indent(std::function<void()> fn) {
        _printer->Indent();
        _printer->Indent();
        yaz_defer {
            _printer->Outdent();
            _printer->Outdent();
        };
        fn();
        return *this;
    }

    void FileWriter::add_to_chai(chaiscript::ChaiScript& script) {
        using namespace chaiscript;
        script.add(fun(
            [this](const std::string& str) { w("{}", str); }),
            "w"
        );
        script.add(fun(
            [this](const std::string& str) { wl("{}", str); }),
            "wl"
        );
        script.add(fun(
            [this]() { l(); }),
            "l"
        );
        script.add(fun(
            [this](const std::string& str) { wl_indent("{}", str); }),
            "wl_indent"
        );
        script.add(fun(
            [this](const std::string& str) { wl_outdent("{}", str); }),
            "wl_outdent"
        );

        script.add(
            fun([this](const std::string& name, std::function<void()> fn) {
                w("struct {} ", name)
                    .braced_detail(std::move(fn), false)
                    .wl(";");
            }),
            "w_cpp_struct"
        );
        script.add(
            fun([this](const std::string& name, std::function<void()> fn) {
                w("class {} ", name)
                    .braced_detail(std::move(fn), false)
                    .wl(";");
            }),
            "w_cpp_class"
        );


        script.add(
            fun([this](std::function<void()> fn) {
                wl("/*");
                fn();
                wl("*/");
            }),
            "w_cpp_commented"
        );

        auto func_base = [this] (
            const std::vector<std::string>& decl,
            const std::vector<std::string>& args,
            const std::vector<std::string>& mods
        ) {
            w("{}(", decl
                | views::join(' ')
                | ranges::to<std::string>() );

            if (args.empty()) {
                w(")");
            } else if (args.size() == 1) {
                w("{})", args[0]);
            } else {
                w_indent("\n{}\n", args
                    | views::join(std::string{",\n"})
                    | ranges::to<std::string>()
                ).w(")");
            }
            if (!mods.empty()) {
                w(" ");
                w(mods
                    | views::join(' ')
                    | ranges::to<std::string>());
            }

        };
        script.add(
            fun([this, func_base] (
                const std::vector<std::string>& decl,
                const std::vector<std::string>& args,
                const std::vector<std::string>& mods
            ) {
                func_base(decl, args, mods);
                wl(";");
            }),
            "w_cpp_fn_decl"
        );
        script.add(
            fun([this, func_base] (
                const std::vector<std::string>& decl,
                const std::vector<std::string>& args,
                const std::vector<std::string>& mods,
                std::function<void()> fn
            ) {
                func_base(decl, args, mods);
                w(" ");
                braced(std::move(fn));
            }),
            "w_cpp_fn_impl"
        );
        script.add(
            fun([this, func_base] (
                const std::vector<std::string>& templ,
                const std::vector<std::string>& decl,
                const std::vector<std::string>& args,
                const std::vector<std::string>& mods,
                std::function<void()> fn
            ) {
                wl("template <{}>", templ
                    | views::join(std::string{", "})
                    | ranges::to<std::string>());
                w("concept ");
                func_base(decl, args, mods);
                w(" ");
                braced_detail(std::move(fn), false);
                wl(";");
            }),
            "w_cpp_concept"
        );
        script.add(
            fun([this, func_base] (
                const std::vector<std::string>& decl,
                const std::vector<std::string>& args,
                const std::vector<std::string>& mods,
                const std::vector<std::string>& value_inits
            ) {
                func_base(decl, args, mods);
                if (!value_inits.empty()) {
                    wl("\n: {}", value_inits
                        | views::join(std::string{"\n, "})
                        | ranges::to<std::string>());
                }
                wl("{{}}");
            }),
            "w_cpp_constructor_impl_no_body"
        );
        script.add(
            fun([this](std::function<void()> fn) {
                braced_detail(std::move(fn), false);
            }),
            "braced"
        );

    }

} // end of ::yazik::compiler namespace