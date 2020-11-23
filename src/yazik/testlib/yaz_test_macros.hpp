#pragma once

#include <yazik/utility/result.hpp>
#include <yazik/concurrency/future_result.hpp>
#include <yazik/concurrency/scheduler.hpp>
#include <yazik/utility/macro.hpp>
#include <catch2/catch_test_macros.hpp>

#define _YAZ_TEST_CASE(ID, ...) \
    ::yazik::Result<> ID(); \
    TEST_CASE(__VA_ARGS__) { \
        auto res = ID(); \
        fflush(stdout); \
        if (!res)   \
            FAIL(res.error()); \
    } \
    ::yazik::Result<> ID()

#define YAZ_TEST_CASE(...) _YAZ_TEST_CASE( YAZ_CONCAT(__TEST_CASE, __LINE__ ), __VA_ARGS__)

#define _YAZ_F_TEST_CASE(ID, ...) \
    ::yazik::Future<> ID(::yazik::concurrency::scheduler_ptr_t); \
    TEST_CASE(__VA_ARGS__) { \
        auto work = ::yazik::concurrency::create_asio_scheduler(); \
        work->mark_thread(); \
        auto res = work->run_until_done(ID(work)).get(); \
        if (!res)   \
            FAIL(res.error()); \
    } \
    ::yazik::Future<> ID(::yazik::concurrency::scheduler_ptr_t __scheduler)

#define YAZ_F_TEST_CASE(...) _YAZ_F_TEST_CASE( YAZ_CONCAT(__TEST_CASE, __LINE__ ), __VA_ARGS__)