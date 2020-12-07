#include <yazik/testlib/yaz_test_macros.hpp>
#include <yazik/concurrency/executor.hpp>
#include <yazik/utility/utility_defs.hpp>

using namespace yazik;

Future<void> test_executor(const concurrency::executor_ptr_t& executor) {
    auto executor_thread_id = co_await executor->dispatch_task([] {
        return std::this_thread::get_id();
    } YAZ_PRE_COMMA_LOCATION_STR);
    REQUIRE(std::this_thread::get_id() == executor_thread_id);
    co_return;
}

TEST_CASE( "Executor test", "[concurrency/executor]" ) {
    auto executor = concurrency::create_single_thread_executor();
    REQUIRE(test_executor(executor).get());
    executor->stop();
    executor->wait();
}

TEST_CASE( "Executor cancellation test", "[concurrency/executor]" ) {
    auto executor = concurrency::create_single_thread_executor();
    concurrency::Event event_start;
    concurrency::Event event_stop;
    executor->dispatch(
        std::bind(&concurrency::Event::wait, &event_start)
        YAZ_PRE_COMMA_LOCATION_STR
    );
    executor->dispatch([](auto cancel_token){
        REQUIRE_FALSE(cancel_token.is_cancelled());
    } YAZ_PRE_COMMA_LOCATION_STR);
    auto disposer = executor->dispatch([](auto cancel_token){
        REQUIRE(cancel_token.is_cancelled());
    } YAZ_PRE_COMMA_LOCATION_STR);
    executor->dispatch(
        std::bind(&concurrency::Event::set, &event_stop)
        YAZ_PRE_COMMA_LOCATION_STR
    );
    disposer.dispose();
    event_start.set();
    event_stop.wait();
}

TEST_CASE( "Executor order test", "[concurrency]" ) {
    auto executor = concurrency::create_single_thread_executor(false);
    auto order = vector<int>{};
    executor->dispatch([&] {
        order.push_back(0);
        executor->dispatch([&] { order.push_back(4); } YAZ_PRE_COMMA_LOCATION_STR);
    } YAZ_PRE_COMMA_LOCATION_STR);
    executor->dispatch([&] {
        order.push_back(1);
        executor->dispatch([&] { order.push_back(5); } YAZ_PRE_COMMA_LOCATION_STR);
    } YAZ_PRE_COMMA_LOCATION_STR);
    executor->dispatch([&] {
        order.push_back(2);
        executor->dispatch([&] { order.push_back(6); } YAZ_PRE_COMMA_LOCATION_STR);
    } YAZ_PRE_COMMA_LOCATION_STR);
    executor->dispatch([&] {
        order.push_back(3);
        executor->dispatch([&] { order.push_back(7); } YAZ_PRE_COMMA_LOCATION_STR);
    } YAZ_PRE_COMMA_LOCATION_STR);

    executor->start().value();
    executor->stop();
    executor->wait();

    int i = 0;
    for (auto num : order)
        REQUIRE(num == i++);
    REQUIRE(i == 8);
}

TEST_CASE( "Executor order from thread test", "[concurrency]" ) {
    auto executor = concurrency::create_single_thread_executor(false);
    auto order = vector<int>{};
    auto thread = std::thread { [&] {
        executor->dispatch([&] {
            order.push_back(0);
            executor->dispatch([&] { order.push_back(4); } YAZ_PRE_COMMA_LOCATION_STR);
        } YAZ_PRE_COMMA_LOCATION_STR);
        executor->dispatch([&] {
            order.push_back(1);
            executor->dispatch([&] { order.push_back(5); } YAZ_PRE_COMMA_LOCATION_STR);
        } YAZ_PRE_COMMA_LOCATION_STR);
        executor->dispatch([&] {
            order.push_back(2);
            executor->dispatch([&] { order.push_back(6); } YAZ_PRE_COMMA_LOCATION_STR);
        } YAZ_PRE_COMMA_LOCATION_STR);
        executor->dispatch([&] {
            order.push_back(3);
            executor->dispatch([&] { order.push_back(7); } YAZ_PRE_COMMA_LOCATION_STR);
        } YAZ_PRE_COMMA_LOCATION_STR);
        executor->start().value();
    }};
    thread.join();
    executor->stop();
    executor->wait();

    int i = 0;
    for (auto num : order)
        REQUIRE(num == i++);
    REQUIRE(i == 8);
}

TEST_CASE( "Executor with future await test", "[concurrency/executor]" ) {
    auto executor = concurrency::create_single_thread_executor();

    auto promise1 = Promise<void>{};
    auto promise2 = Promise<void>{};

    auto event_started1 = concurrency::Event{};
    auto event_started2 = concurrency::Event{};

    auto future1 = executor->dispatch_future(
        [&promise2, &event_started1] () -> Task<> {
            event_started1.set();
            co_await promise2.get_future();;
            co_return;
        } YAZ_PRE_COMMA_LOCATION_STR
    ).fut();
    auto future2 = executor->dispatch_future(
        [&promise1, &event_started2] () -> Task<> {
            event_started2.set();
            co_await promise1.get_future();
            co_return;
        } YAZ_PRE_COMMA_LOCATION_STR
    ).fut();

    event_started1.wait();
    event_started2.wait();

    promise1.set_value();
    promise2.set_value();

    future2.wait();
    future1.wait();
}

TEST_CASE( "Executor await same executor handled future test", "[concurrency/executor]" ) {
    auto executor = concurrency::create_single_thread_executor(false);

    auto order = vector<int>{};

    auto future1 = executor->dispatch_future(
        [executor, &order] () mutable -> Future<void> {
            order.push_back(10);
            co_await executor->on(YAZ_LOCATION_STR);
            order.push_back(11);
            co_await executor->on(YAZ_LOCATION_STR);
            order.push_back(12);
            co_return;
        } YAZ_PRE_COMMA_LOCATION_STR
    ).fut();
    auto future2 = executor->dispatch_future(
        [executor, &order] () mutable -> Future<void> {
            order.push_back(20);
            co_await executor->on(YAZ_LOCATION_STR);
            order.push_back(21);
            co_await executor->on(YAZ_LOCATION_STR);
            order.push_back(22);
            co_return;
        } YAZ_PRE_COMMA_LOCATION_STR
    ).fut();

    executor->start().value();

    future2.wait();
    future1.wait();

    executor->stop();
    executor->wait();

    REQUIRE(order[0] == 10);
    REQUIRE(order[1] == 20);
    REQUIRE(order[2] == 11);
    REQUIRE(order[3] == 21);
    REQUIRE(order[4] == 12);
    REQUIRE(order[5] == 22);
}