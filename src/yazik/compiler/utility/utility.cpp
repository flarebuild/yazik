#include <google/protobuf/compiler/cpp/cpp_helpers.h>
#include <boost/algorithm/string.hpp>
#include "utility.hpp"
#include <algorithm>

namespace yazik::compiler {

    std::string q(const std::string& str) {
        return do_sformat("\"{}\"", str);
    }

    std::string ab(const std::string& str) {
        return do_sformat("<{}>", str);
    }

    std::string qb(const std::string& str) {
        return do_sformat("<{}>", str);
    }

    std::string strip_proto(const std::string& filename) {
        return gpc::cpp::StripProto(filename);
    }

    std::string replace_proto(const std::string& filename, const std::string& ext) {
        return do_sformat("{}.{}", strip_proto(filename), ext);
    }

    std::string camel_case(const std::string& str) {
        return gpc::cpp::UnderscoresToCamelCase(str, true);
    }

    std::string camel_case_upper(const std::string& str) {
        std::string copy = str;
        for (auto & c: copy) c = std::toupper(c);
        return copy;
    }

    std::string snake_case(const std::string& str) {
        std::string result(1, tolower(str[0]));

        // First place underscores between contiguous lower and upper case letters.
        // For example, `_LowerCamelCase` becomes `_Lower_Camel_Case`.
        for (auto it = str.begin() + 1; it != str.end(); ++it) {
            if (isupper(*it) && *(it-1) != '_' && islower(*(it-1))) {
                result += "_";
            }
            result += *it;
        }

        // Then convert it to lower case.
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);

        return result;
    }

    std::string to_cpp_ns(const std::string& ns) {
        return ::boost::replace_all_copy(ns, ".", "::");
    }

} // end of ::yazik::compiler namespace