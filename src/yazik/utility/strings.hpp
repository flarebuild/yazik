#pragma once

#include <string_view>

#include <yazik/utility/utility_defs.hpp>
#include <yazik/utility/result.hpp>
#include <yazik/utility/macro.hpp>

namespace yazik {

namespace strings {

    template<char Delimiter>
    vector<string_view> split(const string& source) {
        vector<string_view> result;

        int index_comma_to_left_of_column = 0;
        int index_comma_to_right_of_column = -1;

        for (int i=0; i<static_cast<int>(source.size()); i++) {
            if (source[i] == Delimiter) {
                index_comma_to_left_of_column = index_comma_to_right_of_column;
                index_comma_to_right_of_column = i;
                int index = index_comma_to_left_of_column + 1;
                int length = index_comma_to_right_of_column - index;

                string_view column(source.data() + index, length);
                result.push_back(column);
            }
        }
        const string_view final_column(
            source.data() + index_comma_to_right_of_column + 1,
            source.size() - index_comma_to_right_of_column - 1
        );
        result.push_back(final_column);
        return result;
    }

    Result<std::size_t> size_from_string_view(string_view source);

    template<char ... Chars>
    struct FS {
        static constexpr const char value[sizeof...(Chars)+1] = {Chars...,'\0'};
        static constexpr int size = sizeof...(Chars);
    };

    template<char ... Chars>
    constexpr const char FS<Chars...>::value[sizeof...(Chars)+1];

} // end of ::yazik::strings namespace

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-string-literal-operator-template"

    template <typename T, T... Chars>
    constexpr strings::FS<Chars...> operator""_fs() { return { }; }

#pragma GCC diagnostic pop

} // end of ::yazik namespace

#define _fs(s) decltype(YAZ_CONCAT(s,_fs))