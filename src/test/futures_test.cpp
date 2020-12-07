#include <yazik/testlib/yaz_test_macros.hpp>
#include <yazik/utility/utility_defs.hpp>
#include <yazik/concurrency/future_result.hpp>

using namespace yazik;

Future<int> success_int_future(int res) {
    co_return res;
}

Future<int> failed_int_future(int res) {
    co_await yaz_fail<string>("test");
    co_return res;
}

Future<int> test_succeeded_future() {
    int x = co_await success_int_future(42);
    x *= co_await success_int_future(2);
    x -= co_await success_int_future(4);
    co_return x;
}

Future<int> test_unsucceed_future() {
    int x = co_await success_int_future(42);
    x *= co_await failed_int_future(2);
    x -= co_await success_int_future(4);
    co_return x;
}

Future<void> test_void_unsucceed_from_int_future() {
    int x = co_await success_int_future(42);
    x *= co_await failed_int_future(2);
    x -= co_await success_int_future(4);
    co_return;
}

Future<void> success_future() {
    co_return;
}

Future<void> failed_future() {
    co_await yaz_fail("test");
    co_return;
}

Future<void> thrown_future() {
    co_await success_future();
    throw std::runtime_error("test");
    co_return;
}

template <typename T>
string get_error(Future<T>&& f) {
    return f.get().error();
}

TEST_CASE( "future + co_await  test", "[concurrency/future]" ) {
    REQUIRE(test_succeeded_future().get().value() == 80);
    REQUIRE(get_error(test_unsucceed_future()) ==  "test");
    REQUIRE(get_error(test_void_unsucceed_from_int_future()) ==  "test");
    REQUIRE(get_error(thrown_future()) ==  "test");
}

Future<void> test_future_result() {
    Result<int> x = co_await success_int_future(42).wrapped();
    REQUIRE(x);
    REQUIRE(x.value() == 42);

    x = co_await failed_int_future(2).wrapped();
    REQUIRE_FALSE(x);
    REQUIRE(x.error() ==  "test");

    co_return;
}

TEST_CASE( "future result  test", "[concurrency/future]" ) {
    test_future_result().wait();
}

Future<int> test_async_futures_inner(
    const concurrency::executor_ptr_t& ex1,
    const concurrency::executor_ptr_t& ex2
) {
    auto this_thread_id = std::this_thread::get_id();
    auto thread1_id = ex1->dispatch_task([] {
        return std::this_thread::get_id();
    } YAZ_PRE_COMMA_LOCATION_STR).fut().get_unwrapped();
    auto thread2_id = ex2->dispatch_task([] {
        return std::this_thread::get_id();
    } YAZ_PRE_COMMA_LOCATION_STR).fut().get_unwrapped();

    int x = co_await success_int_future(42);
    REQUIRE(std::this_thread::get_id() == this_thread_id);
    x *= co_await ex1->dispatch_task([] { return 2; } YAZ_PRE_COMMA_LOCATION_STR);
    REQUIRE(std::this_thread::get_id() == thread1_id);
    x -= co_await ex1->dispatch_future([&]() -> Future<int> {
        co_await ex2->on(YAZ_LOCATION_STR);
        co_return 4;
    } YAZ_PRE_COMMA_LOCATION_STR);
    REQUIRE(std::this_thread::get_id() == thread2_id);
    co_return x;
}

int test_async_futures() {
    auto executor1 = concurrency::create_single_thread_executor();
    auto executor2 = concurrency::create_single_thread_executor();
    return test_async_futures_inner(executor1, executor2).get().value();
}

TEST_CASE( "future async + co_await  test", "[concurrency/future]" ) {
    REQUIRE(test_async_futures() == 80);
}
