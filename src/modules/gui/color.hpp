#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <cmath>
#include <nlohmann/json.hpp>

#include <fmt/format.h>

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

        /// @brief Returns a pointer to the color data
        /// @return Pointer to the color data
        inline float* data() {
            return &r;
        }

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
        inline static Color fromHSV(const ImVec4& hsv) {
            return fromHSV(hsv.x, hsv.y, hsv.z, hsv.w);
        }

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
    };

    void to_json(nlohmann::json& j, const Color& e);
    void from_json(const nlohmann::json& j, Color& e);
}
