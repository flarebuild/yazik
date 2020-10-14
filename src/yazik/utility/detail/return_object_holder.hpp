#include <optional>

#include <yazik/utility/utility_defs.hpp>

namespace yazik::detail {

    // based on
    // https://github.com/toby-allsopp/coroutine_monad
    template <typename T>
    class ReturnObjectHolder {
        // The staging object that is returned (by copy/move)
        // to the caller of the coroutine.
        std::optional<T> _stage;
        ReturnObjectHolder*& _ptr;
    public:
        // When constructed, we assign a pointer to ourselves
        // to the supplied reference to pointer.
        ReturnObjectHolder(ReturnObjectHolder*& ptr)
        : _ptr{ptr}
        { _ptr = this; }

        // Copying doesn't make any sense (which copy should the pointer refer to?).
        ReturnObjectHolder(ReturnObjectHolder const&) = delete;

        // To move, we just update the pointer to point at the new object.
        ReturnObjectHolder(ReturnObjectHolder&& other)
        : _stage{std::move(other.stage)}
        , _ptr{other._ptr}
        { _ptr = this; }

        // Assignment doesn't make sense.
        void operator=(ReturnObjectHolder const&) = delete;
        void operator=(ReturnObjectHolder&&) = delete;

        // A non-trivial destructor is required until
        // https://bugs.llvm.org//show_bug.cgi?id=28593 is fixed.
        ~ReturnObjectHolder() {}

        // Construct the staging value; arguments are perfect forwarded to T's constructor.
        void emplace(T&& value) {
            _stage = std::forward<T>(value);
        }

        // We assume that we will be converted only once, so we can move from the staging
        // object. We also assume that `emplace` has been called at least once.
        operator T() {
            return std::move(*_stage);
        }
    };

    template <typename T>
    auto make_return_object_holder(ReturnObjectHolder<T>*& p) {
        return ReturnObjectHolder<T>{p};
    }

} // end of ::yazik::detail namespace
