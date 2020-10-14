#pragma once

namespace yazik::utility {

    /// Barton–Nackman trick implementation.
    /// `Subclass` must provide a compare member function that compares
    /// to instances of `T` and returns an integer x with:
    /// - `x < 0` if `*this < other`
    /// - `x > 0` if `*this > other`
    /// - `x == 0` if `*this == other`
    template <class Subclass, class T = Subclass>
    class comparable {
        friend bool operator==(const Subclass& lhs, const T& rhs) noexcept {
            return lhs.compare(rhs) == 0;
        }

        friend bool operator==(const T& lhs, const Subclass& rhs) noexcept {
            return rhs.compare(lhs) == 0;
        }

        friend bool operator!=(const Subclass& lhs, const T& rhs) noexcept {
            return lhs.compare(rhs) != 0;
        }

        friend bool operator!=(const T& lhs, const Subclass& rhs) noexcept {
            return rhs.compare(lhs) != 0;
        }

        friend bool operator<(const Subclass& lhs, const T& rhs) noexcept {
            return lhs.compare(rhs) < 0;
        }

        friend bool operator>(const Subclass& lhs, const T& rhs) noexcept {
            return lhs.compare(rhs) > 0;
        }

        friend bool operator<(const T& lhs, const Subclass& rhs) noexcept {
            return rhs > lhs;
        }

        friend bool operator>(const T& lhs, const Subclass& rhs) noexcept {
            return rhs < lhs;
        }

        friend bool operator<=(const Subclass& lhs, const T& rhs) noexcept {
            return lhs.compare(rhs) <= 0;
        }

        friend bool operator>=(const Subclass& lhs, const T& rhs) noexcept {
            return lhs.compare(rhs) >= 0;
        }

        friend bool operator<=(const T& lhs, const Subclass& rhs) noexcept {
            return rhs >= lhs;
        }

        friend bool operator>=(const T& lhs, const Subclass& rhs) noexcept {
            return rhs <= lhs;
        }
    };

    template <class Subclass>
    class comparable<Subclass, Subclass> {
        friend bool operator==(const Subclass& lhs, const Subclass& rhs) noexcept {
            return lhs.compare(rhs) == 0;
        }

        friend bool operator!=(const Subclass& lhs, const Subclass& rhs) noexcept {
            return lhs.compare(rhs) != 0;
        }

        friend bool operator<(const Subclass& lhs, const Subclass& rhs) noexcept {
            return lhs.compare(rhs) < 0;
        }

        friend bool operator<=(const Subclass& lhs, const Subclass& rhs) noexcept {
            return lhs.compare(rhs) <= 0;
        }

        friend bool operator>(const Subclass& lhs, const Subclass& rhs) noexcept {
            return lhs.compare(rhs) > 0;
        }

        friend bool operator>=(const Subclass& lhs, const Subclass& rhs) noexcept {
            return lhs.compare(rhs) >= 0;
        }
    };

} // end of ::yazik::utiulity namespace