#include <google/protobuf/descriptor.pb.h>
#include <yazik/compiler/utility/utility.hpp>

#include "writer.hpp"


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

} // end of ::yazik::compiler namespace