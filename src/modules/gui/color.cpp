#include "color.hpp"
#include <imgui.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace eclipse::gui {
    Color const Color::WHITE = {1, 1, 1};
    Color const Color::BLACK = {0, 0, 0};
    Color const Color::RED = {1, 0, 0};
    Color const Color::GREEN = {0, 1, 0};
    Color const Color::BLUE = {0, 0, 1};
    Color const Color::YELLOW = {1, 1, 0};
    Color const Color::CYAN = {0, 1, 1};
    Color const Color::MAGENTA = {1, 0, 1};

    Color::operator ImU32() const {
        return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
    }

    Color& Color::operator=(Color const& other) {
        if (this == &other) return *this;
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        return *this;
    }

    Color& Color::operator=(Color&& other) noexcept {
        if (this == &other) return *this;
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        other.r = other.g = other.b = 0;
        other.a = 1.0f;
        return *this;
    }

    Color::operator ImVec4() const {
        return {r, g, b, a};
    }

    Color& Color::operator=(ImVec4 const& col2) {
        r = col2.x;
        g = col2.y;
        b = col2.z;
        a = col2.w;
        return *this;
    }

    Color::operator cocos2d::ccColor4F() const {
        return {r, g, b, a};
    }

    Color::operator cocos2d::_ccColor4B() const {
        return {
            static_cast<uint8_t>(r * 255),
            static_cast<uint8_t>(g * 255),
            static_cast<uint8_t>(b * 255),
            static_cast<uint8_t>(a * 255)
        };
    }

    Color Color::fromHSV(float h, float s, float v, float a) {
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

    Color Color::fromHSV(ImVec4 const& hsv) {
        return fromHSV(hsv.x, hsv.y, hsv.z, hsv.w);
    }

    Color Color::fromInt(int color, Color::IntType type) {
        float v1, v2, v3, v4;
        v1 = (float) ((color >> 24) & 0xFF) / 255.0f;
        v2 = (float) ((color >> 16) & 0xFF) / 255.0f;
        v3 = (float) ((color >> 8) & 0xFF) / 255.0f;
        v4 = (float) (color & 0xFF) / 255.0f;

        switch (type) {
            default: return {v1, v2, v3, v4};
            case IntType::ARGB: return {v2, v3, v4, v1};
            case IntType::ABGR: return {v4, v3, v2, v1};
            case IntType::BGRA: return {v3, v2, v1, v4};
        }
    }

    int Color::toInt(Color::IntType type) const {
        int rv, gv, bv, av;
        rv = static_cast<int>(this->r * 255);
        gv = static_cast<int>(this->g * 255);
        bv = static_cast<int>(this->b * 255);
        av = static_cast<int>(this->a * 255);
        switch (type) {
            default: return (rv << 24) | (gv << 16) | (bv << 8) | av;
            case IntType::ARGB: return (av << 24) | (rv << 16) | (gv << 8) | bv;
            case IntType::ABGR: return (av << 24) | (bv << 16) | (gv << 8) | rv;
            case IntType::BGRA: return (bv << 24) | (gv << 16) | (rv << 8) | av;
        }
    }

    Color Color::fromString(std::string_view color, Color::IntType type) {
        return fromInt(geode::utils::numFromString<uint32_t>(color, 16).unwrapOr(0), type);
    }

    std::string Color::toString(Color::IntType type) const {
        uint32_t c = toInt(type);
        return fmt::format("{:08X}", c);
    }

    Color Color::fromCCColor3B(cocos2d::ccColor3B const& color) {
        return Color(color.r / 255.F, color.g / 255.F, color.b / 255.F);
    }

    cocos2d::ccColor3B Color::toCCColor3B() const {
        return {
            static_cast<uint8_t>(r * 255),
            static_cast<uint8_t>(g * 255),
            static_cast<uint8_t>(b * 255)
        };
    }

    constexpr float hue2rgb(float p, float q, float t) {
        if (t < 0) t += 1;
        if (t > 1) t -= 1;
        if (t < 1.0f / 6) return p + (q - p) * 6 * t;
        if (t < 1.0f / 2) return q;
        if (t < 2.0f / 3) return p + (q - p) * (2.0f / 3 - t) * 6;
        return p;
    }

    Color::HSL Color::HSL::fromColor(Color const& color) {
        auto max = std::max({color.r, color.g, color.b});
        auto min = std::min({color.r, color.g, color.b});
        float h, s, l = (max + min) / 2;

        if (max == min) {
            h = s = 0;
        } else {
            float d = max - min;
            s = l > 0.5 ? d / (2 - max - min) : d / (max + min);

            if (max == color.r) {
                h = (color.g - color.b) / d + (color.g < color.b ? 6 : 0);
            } else if (max == color.g) {
                h = (color.b - color.r) / d + 2;
            } else {
                h = (color.r - color.g) / d + 4;
            }

            h /= 6;
        }

        return {h, s, l};
    }

    Color Color::HSL::toColor(HSL const& hsl) {
        float r, g, b;

        if (hsl.s == 0.f) {
            r = g = b = hsl.l;
        } else {
            auto q = hsl.l < 0.5 ? hsl.l * (1 + hsl.s) : hsl.l + hsl.s - hsl.l * hsl.s;
            auto p = 2 * hsl.l - q;
            r = hue2rgb(p, q, hsl.h + 1.0f / 3);
            g = hue2rgb(p, q, hsl.h);
            b = hue2rgb(p, q, hsl.h - 1.0f / 3);
        }

        return {r, g, b};
    }

    Color::HSL Color::toHSL() const {
        return HSL::fromColor(*this);
    }

    Color Color::fromHSL(HSL const& hsl) const {
        return HSL::toColor(hsl);
    }

    float Color::luminance() const {
        return 0.2126f * r + 0.7152f * g + 0.0722f * b;
    }

    Color Color::darken(float factor) const {
        auto hsl = this->toHSL();
        hsl.l = std::max(0.f, hsl.l - factor);
        Color rgb = hsl;
        rgb.a = a;
        return rgb;
    }

    Color Color::lighten(float factor) const {
        auto hsl = this->toHSL();
        hsl.l = std::min(1.f, hsl.l + factor);
        Color rgb = hsl;
        rgb.a = a;
        return rgb;
    }

    void to_json(nlohmann::json& j, Color const& e) {
        auto str = e.toString();
        j = str;
    }

    void from_json(nlohmann::json const& j, Color& e) {
        e = Color::fromString(j.get<std::string>());
    }
}
