#pragma once
#include <algorithm>
#include <string_view>

namespace eclipse {
    template <size_t N>
    struct FixedString {
        char data[N + 1]{};
        explicit(false) constexpr FixedString(char const* str) { std::copy_n(str, N + 1, data); }
        constexpr operator std::string_view() const { return { data, N }; }
    };

    template <size_t N>
    FixedString(char const (&)[N]) -> FixedString<N - 1>;
}