#include <yazik/testlib/yaz_test_macros.hpp>
#include <yazik/utility/utility_defs.hpp>

using namespace yazik;

std::atomic_size_t s_counter { 0 };

struct TestCountedObj {

    TestCountedObj() {
        ++s_counter;
    }

    ~TestCountedObj() {
        --s_counter;
    }
};

OneWayTask oneway_task(
    const concurrency::scheduler_ptr_t& sched,
    concurrency::CoroEvent& ev
) {
    static bool is_first_call = true;
    TestCountedObj obj1;
    co_await sched->schedule_delayed(std::chrono::milliseconds{10});
    if (is_first_call) {
        is_first_call = false;
        oneway_task(sched, ev);
    } else {
        ev.set();
    }
    TestCountedObj obj2;
}

OneWayTask oneway_failed_task(
    const concurrency::scheduler_ptr_t& sched,
    concurrency::CoroEvent& ev
) {
    static bool is_first_call = true;
    TestCountedObj obj1;
    co_await sched->schedule_delayed(std::chrono::milliseconds{10});
    if (is_first_call) {
        is_first_call = false;
        oneway_failed_task(sched, ev);
    } else {
        ev.set();
        co_await yaz_fail("oops");
    }
    TestCountedObj obj2;
}

YAZ_F_TEST_CASE("OneWay task", "[utility/memory]") {
    {
        concurrency::CoroEvent ev;
        oneway_task(__scheduler, ev);
        co_await ev;
        co_await __scheduler->schedule_delayed(std::chrono::milliseconds{10});
        REQUIRE(s_counter == 0);
    } {
        concurrency::CoroEvent ev;
        oneway_failed_task(__scheduler, ev);
        co_await ev;
        co_await __scheduler->schedule_delayed(std::chrono::milliseconds{10});
        REQUIRE(s_counter == 0);
    }
}