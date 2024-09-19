#include "color.hpp"

namespace eclipse::gui {

    const Color Color::WHITE = {1, 1, 1};
    const Color Color::BLACK = {0, 0, 0};
    const Color Color::RED = {1, 0, 0};
    const Color Color::GREEN = {0, 1, 0};
    const Color Color::BLUE = {0, 0, 1};
    const Color Color::YELLOW = {1, 1, 0};
    const Color Color::CYAN = {0, 1, 1};
    const Color Color::MAGENTA = {1, 0, 1};

    Color::operator ImU32() const {
        return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
    }

    Color &Color::operator=(const Color &other) {
        if (this == &other) return *this;
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        return *this;
    }

    Color &Color::operator=(Color &&other) noexcept {
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

    Color &Color::operator=(const ImVec4 &col2) {
        r = col2.x;
        g = col2.y;
        b = col2.z;
        a = col2.w;
        return *this;
    }

    Color::operator cocos2d::ccColor4F() const {
        return {r, g, b, a};
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

    Color Color::fromInt(int color, Color::IntType type) {
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

    int Color::toInt(Color::IntType type) const {
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

    Color Color::fromString(const std::string &color, Color::IntType type) {
        uint32_t c = std::strtoul(color.c_str(), nullptr, 16);
        return fromInt(c, type);
    }

    std::string Color::toString(Color::IntType type) const {
        uint32_t c = toInt(type);
        return fmt::format("{:08X}", c);
    }

    cocos2d::ccColor3B Color::toCCColor3B() const {
        return {
            static_cast<uint8_t>(r * 255),
            static_cast<uint8_t>(g * 255),
            static_cast<uint8_t>(b * 255)
        };
    }

    void to_json(nlohmann::json& j, const Color& e) {
        auto str = e.toString();
        j = str;
    }

    void from_json(const nlohmann::json& j, Color& e) {
        e = Color::fromString(j.get<std::string>());
    }
}