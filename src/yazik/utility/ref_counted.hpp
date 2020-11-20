#pragma once

#include <atomic>
#include <cstddef>
#include "logging.hpp"

namespace yazik::utility {

    /// Base class for reference counted objects with an atomic reference count.
    /// Serves the requirements of {@link intrusive_ptr}.
    /// @note *All* instances of `ref_counted` start with a reference count of 1.
    /// @relates intrusive_ptr
    class ref_counted {
    public:
        virtual ~ref_counted() {}

        inline ref_counted() {}
        inline ref_counted(const ref_counted&)
        : rc_(1)
        {}

        ref_counted& operator=(const ref_counted&) {
            return *this;
        }

        /// Increases reference count by one.
        inline void ref() const noexcept {
            rc_.fetch_add(1, std::memory_order_relaxed);
        }

        /// Decreases reference count by one and calls delete
        /// when it drops to zero.
        inline void deref() const noexcept {
            if (unique()) {
                delete this;
                return;
            }
            if (rc_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete this;
            }
        }

        /// Queries whether there is exactly one reference.
        inline bool unique() const noexcept {
            return rc_ == 1;
        }

        inline size_t get_reference_count() const noexcept {
            return rc_;
        }

    protected:
        mutable std::atomic<size_t> rc_{0};
    };

} // end of ::yazik::utility namespace

/// @relates ref_counted
inline void intrusive_ptr_add_ref(const ::yazik::utility::ref_counted* p) {
    p->ref();
}

/// @relates ref_counted
inline void intrusive_ptr_release(const ::yazik::utility::ref_counted* p) {
    p->deref();
}