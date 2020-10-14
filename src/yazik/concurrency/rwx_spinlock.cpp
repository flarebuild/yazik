#include <yazik/concurrency/rwx_spinlock.hpp>
#include <yazik/concurrency/baton.hpp>
#include <limits>
#include <assert.h>

namespace {

    constexpr long sMinLong = std::numeric_limits<long>::min();

} // namespace <anonymous>

namespace yazik::concurrency {

bool RWXSpinlock::try_r_lock() noexcept {
    long v = _flag.load();
    if (v >= 0 &&  (v % 2) == 0)
        return _flag.compare_exchange_weak(v, v + 2);
    return false;
}

void RWXSpinlock::r_lock() noexcept {
    long v = _flag.load();
    for (;;) {
        Baton b;
        if (v < 0 || (v % 2) != 0) {
            v = _flag.load();
        } else if (_flag.compare_exchange_weak(v, v + 2)) {
            return;
        }
        b.increment_and_check();
    }
}

bool RWXSpinlock::try_w_lock() noexcept {
    long v = _flag.load();
    if (v >= 0 && (v % 2) == 0)
        return _flag.compare_exchange_weak(v, v + 1);
    return false;
}


void RWXSpinlock::w_lock() noexcept {
    long v = _flag.load();
    for (;;) {
        Baton b;
        if (v < 0 || (v % 2) != 0) {
            v = _flag.load();
        } else if (_flag.compare_exchange_weak(v, v + 1)) {
            return;
        }
        b.increment_and_check();
    }
}

bool RWXSpinlock::try_x_lock() noexcept {
    long v = _flag.load();
    if (v == 0)
        return _flag.compare_exchange_weak(v,sMinLong);
    return false;
}


void RWXSpinlock::x_lock() noexcept {
    long v = _flag.load();
    for(;;) {
        Baton b;
        if (v != 0) {
            v = _flag.load();
        } else if (_flag.compare_exchange_weak(v,sMinLong)) {
            return;
        }
        b.increment_and_check();
    }
}

bool RWXSpinlock::try_upgrade_r_to_w_lock() noexcept {
    long v = _flag.load();
    if (v >= 0 && (v % 2) == 0)
        return _flag.compare_exchange_weak(v, v - 1);
    return false;
}

void RWXSpinlock::upgrade_w_to_x_lock() noexcept {
    long v = _flag.load();
    assert(v > 0 && (v % 2) == 1);
    for(;;) {
        Baton b;
        if (v != 1) {
            v = _flag.load();
        } else if (_flag.compare_exchange_weak(v,sMinLong)) {
            return;
        }
        b.increment_and_check();
    }
}

void RWXSpinlock::r_unlock() noexcept {
    _flag.fetch_sub(2);
}

void RWXSpinlock::w_unlock() noexcept {
    _flag.fetch_sub(1);
}

void RWXSpinlock::x_unlock() noexcept {
    _flag.store(0);
}

RWXLocker::RWXLocker() noexcept
: _spinlock{nullptr}
, _lockType{RWXLockType::no}
{}

RWXLocker::RWXLocker(RWXSpinlock& spinlock,RWXLockType lockType) noexcept
: _spinlock{&spinlock}
, _lockType{lockType}
{
    switch(_lockType) {
        case RWXLockType::read:
            _spinlock->r_lock();
            break;
        case RWXLockType::write:
            _spinlock->w_lock();
            break;
        case RWXLockType::exclusive:
            _spinlock->x_lock();
            break;
        default:
            break;
    }
}

RWXLocker::RWXLocker(RWXLocker&& other) noexcept {
    _spinlock = other._spinlock;
    other._spinlock = nullptr;
    _lockType = other._lockType;
    other._lockType = RWXLockType::no;
}

RWXLocker::~RWXLocker() noexcept {
    unlock();
}

RWXLocker& RWXLocker::operator= (RWXLocker&& other) noexcept {
    unlock();
    _spinlock = other._spinlock;
    other._spinlock = nullptr;
    _lockType = other._lockType;
    other._lockType = RWXLockType::no;
    return *this;
}

void RWXLocker::unlock() noexcept {
    switch(_lockType) {
        case RWXLockType::read:
            _spinlock->r_unlock();
            break;
        case RWXLockType::write:
            _spinlock->w_unlock();
            break;
        case RWXLockType::exclusive:
            _spinlock->x_unlock();
            break;
        default:
            break;
    }
    _lockType = RWXLockType::no;
}

bool RWXLocker::try_upgrade_to_w() noexcept {
    switch(_lockType) {
        case RWXLockType::read:
            if (_spinlock->try_upgrade_r_to_w_lock()) {
                _lockType = RWXLockType::write;
                return true;
            }
        default:
            return false;
    }
}

void RWXLocker::upgrade_to_x() noexcept {
    switch(_lockType) {
        case RWXLockType::write:
            _spinlock->upgrade_w_to_x_lock();
            _lockType = RWXLockType::exclusive;
            break;
        case RWXLockType::exclusive:
            break;
        default:
            assert(false);
            break;
    }
}

} // end of ::yazik::concurrency namespace