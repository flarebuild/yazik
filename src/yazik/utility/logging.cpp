#include <iostream>

#include <yazik/utility/logging.hpp>

namespace yazik::logging {

    static std::ostream& operator<<(std::ostream& os, LogLevel ll) {
        return os;
    }

    void do_log(const char* level, const string& data) {
        std::cout << do_format("{} : {}\n", level, data);
    }

} // end of ::yazik::logging namespace