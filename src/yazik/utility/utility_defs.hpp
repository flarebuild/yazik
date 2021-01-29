#pragma once

#include <new>

#include <folly/container/F14Set.h>
#include <folly/container/F14Map.h>
#include <folly/Format.h>
#include <folly/FBString.h>
#include <folly/FBVector.h>
#include <folly/Conv.h>
#include <folly/Function.h>
#include <folly/Try.h>
#include <folly/json.h>

#include <range/v3/all.hpp>

#include <yazik/utility/concepts.hpp>

namespace yazik {
    template <typename T>
    using vector = ::folly::fbvector<T>;

    template <typename T, typename Hasher = ::folly::f14::DefaultHasher<T>>
    using set = ::folly::F14FastSet<T, Hasher>;

    template <typename K, typename V>
    using map = ::folly::F14FastMap<K, V>;

    using string = ::folly::basic_fbstring<char>;

    using string_view = ::folly::StringPiece;

    using ::folly::to;

    template <typename... Signatures>
    using unique_function = ::folly::Function<Signatures...>;

    template <class... Args>
    inline string do_format(string_view fmt, Args&&... args) {
        return ::folly::format(fmt, std::forward<Args>(args)...).fbstr();
    }

    template <class... Args>
    inline std::string do_sformat(string_view fmt, Args&&... args) {
        return ::folly::format(fmt, std::forward<Args>(args)...).str();
    }

    constexpr auto $1 = std::placeholders::_1;
    constexpr auto $2 = std::placeholders::_2;

    template<typename T>
    using Try = ::folly::Try<T>;

    using dynamic = ::folly::dynamic;

    namespace views = ::ranges::views;

} // end of ::yazik namespace

namespace std {
    template <>
    struct hash<::yazik::string_view> {
        std::size_t operator()(const ::yazik::string_view& k) const {
            return std::hash<string_view> {} ( string_view { k.data(), k.size() } );
        }
    };
}
