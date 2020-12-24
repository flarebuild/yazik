#include <yazik/testlib/yaz_test_macros.hpp>
#include <yazik/utility/utility_defs.hpp>
#include <yazik/concurrency/channel.hpp>

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

YAZ_F_TEST_CASE("OneWay test", "[utility/memory]") {
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

Channel<int> channel_ok() {
    TestCountedObj obj;
    for (int i: views::ints(0, 10))
        co_yield i;
    co_return;
}

Channel<int> channel_fail() {
    TestCountedObj obj;
    for (int i: views::ints(0, 10))
        co_yield i;
    co_await yaz_fail("oops");
    co_return;
}

template <typename Fn>
Task<> eval_channel(Fn&& fn) {
    TestCountedObj obj;
    auto gen = fn();
    for co_await(auto _: gen) {}
    co_return;
}

YAZ_F_TEST_CASE("Channel test", "[utility/memory]") {
    {
        co_await eval_channel(channel_ok).wrapped();
        REQUIRE(s_counter == 0);
    } {
        co_await eval_channel(channel_fail).wrapped();
        REQUIRE(s_counter == 0);
    }
}

Result<> ok_result() {
    TestCountedObj obj;
    static bool s_is_first_run = true;
    if (s_is_first_run) {
        s_is_first_run = false;
        co_await ok_result();
    }

    co_return;
}

Result<> fail_result() {
    TestCountedObj obj;
    static bool s_is_first_run = true;
    if (s_is_first_run) {
        s_is_first_run = false;
        co_await ok_result();
    } else {
        co_await yaz_fail("oops");
    }

    co_return;
}

TEST_CASE("Result test", "[utility/memory]") {
    {
        ok_result();
        REQUIRE(s_counter == 0);
    } {
        fail_result();
        REQUIRE(s_counter == 0);
    }
}