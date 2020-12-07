#include <yazik/testlib/yaz_test_macros.hpp>
#include <yazik/utility/utility_defs.hpp>

using namespace yazik;

Task<int> success_int_task(int res) {
    co_return res;
}

Task<int> failed_int_task(int res) {
    co_await yaz_fail<string>("test");
    co_return res;
}

Task<int> test_succeeded_task() {
    int x = co_await success_int_task(42);
    x *= co_await success_int_task(2);
    x -= co_await success_int_task(4);
    co_return x;
}

Task<int> test_unsucceed_task() {
    int x = co_await success_int_task(42);
    x *= co_await failed_int_task(2);
    x -= co_await success_int_task(4);
    co_return x;
}

Task<void> test_void_unsucceed_from_int_task() {
    int x = co_await success_int_task(42);
    x *= co_await failed_int_task(2);
    x -= co_await success_int_task(4);
    co_return;
}

Task<void> success_task() {
    co_return;
}

Task<void> failed_task() {
    co_await yaz_fail("test");
    co_return;
}

Task<void> thrown_task() {
    co_await success_task();
    throw std::runtime_error("test");
    co_return;
}

template <typename T>
string get_task_error(Task<T>&& f) {
    return f.fut().get().error();
}

TEST_CASE( "task + co_await  test", "[concurrency/task]" ) {
    REQUIRE(test_succeeded_task().fut().get().value() == 80);
    REQUIRE(get_task_error(test_unsucceed_task()) == "test");
    REQUIRE(get_task_error(test_void_unsucceed_from_int_task()) == "test");
    REQUIRE(get_task_error(thrown_task()) ==  "unhandled_exception: test");
}

Task<void> test_task_result() {
    Result<int> x = co_await success_int_task(42).wrapped();
    REQUIRE(x);
    REQUIRE(x.value() == 42);

    x = co_await failed_int_task(2).wrapped();
    REQUIRE_FALSE(x);
    REQUIRE(x.error() ==  "test");

    co_return;
}

TEST_CASE( "task result  test", "[concurrency/task]" ) {
    test_task_result().fut().wait();
}

Result<int> some_int_result() {
    co_return 42;
}

Result<> some_result() {
    int x = 0;
    x += co_await some_int_result();
    x += co_await some_int_result();
    co_return;
}

Task<> test_task_result_executor(const concurrency::executor_ptr_t& ex) {
    return ex->dispatch_future([]() -> Task<>{
        co_await some_result();
        co_return;
    } YAZ_PRE_COMMA_LOCATION_STR);
}

TEST_CASE( "task result executor test", "[concurrency/task]" ) {
    auto ex = concurrency::create_single_thread_executor();
    REQUIRE(test_task_result_executor(ex).fut().get());
}

Task<int> test_async_tasks_inner(
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

    int x = co_await success_int_task(42);
    REQUIRE(std::this_thread::get_id() == this_thread_id);
    x *= co_await ex1->dispatch_task([] { return 2; } YAZ_PRE_COMMA_LOCATION_STR);
    REQUIRE(std::this_thread::get_id() == thread1_id);
    x -= co_await ex1->dispatch_future([&]() -> Task<int> {
        co_await ex2->on(YAZ_LOCATION_STR);
        co_return 4;
    } YAZ_PRE_COMMA_LOCATION_STR);
    REQUIRE(std::this_thread::get_id() == thread2_id);
    co_return x;
}

int test_async_tasks() {
    auto executor1 = concurrency::create_single_thread_executor();
    auto executor2 = concurrency::create_single_thread_executor();
    return test_async_tasks_inner(executor1, executor2).fut().get().value();
}

TEST_CASE( "task async + co_await  test", "[concurrency/task]" ) {
    REQUIRE(test_async_tasks() == 80);
}