#pragma once

#include <chrono>
#include <thread>
#include <mutex>

namespace yazik::concurrency {

    class Waiter {
        constexpr static std::size_t s_min_nanosec = 16;
        constexpr static std::size_t s_max_nanosec = 1'0 * 1'000 * 1'000;
        unsigned wait_nanosec = s_min_nanosec;

    public:

        inline void flush() noexcept {
            wait_nanosec = s_min_nanosec;
        }

        inline std::size_t wait_nanosec_count() noexcept {
            return wait_nanosec;
        }

        inline void increment() noexcept {
            wait_nanosec *= 2;
            if (wait_nanosec > s_max_nanosec)
                wait_nanosec = s_max_nanosec;
        }

    };

    class ThreadSleeper
    : public Waiter {
        using nanosec_t = std::chrono::nanoseconds;
        using h_r_clock = std::chrono::high_resolution_clock;
    public:
        inline void wait() noexcept {
            std::this_thread::sleep_for(nanosec_t{wait_nanosec_count()});
        }
    };

    class Baton {
        mutable size_t _attempts = 0;
        size_t _max_attempts = 5;
        ThreadSleeper _sleeper;

    public:

        inline Baton() noexcept {}

        inline Baton(size_t max_attempts) noexcept
        : _max_attempts{max_attempts}
        {}

        inline void increment_and_check() noexcept {
            if (_max_attempts && _attempts++ > _max_attempts) {
                _sleeper.wait();
                _sleeper.increment();
                _attempts = 0;
            }
        }

        inline void flush() {
            _attempts = 0;
            _sleeper.flush();
        }
    };

    class CVWaiter
    : public Waiter {
        std::mutex _wait_mutex;
        std::unique_lock<std::mutex> _wait_lock{_wait_mutex};
        std::condition_variable& _wait_cv;
    public:
        CVWaiter(std::condition_variable& wait_cv)
        : _wait_cv{wait_cv}
        {}

        inline void wait(unsigned millisec) {
            _wait_cv.wait_for(_wait_lock, std::chrono::milliseconds(millisec));
        }

        inline void check(bool processed) noexcept {
            if (processed) {
               flush();
            } else {
                wait(wait_nanosec_count());
                increment();
            }
        }
    };


} // end of ::yazik::concurrency namespace