#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <cmath>
#include <cstdio>
#include <nlohmann/json.hpp>

#include <fmt/format.h>

namespace eclipse::gui {
    struct Color {
        // Define default colors
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

        /// @brief Converts the color to ImVec4
        operator ImVec4() const {
            return {r, g, b, a};
        }

        /// @brief Converts the color to ImU32
        operator ImU32() const {
            return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
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

        /// @brief Gets the color from an integer
        /// @param color Integer color
        /// @return New color
        static Color fromInt(int color) {
            return {
                    (float) ((color >> 16) & 0xFF) / 255.0f,
                    (float) ((color >> 8) & 0xFF) / 255.0f,
                    (float) (color & 0xFF) / 255.0f,
                    (float) ((color >> 24) & 0xFF) / 255.0f};
        }

        /// @brief Converts the color to an integer
        /// @return Integer color
        [[nodiscard]] int toInt() const {
            return ((int) (r * 255) << 24) | ((int) (g * 255) << 16) | (int) (b * 255) << 8 | ((int) (a * 255));
        }

        /// @brief Creates a new color from a string
        /// @param color String color in format "RRGGBBAA"
        /// @return New color
        static Color fromString(const char *color) {
            uint32_t c;
            sscanf(color, "%X", &c);
            return fromInt(c);
        }

        /// @brief Converts the color to a string
        /// @return String color in format "RRGGBBAA"
        [[nodiscard]] std::string toString() const {
            uint32_t c = toInt();
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