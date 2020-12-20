#include <yazik/testlib/yaz_test_macros.hpp>
#include <yazik/concurrency/generator.hpp>

using namespace yazik;
using namespace yazik::concurrency;

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

Generator<int> fibonacci_generator(const int ceiling) {
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

Result<> test_generator_result() {
    int ceiling = fibonacci(42);
    int i = 0;
    for (int value: fibonacci_generator(ceiling)) {
        int calculated = fibonacci(i);
        REQUIRE(value == calculated);
        ++i;
    }
    co_return;
}

Future<> test_generator_future() {
    int ceiling = fibonacci(42);
    int i = 0;
    for (int value: fibonacci_generator(ceiling)) {
        int calculated = fibonacci(i);
        REQUIRE(value == calculated);
        ++i;
    }
    co_return;
}

//TEST_CASE( "generator test", "[concurrency/generator]" ) {
//    REQUIRE(test_generator_result());
//    REQUIRE(test_generator_future().get());
//}

Generator<int> successful_generator() {
    for (int i = 0; i < 4; ++i)
        co_yield i;
    co_return;
}

Generator<int> unsuccessful_generator() {
    for (int i = 0; i < 2; ++i)
        co_yield i;
    co_await yaz_fail<string>("noop");
    co_yield 1;
    co_return;
}

Generator<int> unsuccessful_throw_generator() {
    for (int i = 0; i < 2; ++i)
        co_yield i;
    throw std::runtime_error("noop");
    co_yield 1;
    co_return;
}

TEST_CASE( "generator result test", "[concurrency/generator]" ) {
    {
        auto suc_gen = successful_generator();
        for (int _: suc_gen){}
        REQUIRE(suc_gen.result());
    } {
        auto unsuc_gen = unsuccessful_generator();
        try { for (int _: unsuc_gen){} } catch (...) {}
        REQUIRE(unsuc_gen.result().hasError());
    } {
        auto unsuc_gen = unsuccessful_throw_generator();
        try { for (int _: unsuc_gen){} } catch (...) {}
        REQUIRE(unsuc_gen.result().hasError());
    }

}