#pragma once

#include <yazik/utility/comparable.hpp>

namespace yazik::utility {

    /// Represents "nothing", e.g., for clearing an `optional` by assigning `none`.
    struct none_t : comparable<none_t> {
        constexpr none_t() {
            // nop
        }
        constexpr explicit operator bool() const {
            return false;
        }

        static constexpr int compare(none_t) {
            return 0;
        }
    };

    static constexpr none_t none = none_t{};

} // end of ::yazik::utility namespace