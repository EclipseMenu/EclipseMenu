#pragma once

#include <string>
#include <Geode/cocos/include/ccTypes.h>

#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#include <nlohmann/json_fwd.hpp>
#endif

struct ImVec4;
typedef unsigned int ImU32;

namespace eclipse::gui {
    struct Color {
        static const Color WHITE;
        static const Color BLACK;
        static const Color RED;
        static const Color GREEN;
        static const Color BLUE;
        static const Color YELLOW;
        static const Color CYAN;
        static const Color MAGENTA;

        float r, g, b, a;

        Color() : r(0), g(0), b(0), a(1.0f) {}
        Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
        Color(const Color& other) = default;
        explicit Color(const cocos2d::ccColor4F& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}
        explicit Color(const cocos2d::ccColor4B& other) : r(other.r / 255.0f), g(other.g / 255.0f), b(other.b / 255.0f), a(other.a / 255.0f) {}

        Color(Color&& other) noexcept : r(other.r), g(other.g), b(other.b), a(other.a) {
            other.r = other.g = other.b = 0;
            other.a = 1.0f;
        }

        Color& operator=(const Color& other);

        Color& operator=(Color&& other) noexcept;

        /// @brief Converts the color to ImVec4
        operator ImVec4() const;

        /// @brief Converts the color to ImU32
        operator ImU32() const;

        Color& operator=(const ImVec4& col2);

        operator cocos2d::ccColor4F() const;
        operator cocos2d::ccColor4B() const;

        /// @brief Returns a pointer to the color data
        /// @return Pointer to the color data
        float* data() { return &r; }

        GLubyte getAlphaByte() const { return static_cast<GLubyte>(a * 255); }

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
        static Color fromHSV(const ImVec4& hsv);

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
        static Color fromString(const std::string& color, IntType type = IntType::RGBA);

        /// @brief Converts the color to a string
        /// @param type Integer type to convert to
        /// @return String color in format "RRGGBBAA"
        [[nodiscard]] std::string toString(IntType type = IntType::RGBA) const;

        /// @brief Converts the color to a CCColor3B
        /// @return CCColor3B color
        [[nodiscard]] cocos2d::ccColor3B toCCColor3B() const;

        /// @brief Creates a new color from a CCColor3B
        /// @param color CCColor3B color in proper format
        /// @return New color
        static Color fromCCColor3B(cocos2d::ccColor3B const& color);

        struct HSL {
            float h, s, l;

            HSL() : h(0), s(0), l(0) {}
            HSL(float h, float s, float l) : h(h), s(s), l(l) {}
            HSL(const HSL& other) = default;

            static HSL fromColor(const Color& color);
            static Color toColor(const HSL& hsl);

            operator Color() const { return toColor(*this); }
        };

        [[nodiscard]] HSL toHSL() const;
        [[nodiscard]] Color fromHSL(const HSL& hsl) const;

        /// @brief Gets the luminance of the color (0-1)
        [[nodiscard]] float luminance() const;

        [[nodiscard]] Color darken(float factor) const;
        [[nodiscard]] Color lighten(float factor) const;
    };

    void to_json(nlohmann::json& j, const Color& e);
    void from_json(const nlohmann::json& j, Color& e);
}
