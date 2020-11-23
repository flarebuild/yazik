#pragma once

#define YAZ_PLAIN(x) x

#define _YAZ_STRINGIFY_(x) #x
#define YAZ_STRINGIFY(x) _YAZ_STRINGIFY_(x)

#define _YAZ_CONCAT_(x,y) x##y
#define YAZ_CONCAT(x,y) _YAZ_CONCAT_(x,y)

#define _l_move(x) x = std::move(x)
#define _l_forward(x) x = std::forward<decltype(x)>(x)

#define $no(...)
#define $yes(...)        __VA_ARGS__

#if defined(NDEBUG) || defined(_NDEBUG) || defined(RELEASE)
#   define $yaz_release   $yes
#   define $yaz_debug     $no
#   define $breakpoint_hint
#   define YAZ_LOCATION_STR
#   define YAZ_PRE_COMMA_LOCATION_STR
#   define YAZ_DEBUG 0
#else
#   define $yaz_release   $no
#   define $yaz_debug     $yes
#   define $breakpoint_hint int YAZ_CONCAT(__HINT_,__LINE__) = 0; ++ YAZ_CONCAT(__HINT_,__LINE__);
#   define YAZ_LOCATION_STR __FILE__ ":" YAZ_STRINGIFY(__LINE__)
#   define YAZ_PRE_COMMA_LOCATION_STR , YAZ_LOCATION_STR
#   define YAZ_DEBUG 1
#endif

#ifdef YAZ_TEST
#   define $yaz_test   $yes
#else
#   define $yaz_test   $no
#endif