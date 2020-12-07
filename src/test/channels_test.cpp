#include <yazik/testlib/yaz_test_macros.hpp>
#include <yazik/utility/utility_defs.hpp>
#include <yazik/concurrency/executor.hpp>
#include <yazik/utility/detail/result_detail.hpp>
#include <yazik/concurrency/channel.hpp>
#include <yazik/rpc/base/rpc_status.hpp>

using namespace yazik;
using namespace yazik::concurrency;


TEST_CASE( "channel status test", "[rpc/channel]") {
    bool constructible_unexpected = std::is_constructible<
        rpc::RpcStatus,
        ::yazik::detail::UnxepectedStringTag,
        string
    >::value;
    REQUIRE(constructible_unexpected);
    bool has_to_error_string = ::yazik::detail::c_is_legal_result_error<
        rpc::RpcStatus
    >;
    REQUIRE(has_to_error_string);
}


int fibonacci(int n) {
    /* Declare an array to store Fibonacci numbers. */
    int f[n+1];
    int i;

    /* 0th and 1st number of the series are 0 and 1*/
    f[0] = 0;
    f[1] = 1;

    for (i = 2; i <= n; i++) {
        /* Add the previous 2 numbers in the series
         and store it */
        f[i] = f[i-1] + f[i-2];
    }

    return f[n];
}


Channel<int, rpc::RpcStatus> fibonacci_channel(const int ceiling) {
    int j = 0;
    int i = 1;
    co_yield j;
    if (ceiling > j) {
        do {
            co_yield i;
            int tmp = i;
            i += j;
            j = tmp;
        } while (i <= ceiling);
    }
    co_return;
}

TEST_CASE( "fibonacci channel test", "[rpc/channel]") {
    int ceiling = fibonacci(42);
    int i = 0;
    [&]() -> Future<void> {
        for co_await(int value: fibonacci_channel(ceiling)) {
            int calculated = fibonacci(i);
            REQUIRE(value == calculated);
            ++i;
        }
    }().get().value();
    REQUIRE(i == 43);
}

Channel<int, rpc::RpcStatus> fibonacci_async_channel(
    const int ceiling,
    const concurrency::executor_ptr_t& executor
) {
    int j = 0;
    int i = 1;
    co_await executor->on(YAZ_LOCATION_STR);
    co_yield j;
    if (ceiling > j) {
        do {
            co_await executor->on(YAZ_LOCATION_STR);
            co_yield i;
            int tmp = i;
            i += j;
            j = tmp;
        } while (i <= ceiling);
    }
    co_return;
}

TEST_CASE( "async channel test", "[rpc/channel]") {
    int ceiling = fibonacci(42);

    auto executor = concurrency::create_single_thread_executor();
    auto order = vector<int>{};

    auto event = concurrency::CoroEvent{};

    auto async_fib = [&] () -> Future<> {
        co_await event;
        for co_await(int value:  fibonacci_async_channel(ceiling, executor))
            order.push_back(value);

        co_return;
    };

    auto future1 = executor->dispatch_future(async_fib YAZ_PRE_COMMA_LOCATION_STR).fut();
    auto future2 = executor->dispatch_future(async_fib YAZ_PRE_COMMA_LOCATION_STR).fut();
    executor->dispatch([&]{ event.set(); } YAZ_PRE_COMMA_LOCATION_STR);

    future1.wait();
    future2.wait();

    executor->stop();
    executor->wait();

    int i = 0;
    for (auto it = order.begin(); it != order.end(); ++it) {
        int fib = fibonacci(i++);
        REQUIRE(*it == fib);
        REQUIRE(*(++it) == fib);
    }
    REQUIRE(i == 43);
}

Channel<int, rpc::RpcStatus> success_channel() {
    co_yield 0;
    co_yield 1;
    co_yield 2;
    co_yield 3;
}

Future<void> success_channel_iterator(Channel<int, rpc::RpcStatus> channel) {
    int i = 0;
    for co_await(auto val: channel)
        REQUIRE(val == i++);

    REQUIRE(i == 4);

    REQUIRE(channel.status().is_ok());

    co_return;
}

Channel<int, rpc::RpcStatus> error_channel() {
    co_yield 0;
    co_yield 1;
    co_yield 2;
    throw std::runtime_error("fial");
    co_yield 3;
}

Future<void, rpc::RpcStatus> error_channel_iterator(Channel<int, rpc::RpcStatus> channel) {
    int i = 0;
    for co_await(auto val: channel)
        REQUIRE(val == i++);

    REQUIRE(i == 3);

    REQUIRE_FALSE(channel.status().is_ok());
    co_await yaz_fail(channel.status().to_error_string());
    co_return;
}

Channel<int, rpc::RpcStatus> error_only_channel() {
    co_await yaz_fail("fial");
    co_return;
}

Future<void, rpc::RpcStatus> error_only_channel_iterator(Channel<int, rpc::RpcStatus> channel) {
    for co_await(auto val: channel) {}
    if (!channel.status().is_ok())
        co_await channel.status().as_unexpected();
    co_return;
}

TEST_CASE( "channel error test", "[rpc/channel]") {
    REQUIRE(success_channel_iterator(success_channel()).get());
    REQUIRE_FALSE(error_channel_iterator(error_channel()).get());
    REQUIRE_FALSE(error_only_channel_iterator(error_only_channel()).get());
}
