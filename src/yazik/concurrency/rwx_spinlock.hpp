#pragma once

#include <atomic>
#include <cstddef>

namespace yazik::concurrency {

class RWXSpinlock {
    std::atomic<long> _flag{0};

public:

    bool try_r_lock() noexcept;
    void r_lock() noexcept;
    bool try_w_lock() noexcept;
    void w_lock() noexcept;
    bool try_x_lock() noexcept;
    void x_lock() noexcept;

    bool try_upgrade_r_to_w_lock() noexcept;

    void upgrade_w_to_x_lock() noexcept;

    void r_unlock() noexcept;
    void w_unlock() noexcept;
    void x_unlock() noexcept;

};

enum class RWXLockType : int {
    no,
    read,
    write,
    exclusive
};

class RWXLocker {

    RWXSpinlock* _spinlock;
    RWXLockType _lockType;

public:

    RWXLocker() noexcept;
    RWXLocker(RWXSpinlock& spinlock, RWXLockType lockType) noexcept;
    RWXLocker(RWXLocker&&) noexcept;
    RWXLocker(const RWXLocker&) = delete;
    RWXLocker& operator= (RWXLocker&&) noexcept;
    RWXLocker& operator= (const RWXLocker&) = delete;

    bool try_upgrade_to_w() noexcept;
    void upgrade_to_x() noexcept;
    void unlock() noexcept;
    ~RWXLocker() noexcept;
};

} //end of ::yazik::concurrency namespace