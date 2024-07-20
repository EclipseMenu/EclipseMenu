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

        Color(const Color &other) : r(other.r), g(other.g), b(other.b), a(other.a) {}

        Color(Color &&other) noexcept : r(other.r), g(other.g), b(other.b), a(other.a) {
            other.r = other.g = other.b = 0;
            other.a = 1.0f;
        }

        Color& operator=(const Color &other) {
            if (this == &other) return *this;
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
            return *this;
        }

        Color& operator=(Color &&other) noexcept {
            if (this == &other) return *this;
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
            other.r = other.g = other.b = 0;
            other.a = 1.0f;
            return *this;
        }

        /// @brief Converts the color to ImVec4
        operator ImVec4() const {
            return {r, g, b, a};
        }

        /// @brief Converts the color to ImU32
        operator ImU32() const {
            return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
        }

        Color& operator=(const ImVec4 &col2) {
            r = col2.x;
            g = col2.y;
            b = col2.z;
            a = col2.w;
            return *this;
        }

        operator cocos2d::ccColor4F() const {
            return {r, g, b, a};
        }

        /// @brief Returns a pointer to the color data
        /// @return Pointer to the color data
        float *data() {
            return &r;
        }

        /// @brief Creates a new color from HSV values
        /// @param h Hue
        /// @param s Saturation
        /// @param v Value
        /// @param a Alpha
        /// @return New color
        static Color fromHSV(float h, float s, float v, float a = 1.0f) {
            float c = v * s;
            float x = c * (1 - std::abs(fmod(h / 60.0f, 2) - 1));
            float m = v - c;

            float r, g, b;
            if (h < 60) {
                r = c;
                g = x;
                b = 0;
            } else if (h < 120) {
                r = x;
                g = c;
                b = 0;
            } else if (h < 180) {
                r = 0;
                g = c;
                b = x;
            } else if (h < 240) {
                r = 0;
                g = x;
                b = c;
            } else if (h < 300) {
                r = x;
                g = 0;
                b = c;
            } else {
                r = c;
                g = 0;
                b = x;
            }

            return {r + m, g + m, b + m, a};
        }

        /// @brief Creates a new color from HSV values
        /// @param hsv HSV values
        /// @return New color
        static Color fromHSV(const ImVec4 &hsv) {
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
        static Color fromInt(int color, IntType type = IntType::RGBA) {
            float v1, v2, v3, v4;
            v1 = (float) ((color >> 24) & 0xFF) / 255.0f;
            v2 = (float) ((color >> 16) & 0xFF) / 255.0f;
            v3 = (float) ((color >> 8) & 0xFF) / 255.0f;
            v4 = (float) (color & 0xFF) / 255.0f;

            switch (type) {
                default:
                    return {v1, v2, v3, v4};
                case IntType::ARGB:
                    return {v2, v3, v4, v1};
                case IntType::ABGR:
                    return {v4, v3, v2, v1};
                case IntType::BGRA:
                    return {v3, v2, v1, v4};
            }
        }

        /// @brief Converts the color to an integer
        /// @param type Integer type to convert to
        /// @return Integer color
        [[nodiscard]] int toInt(IntType type = IntType::RGBA) const {
            int rv, gv, bv, av;
            rv = static_cast<int>(this->r * 255);
            gv = static_cast<int>(this->g * 255);
            bv = static_cast<int>(this->b * 255);
            av = static_cast<int>(this->a * 255);
            switch (type) {
                default:
                    return (rv << 24) | (gv << 16) | (bv << 8) | av;
                case IntType::ARGB:
                    return (av << 24) | (rv << 16) | (gv << 8) | bv;
                case IntType::ABGR:
                    return (av << 24) | (bv << 16) | (gv << 8) | rv;
                case IntType::BGRA:
                    return (bv << 24) | (gv << 16) | (rv << 8) | av;
            }
        }

        /// @brief Creates a new color from a string
        /// @param color String color in proper format
        /// @param type Integer type to convert from
        /// @return New color
        static Color fromString(const std::string& color, IntType type = IntType::RGBA) {
            uint32_t c = std::strtoul(color.c_str(), nullptr, 16);
            return fromInt(c, type);
        }

        /// @brief Converts the color to a string
        /// @param type Integer type to convert to
        /// @return String color in format "RRGGBBAA"
        [[nodiscard]] std::string toString(IntType type = IntType::RGBA) const {
            uint32_t c = toInt(type);
            return fmt::format("{:08X}", c);
        }

        /// @brief Converts the color to a CCColor3B
        /// @return CCColor3B color
        [[nodiscard]] cocos2d::ccColor3B toCCColor3B() const {
            return {
                static_cast<uint8_t>(r * 255),
                static_cast<uint8_t>(g * 255),
                static_cast<uint8_t>(b * 255)
            };
        }
    };

    void to_json(nlohmann::json &j, const Color &e);
    void from_json(const nlohmann::json &j, Color &e);
}
