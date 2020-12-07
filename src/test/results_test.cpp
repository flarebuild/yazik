#include <yazik/testlib/yaz_test_macros.hpp>
#include <yazik/utility/utility_defs.hpp>

using namespace yazik;

Result<int> success_int(int res) {
    co_return res;
}

Result<int> failed_int(int res) {
    co_await yaz_fail<string>("test");
    co_return res;
}

Result<int> test_succeeded() {
    int x = co_await success_int(42);
    x *= co_await success_int(2);
    x -= co_await success_int(4);
    co_return x;
}

Result<int> test_unsucceed() {
    int x = co_await success_int(42);
    x *= co_await failed_int(2);
    x -= co_await success_int(4);
    co_return x;
}

Result<void> test_void_unsucceed_from_int() {
    int x = co_await success_int(42);
    x *= co_await failed_int(2);
    x -= co_await success_int(4);
    co_return;
}

Result<void> success() {
    co_return;
}

Result<void> failed() {
    co_await yaz_fail("test");
    co_return;
}

Result<void> thrown() {
    co_await success();
    throw std::runtime_error("test");
    co_return;
}

struct IllegalCustomError {};

struct CustomError {
    CustomError() {}
    CustomError(detail::UnxepectedStringTag, string) {}
    string to_error_string() const { return {}; }
};

Result<void, CustomError> custom_error_success() {
    co_return;
}

Result<void, CustomError> custom_error_fail() {
    co_await yaz_fail<string>("test");
    co_return;
}


Result<void> custom_error_success_result_test() {
    co_await custom_error_success();
    co_return;
}

Result<void> custom_error_fail_result_test() {
    co_await custom_error_fail();
    co_return;
}

TEST_CASE( "results + co_await  test", "[utility/result]" ) {
    REQUIRE(test_succeeded().value() == 80);
    REQUIRE(test_unsucceed().error() ==  "test");
    REQUIRE(test_void_unsucceed_from_int().error() ==  "test");
    REQUIRE(thrown().error() ==  "unhandled_exception: test");

    REQUIRE_FALSE(detail::c_is_legal_result_error<IllegalCustomError>);
    REQUIRE(detail::c_is_legal_result_error<CustomError>);

    REQUIRE(custom_error_success_result_test());
    REQUIRE_FALSE(custom_error_fail_result_test());
}