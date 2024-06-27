#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <cmath>
#include <stdio.h>

#include <fmt/format.h>

namespace eclipse::gui {
    struct Color {
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
            return ((int) (r * 255) << 16) | ((int) (g * 255) << 8) | (int) (b * 255) | ((int) (a * 255) << 24);
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
    };

    void to_json(nlohmann::json &j, const Color &e) {
        auto str = e.toString();
        j = str;
    }

    void from_json(const nlohmann::json &j, Color &e) {
        e = Color::fromString(j.get<std::string>().c_str());
    }
}