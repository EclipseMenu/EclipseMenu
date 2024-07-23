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

    void to_json(nlohmann::json& j, const Color& e) {
        auto str = e.toString();
        j = str;
    }

    void from_json(const nlohmann::json& j, Color& e) {
        e = Color::fromString(j.get<std::string>());
    }
}