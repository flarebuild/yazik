#pragma once

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/printer.h>

#include <yazik/compiler/common.hpp>

namespace yazik::compiler {

    class FileWriter {
        const gp::FileDescriptor* _file;
        const std::string _filename;
        const std::unique_ptr<gp::io::ZeroCopyOutputStream> _output;
        const std::unique_ptr<gp::io::Printer> _printer;

    public:

        FileWriter(
            const gp::FileDescriptor* file,
            gpc::GeneratorContext* generator_context,
            const std::string& extension
        );

        const gp::FileDescriptor* file() const;

        FileWriter& w_raw(const char* msg);
        FileWriter& w_raw(const std::string& msg);
        FileWriter& l();
        FileWriter& braced_detail(
            std::function<void()> fn,
            bool nl = true,
            const std::string& start_brace = "{",
            const std::string& end_brace = "}"
        );
        FileWriter& braced(std::function<void()> fn);
        FileWriter& class_braced(std::function<void()> fn);

        template <typename S, typename... Args>
        inline FileWriter& w(const S& format_str, Args&&... args) {
            return w_raw(do_sformat(format_str, std::forward<Args>(args)...));
        }

        template <typename S, typename... Args>
        inline FileWriter& wl(const S& format_str, Args&&... args) {
            return w_raw(do_sformat(format_str, std::forward<Args>(args)...)).l();
        }

        FileWriter& with_indent(std::function<void()> fn);

        template <typename S, typename... Args>
        inline FileWriter& wl_outdent(const S& format_str, Args&&... args) {
            _printer->Outdent();
            _printer->Outdent();
            yaz_defer {
                _printer->Indent();
                _printer->Indent();
            };
            return wl(format_str, std::forward<Args>(args)...);
        }

        template <typename S, typename... Args>
        inline FileWriter& wl_indent(const S& format_str, Args&&... args) {
            _printer->Indent();
            _printer->Indent();
            yaz_defer {
                _printer->Outdent();
                _printer->Outdent();
            };
            return wl(format_str, std::forward<Args>(args)...);
        }
    };

} // end of ::yazik::compiler namespace