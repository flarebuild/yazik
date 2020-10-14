#include <yazik/utility/strings.hpp>

#include <charconv>

namespace yazik {
namespace strings {

    Result<std::size_t> size_from_string_view(string_view source) {
        std::size_t result_value;
        auto result = std::from_chars(
            source.data(),
            source.data() + source.size(),
            result_value
        );
        if (result.ec == std::errc::invalid_argument) {
            return yaz_fail_fmt("invalid argument: {}", source);
        }
        return result_value;
    }

} // end of ::yazik::strings namespace
} // end of ::yazik namespace