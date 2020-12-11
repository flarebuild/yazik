#include <iostream>

#include <yazik/utility/logging.hpp>

namespace yazik::logging {

    static std::ostream& operator<<(std::ostream& os, LogLevel ll) {
        return os;
    }

    const char* c_log_level_str(LogLevel ll) {
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

    void do_log(LogLevel ll, const string& data) {
        std::cout <<  c_log_level_str(ll) << ": " << data << std::endl;
        if (ll == LogLevel::Crit)
            std::abort();
    }

} // end of ::yazik::logging namespace