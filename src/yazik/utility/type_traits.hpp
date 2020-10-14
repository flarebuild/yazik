#pragma once

#include <type_traits>
#include <yazik/utility/type_list.hpp>

namespace yazik::utility {
    template <typename... Values>
    struct c_all_of;

    template <typename Value>
    struct c_all_of<Value> {
        static constexpr auto value = Value::value;
    };

    template <typename Value, typename... Values>
    struct c_all_of<Value, Values...> {
        static constexpr auto value = Value::value && c_all_of<Values...>::value;
    };

    template <typename... Values>
    struct c_any_of;

    template <typename Value>
    struct c_any_of<Value> {
        static constexpr auto value = Value::value;
    };

    template <typename Value, typename... Values>
    struct c_any_of<Value, Values...> {
        static constexpr auto value = Value::value || c_all_of<Values...>::value;
    };

    template <class T>
    class NonCopyable {
    public:
        NonCopyable (const NonCopyable &) = delete;
        T & operator = (const T &) = delete;
        NonCopyable(NonCopyable&&) = delete;
        NonCopyable &operator=(const NonCopyable&) = delete;
        NonCopyable &operator=(NonCopyable&&) = delete;

    protected:
        NonCopyable () = default;
        ~NonCopyable () = default; /// Protected non-virtual destructor
    };

    template <class T>
    using decay_t = typename std::decay<T>::type;

namespace detail {

    /// Defines `result_type,` `arg_types,` and `fun_type`. Functor is
    ///    (a) a member function pointer, (b) a function,
    ///    (c) a function pointer, (d) an std::function.
    ///
    /// `result_type` is the result type found in the signature.
    /// `arg_types` are the argument types as {@link type_list}.
    /// `fun_type` is an std::function with an equivalent signature.
    template <class Functor>
    struct callable_trait;

    // good ol' function
    template <class R, class... Ts>
    struct callable_trait<R (Ts...)> {
        using result_type = R;
        using arg_types = type_list<Ts...>;
        using fun_sig = R (Ts...);
        using fun_type = std::function<R (Ts...)>;
        static constexpr size_t num_args = sizeof...(Ts);
    };

    // member noexcept const function pointer
    template <class C, typename R, class... Ts>
    struct callable_trait<R (C::*)(Ts...) const noexcept>
    : callable_trait<R(Ts...)> {};

    // member noexcept function pointer
    template <class C, typename R, class... Ts>
    struct callable_trait<R (C::*)(Ts...) noexcept> : callable_trait<R(Ts...)> {};

    // member const function pointer
    template <class C, typename R, class... Ts>
    struct callable_trait<R (C::*)(Ts...) const> : callable_trait<R (Ts...)> {};

    // member function pointer
    template <class C, typename R, class... Ts>
    struct callable_trait<R (C::*)(Ts...)> : callable_trait<R (Ts...)> {};

    // good ol' noexcept function pointer
    template <class R, class... Ts>
    struct callable_trait<R (*)(Ts...) noexcept> : callable_trait<R(Ts...)> {};

    // good ol' function pointer
    template <class R, class... Ts>
    struct callable_trait<R (*)(Ts...)> : callable_trait<R (Ts...)> {};

    template <class T>
    struct has_apply_operator {
        template <class U>
        static auto sfinae(U*) -> decltype(&U::operator(), std::true_type());

        template <class U>
        static auto sfinae(...) -> std::false_type;

        using type = decltype(sfinae<T>(nullptr));
        static constexpr bool value = type::value;
    };

    // matches (IsFun || IsMemberFun)
    template <class T,
              bool IsFun = std::is_function<T>::value
                           || std::is_function<
                                typename std::remove_pointer<T>::type
                              >::value
                           || std::is_member_function_pointer<T>::value,
              bool HasApplyOp = has_apply_operator<T>::value>
    struct get_callable_trait_helper {
        using type = callable_trait<T>;
        using result_type = typename type::result_type;
        using arg_types = typename type::arg_types;
        using fun_type = typename type::fun_type;
        using fun_sig = typename type::fun_sig;
        static constexpr size_t num_args = tl_size<arg_types>::value;
    };

    // assume functor providing operator()
    template <class T>
    struct get_callable_trait_helper<T, false, true> {
        using type = callable_trait<decltype(&T::operator())>;
        using result_type = typename type::result_type;
        using arg_types = typename type::arg_types;
        using fun_type = typename type::fun_type;
        using fun_sig = typename type::fun_sig;
        static constexpr size_t num_args = tl_size<arg_types>::value;
    };

    template <class T>
    struct get_callable_trait_helper<T, false, false> {};

    /// Gets a callable trait for `T,` where `T` is a function object type,
    /// i.e., a function, member function, or a class providing
    /// the call operator.
    template <class T>
    struct get_callable_trait : get_callable_trait_helper<decay_t<T>> {};

    template <class T>
    using get_callable_trait_t = typename get_callable_trait<T>::type;

} // end of ::yazik::utility::detail namespace

    /// Checks whether `T` is a function or member function.
    template <class T>
    struct is_callable {
        template <class C>
        static bool _fun(C*, typename detail::get_callable_trait<C>::type* = nullptr);

        static void _fun(void*);

        using result_type = decltype(_fun(static_cast<decay_t<T>*>(nullptr)));

    public:
        static constexpr bool value = std::is_same<bool, result_type>::value;
    };

    /// Checks whether `F` is callable with arguments of types `Ts...`.
    template <class F, class... Ts>
    struct is_callable_with {
        template <class U>
        static auto sfinae(U*)
        -> decltype((std::declval<U&>())(std::declval<Ts>()...), std::true_type());

        template <class U>
        static auto sfinae(...) -> std::false_type;

        using type = decltype(sfinae<F>(nullptr));
        static constexpr bool value = type::value;
    };

    template <typename T>
    struct WithSuper: T {
        using Super = T;
        using Super::Super;
    };

    /// Checks whether `T1` is comparable with `T2`.
    template <class T1, class T2>
    class is_comparable {
        // SFINAE: If you pass a "bool*" as third argument, then
        //     decltype(cmp_help_fun(...)) is bool if there's an
        //     operator==(A, B) because
        //     cmp_help_fun(A*, B*, bool*) is a better match than
        //     cmp_help_fun(A*, B*, void*). If there's no operator==(A, B)
        //     available, then cmp_help_fun(A*, B*, void*) is the only
        //     candidate and thus decltype(cmp_help_fun(...)) is void.
        template <class A, class B>
        static bool cmp_help_fun(
            const A* arg0,
            const B* arg1,
            decltype(*arg0 == *arg1)*,
            std::integral_constant<bool, false>
        );

        // silences float-equal warnings caused by decltype(*arg0 == *arg1)
        template <class A, class B>
        static bool cmp_help_fun(
            const A*,
            const B*,
            bool*,
            std::integral_constant<bool, true>
        );

        template <class A, class B, class C>
        static void cmp_help_fun(
            const A*,
            const B*,
            void*,
            C
        );

        using result_type = decltype(cmp_help_fun(
            static_cast<T1*>(nullptr),
            static_cast<T2*>(nullptr),
            static_cast<bool*>(nullptr),
            std::integral_constant<bool, std::is_arithmetic<T1>::value
                && std::is_arithmetic<T2>::value>{}
        ));

    public:
        static constexpr bool value = std::is_same<bool, result_type>::value;
    };

    template<class T>
    struct is_c_str
    : std::integral_constant<
        bool,
        std::is_same<char const *, typename std::decay<T>::type>::value ||
        std::is_same<char *, typename std::decay<T>::type>::value
    > {};

} // end of ::yazik::utility namespace