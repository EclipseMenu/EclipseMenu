#pragma once

#include <string>
#include <Geode/cocos/include/ccTypes.h>

#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#include <nlohmann/json_fwd.hpp>
#endif

struct ImVec4;
using ImU32 = unsigned int;

namespace eclipse::gui {
    struct Color {
        float r, g, b, a;

        constexpr Color() : r(0), g(0), b(0), a(1.0f) {}
        constexpr Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
        constexpr Color(Color const& other) = default;
        constexpr Color(Color const& other, float a) : r(other.r), g(other.g), b(other.b), a(a) {}
        explicit constexpr Color(cocos2d::ccColor4F const& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}
        explicit constexpr Color(cocos2d::ccColor4B const& other) : r(other.r / 255.0f), g(other.g / 255.0f), b(other.b / 255.0f), a(other.a / 255.0f) {}

        constexpr Color(Color&& other) noexcept : r(other.r), g(other.g), b(other.b), a(other.a) {
            other.r = other.g = other.b = 0;
            other.a = 1.0f;
        }

        constexpr Color& operator=(Color const& other) = default;
        constexpr Color& operator=(Color&& other) noexcept = default;

        /// @brief Converts the color to ImVec4
        operator ImVec4() const;

        /// @brief Converts the color to ImU32
        operator ImU32() const;

        Color& operator=(ImVec4 const& col2);

        constexpr operator cocos2d::ccColor4F() const {
            return {r, g, b, a};
        }

        constexpr operator cocos2d::ccColor4B() const {
            return {
                static_cast<uint8_t>(r * 255),
                static_cast<uint8_t>(g * 255),
                static_cast<uint8_t>(b * 255),
                static_cast<uint8_t>(a * 255)
            };
        }

        /// @brief Returns a pointer to the color data
        /// @return Pointer to the color data
        constexpr float* data() { return &r; }

        constexpr GLubyte getAlphaByte() const { return static_cast<GLubyte>(a * 255); }

        /// @brief Creates a new color from HSV values
        /// @param h Hue
        /// @param s Saturation
        /// @param v Value
        /// @param a Alpha
        /// @return New color
        static Color fromHSV(float h, float s, float v, float a = 1.0f);

        /// @brief Creates a new color from HSV values
        /// @param hsv HSV values
        /// @return New color
        static Color fromHSV(ImVec4 const& hsv);

        enum class IntType {
            RGBA, ARGB,
            ABGR, BGRA
        };

        /// @brief Gets the color from an integer
        /// @param color Integer color
        /// @param type Integer type to convert from
        /// @return New color
        static Color fromInt(int color, IntType type = IntType::RGBA);

        /// @brief Converts the color to an integer
        /// @param type Integer type to convert to
        /// @return Integer color
        [[nodiscard]] int toInt(IntType type = IntType::RGBA) const;

        /// @brief Creates a new color from a string
        /// @param color String color in proper format
        /// @param type Integer type to convert from
        /// @return New color
        static Color fromString(std::string_view color, IntType type = IntType::RGBA);

        /// @brief Converts the color to a string
        /// @param type Integer type to convert to
        /// @return String color in format "RRGGBBAA"
        [[nodiscard]] std::string toString(IntType type = IntType::RGBA) const;

        /// @brief Converts the color to a CCColor3B
        /// @return CCColor3B color
        [[nodiscard]] constexpr cocos2d::ccColor3B toCCColor3B() const {
            return {
                static_cast<uint8_t>(r * 255),
                static_cast<uint8_t>(g * 255),
                static_cast<uint8_t>(b * 255)
            };
        }

        /// @brief Creates a new color from a CCColor3B
        /// @param color CCColor3B color in proper format
        /// @return New color
        static constexpr Color fromCCColor3B(cocos2d::ccColor3B const& color) {
            return Color(color.r / 255.F, color.g / 255.F, color.b / 255.F);
        }

        struct HSL {
            float h, s, l;

            constexpr HSL() : h(0), s(0), l(0) {}
            constexpr HSL(float h, float s, float l) : h(h), s(s), l(l) {}
            constexpr HSL(HSL const& other) = default;

            static HSL fromColor(Color const& color);
            static Color toColor(HSL const& hsl);

            constexpr operator Color() const { return toColor(*this); }
        };

        [[nodiscard]] HSL toHSL() const;
        [[nodiscard]] static Color fromHSL(HSL const& hsl);

        /// @brief Gets the luminance of the color (0-1)
        [[nodiscard]] float luminance() const;

        [[nodiscard]] Color darken(float factor) const;
        [[nodiscard]] Color lighten(float factor) const;
    };

    namespace Colors {
        constexpr Color WHITE = {1, 1, 1};
        constexpr Color BLACK = {0, 0, 0};
        constexpr Color RED = {1, 0, 0};
        constexpr Color GREEN = {0, 1, 0};
        constexpr Color BLUE = {0, 0, 1};
        constexpr Color YELLOW = {1, 1, 0};
        constexpr Color CYAN = {0, 1, 1};
        constexpr Color MAGENTA = {1, 0, 1};
    }

    void to_json(nlohmann::json& j, Color const& e);
    void from_json(nlohmann::json const& j, Color& e);
}
