#pragma once

#include <type_traits>
#include <iterator>

namespace yazik::concepts {

    template <class T, class U>
    concept c_same_as = std::is_same_v<T, U> && std::is_same_v<U, T>;

    template <class T, class U>
    concept c_not_same_as = !c_same_as<T, U>;

    template <class From, class To>
    concept c_just = std::is_convertible_v<From, To>
        && requires(std::add_rvalue_reference_t<From> (&f)()) {
        static_cast<To>(f());
    };

    template<class I>
    concept c_incrementable = requires(I i) {
        { ++i } -> c_same_as<I&>;
        i++;
    };

    template <class T>
    concept c_constant_forward_iterator = c_incrementable<T>;

    template <class T>
    concept c_iterable = requires(const T a) {
        { a.begin() } -> c_constant_forward_iterator;
        { a.end() } -> c_constant_forward_iterator;
    };

    template <class T, class I>
    concept c_iterable_just = c_iterable<T>
        && c_just<decltype(*T{}.begin()), I>;

    template <class T>
    concept c_with_c_str = requires(const T& a) {
        { a.c_str() } -> c_same_as<const char*>;
    };

} // end of ::yazik::concepts namespace