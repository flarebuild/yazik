#include "event.hpp"


namespace yazik {
namespace concurrency {

    Event::Event() {}

    void Event::wait() {
        if (is_set()) return;
        std::unique_lock<std::mutex> lk{_wait_mutex,};
        _cv.wait(lk, std::bind(&Event::is_set, this));
    }

    void Event::wait_for(std::chrono::milliseconds ms) {
        if (is_set()) return;
        std::unique_lock<std::mutex> lk{_wait_mutex,};
        _cv.wait_for(lk, ms, std::bind(&Event::is_set, this));
    }

    void Event::set() {
        if (is_set()) return;
        _notified = true;
        _cv.notify_all();
    }

    void Event::clear() {
        _notified = false;
    }

    bool Event::is_set() {
        return _notified;
    }

    CoroEvent::CoroEvent(bool initially_set) noexcept
    : _state(initially_set ? static_cast<void*>(this) : nullptr)
    {}

    CoroEvent::~CoroEvent() {
        // There should be no coroutines still awaiting the event.
        assert(
            _state.load(std::memory_order_relaxed) == nullptr ||
            _state.load(std::memory_order_relaxed) == static_cast<void*>(this)
        );
    }

    bool CoroEvent::is_set() const noexcept {
        return _state.load(std::memory_order_acquire) == static_cast<const void*>(this);
    }

    CoroEventOp CoroEvent::operator co_await() const noexcept {
        return CoroEventOp{ *this };
    }

    void CoroEvent::set() noexcept {
        void* const set_state = static_cast<void*>(this);

        // Needs 'release' semantics so that prior writes are visible to event awaiters
        // that synchronise either via 'is_set()' or 'operator co_await()'.
        // Needs 'acquire' semantics in case there are any waiters so that we see
        // prior writes to the waiting coroutine's state and to the contents of
        // the queued async_manual_reset_event_operation objects.
        void* old_state = _state.exchange(set_state, std::memory_order_acq_rel);
        if (old_state != set_state) {
            auto* current = static_cast<CoroEventOp*>(old_state);
            while (current != nullptr) {
                auto* next = current->_next;
                current->_awaiter.resume();
                current = next;
            }
        }
    }

    void CoroEvent::reset() noexcept {
        void* old_state = static_cast<void*>(this);
        _state.compare_exchange_strong(old_state, nullptr, std::memory_order_relaxed);
    }

    CoroEventOp::CoroEventOp(const CoroEvent& event) noexcept
    : _event(event)
    {}

    bool CoroEventOp::await_ready() const noexcept {
        return _event.is_set();
    }

    bool CoroEventOp::await_suspend(std::experimental::coroutine_handle<> awaiter) noexcept {
        _awaiter = awaiter;

        const void* const set_state = static_cast<const void*>(&_event);

        void* old_state = _event._state.load(std::memory_order_acquire);
        do {
            if (old_state == set_state) {
                // State is now 'set' no need to suspend.
                return false;
            }

            _next = static_cast<CoroEventOp*>(old_state);
        } while (!_event._state.compare_exchange_weak(
            old_state,
            static_cast<void*>(this),
            std::memory_order_release,
            std::memory_order_acquire
        ));

        // Successfully queued this waiter to the list.
        return true;
    }

} // end of ::yazik::concurrency namespace
} // end of ::yazik namespace