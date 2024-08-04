#include "setting.hpp"

namespace eclipse::labels {
    size_t LabelSettings::instanceCount = 0;

    void from_json(const nlohmann::json& json, LabelSettings& settings) {
        settings.name = json.value("name", fmt::format("New label {}", settings.id));
        settings.text = json.value("text", "");
        settings.font = json.value("font", "bigFont.fnt");
        settings.scale = json.value("scale", 0.6f);
        settings.color = json.value("color", gui::Color::WHITE);
        settings.visible = json.value("visible", true);
        settings.alignment = static_cast<LabelsContainer::Alignment>(json.value("alignment", 0));
    }

    void to_json(nlohmann::json& json, const LabelSettings& settings) {
        json = nlohmann::json{
            {"name", settings.name},
            {"text", settings.text},
            {"font", settings.font},
            {"scale", settings.scale},
            {"color", settings.color},
            {"visible", settings.visible},
            {"alignment", static_cast<int>(settings.alignment)}
        };
    }
}