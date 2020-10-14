#pragma once

#include <yazik/utility/comparable.hpp>

namespace yazik::utility {

    /// Unit is analogous to `void`, but can be safely returned, stored, etc.
    /// to enable higher-order abstraction without cluttering code with
    /// exceptions for `void` (which can't be stored, for example).
    struct unit_t : comparable<unit_t> {
        constexpr unit_t() noexcept {
            // nop
        }

        constexpr unit_t(const unit_t&) noexcept {
            // nop
        }

        template <class T>
        explicit constexpr unit_t(T&&) noexcept {
            // nop
        }

        static constexpr int compare(const unit_t&) noexcept {
            return 0;
        }

        template <class... Ts>
        constexpr unit_t operator()(Ts&&...) const noexcept {
            return {};
        }
    };

    static constexpr unit_t unit = unit_t{};

    template <class T>
    struct lift_void {
        using type = T;
    };

    template <>
    struct lift_void<void> {
        using type = unit_t;
    };

    template <class T>
    struct unlift_void {
        using type = T;
    };

    template <>
    struct unlift_void<unit_t> {
        using type = void;
    };

} // end of ::yazik::utility namespace