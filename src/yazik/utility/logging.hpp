#pragma once

#include <yazik/utility/macro.hpp>
#include <yazik/utility/utility_defs.hpp>

namespace yazik {
namespace logging {

    enum class LogLevel {
        Debug,
        Info,
        Warn,
        Error,
        Crit
    };

    void do_log(LogLevel, const string& data);

    struct FormatAdaptorDefault {
        template <typename Arg>
        static inline Arg adapt(Arg arg) {
            return std::forward<Arg>(arg);
        }
    };

    template <logging::LogLevel LL, logging::LogLevel LLAllowed, typename Adaptor>
    struct Logger {
        template <typename S, typename... Args>
        inline void operator()(const S& format_str, Args&&... args) const {
            if constexpr (LL >= LLAllowed)
                do_log(LL, do_format(format_str, Adaptor::template adapt<Args>(args)...));
        }

        template <typename OtherAdaptor>
        Logger<LL, LLAllowed, OtherAdaptor> with() { return Logger<LL, LLAllowed, OtherAdaptor>{}; }
    };

    template <typename Adaptor, logging::LogLevel LLAllowed>
    struct L {
        static constexpr auto debug = logging::Logger<logging::LogLevel::Debug, LLAllowed, Adaptor>{};
        static constexpr auto info = logging::Logger<logging::LogLevel::Info, LLAllowed, Adaptor>{};
        static constexpr auto warn = logging::Logger<logging::LogLevel::Warn, LLAllowed, Adaptor>{};
        static constexpr auto error = logging::Logger<logging::LogLevel::Error, LLAllowed, Adaptor>{};
        static constexpr auto crit = logging::Logger<logging::LogLevel::Crit, LLAllowed, Adaptor>{};
    };

} // end of ::yazik::logging namespace

using L = logging::L<
    logging::FormatAdaptorDefault,
    $yaz_release(logging::LogLevel::Info)
    $yaz_debug(logging::LogLevel::Debug)
>;

} // end of ::yazik namespace