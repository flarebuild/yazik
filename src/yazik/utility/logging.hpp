#pragma once

#include <yazik/utility/utility_defs.hpp>

namespace yazik {
namespace logging {

    enum class LogLevel {
        Info,
        Debug,
        Warn,
        Error,
        Crit
    };

    inline constexpr const char* c_log_level_str(LogLevel ll) {
        switch(ll) {
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Warn:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Crit:
            return "CRIT";
        }
        return "";
    }

    void do_log(const char* level, const string& data);

    struct FormatAdaptorDefault {
        template <typename Arg>
        static Arg adapt(Arg arg) {
            return std::forward<Arg>(arg);
        }
    };

    template <logging::LogLevel LL, typename Adaptor>
    struct Logger {

//#ifdef DEBUG
        template <typename S, typename... Args>
        inline void operator()(const S& format_str, Args&&... args) const {
            do_log(
                c_log_level_str(LL),
                do_format(format_str, Adaptor::template adapt<Args>(args)...)
            );
        }
//#else
//        template <typename S, typename... Args>
//        inline void operator()(const S&, Args&&...) const {}
//#endif

        template <typename OtherAdaptor>
        Logger<LL, OtherAdaptor> with() { return Logger<LL, OtherAdaptor>{}; }
    };

    template <typename Adaptor>
    struct L {
        static constexpr auto info = logging::Logger<logging::LogLevel::Info, Adaptor>{};
        static constexpr auto debug = logging::Logger<logging::LogLevel::Debug, Adaptor>{};
        static constexpr auto warn = logging::Logger<logging::LogLevel::Warn, Adaptor>{};
        static constexpr auto error = logging::Logger<logging::LogLevel::Error, Adaptor>{};
        static constexpr auto crit = logging::Logger<logging::LogLevel::Crit, Adaptor>{};

    };

} // end of ::yazik::logging namespace

using L = logging::L<logging::FormatAdaptorDefault>;

} // end of ::yazik namespace