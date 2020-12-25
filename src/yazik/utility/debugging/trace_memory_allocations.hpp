#pragma once

#include <new>
#include <typeinfo>
#include <atomic>
#include <folly/Unit.h>

#ifdef YAZ_TEST

namespace yazik::utility::debugging {

    template <typename T>
    std::atomic_size_t* traced_type_counter() {
        static T* s_mark = nullptr;
        static std::atomic_size_t s_counter { 0 };
        return &s_counter;
    }

    void print_traced();
    void* traced_new(std::atomic_size_t* counter, const std::type_info& type, size_t size);
    void traced_delete(std::atomic_size_t* counter, void* ptr);

} // end of ::yazik::utility::debugging namespace

#define $yaz_traced_alloc(x) \
    static void* operator new(size_t size) { \
        return ::yazik::utility::debugging::traced_new( \
            ::yazik::utility::debugging::traced_type_counter<x>(), \
            typeid(x), \
            size \
        ); \
    } \
    static void operator delete(void* ptr) { \
        ::yazik::utility::debugging::traced_delete( \
            ::yazik::utility::debugging::traced_type_counter<x>(), \
            ptr \
        ); \
    } \
    static void* operator new  ( std::size_t count, void* ptr ) { \
        return ptr; \
    }
#else
#define $yaz_traced_alloc(x)
#endif