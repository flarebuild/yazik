#pragma once

#include <yazik/utility/result.hpp>
#include <yazik/concurrency/future_result.hpp>
#include <yazik/concurrency/scheduler.hpp>
#include <yazik/utility/macro.hpp>

#define CATCH_CONFIG_PREFIX_ALL
#include <catch2/catch_test_macros.hpp>

#define TEST_CASE CATCH_TEST_CASE
#define REQUIRE CATCH_REQUIRE
#define REQUIRE_FALSE CATCH_REQUIRE_FALSE

#define _YAZ_R_TEST_CASE(ID, ...) \
    ::yazik::Result<> ID(); \
    TEST_CASE(__VA_ARGS__) { \
        auto res = ID(); \
        if (!res)   \
            CATCH_FAIL(res.error()); \
    } \
    ::yazik::Result<> ID()

#define YAZ_R_TEST_CASE(...) _YAZ_R_TEST_CASE( YAZ_CONCAT(__TEST_CASE, __LINE__ ), __VA_ARGS__)

#define _YAZ_F_TEST_CASE(ID, ...) \
    ::yazik::Task<> ID(::yazik::concurrency::scheduler_ptr_t); \
    TEST_CASE(__VA_ARGS__) { \
        auto work = ::yazik::concurrency::create_asio_scheduler(); \
        ::yazik::concurrency::mark_default_executor(work); \
        auto res = work->run_until_done(ID(work)); \
        if (!res)   \
            CATCH_FAIL(res.error()); \
    } \
    ::yazik::Task<> ID(::yazik::concurrency::scheduler_ptr_t __scheduler)

#define YAZ_F_TEST_CASE(...) _YAZ_F_TEST_CASE( YAZ_CONCAT(__TEST_CASE, __LINE__ ), __VA_ARGS__)