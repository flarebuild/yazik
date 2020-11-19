#pragma once

#include <yazik/utility/result.hpp>
#include <yazik/utility/macro.hpp>
#include <catch2/catch_test_macros.hpp>

#define _YAZ_TEST_CASE(ID, ...) \
    ::yazik::Result<> ID(); \
    TEST_CASE(__VA_ARGS__) { \
        auto res = ID(); \
        if (!res)   \
            FAIL(res.error()); \
    } \
    ::yazik::Result<> ID()

#define YAZ_TEST_CASE(...) _YAZ_TEST_CASE( YAZ_CONCAT(__TEST_CASE, __LINE__ ), __VA_ARGS__)