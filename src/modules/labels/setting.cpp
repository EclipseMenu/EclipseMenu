#include "setting.hpp"

#include <modules/config/config.hpp>
#include <modules/utils/SingletonCache.hpp>
#include <utils.hpp>

#include "variables.hpp"

namespace eclipse::labels {
    size_t LabelSettings::instanceCount = 0;

    // Handle the event based on the type and condition
    Event* handleEvent(LabelEvent const& event, LabelSettings const* label) {
        if (!event.enabled) return nullptr;

        // check if the condition is met
        bool shouldProcess = false;
        switch (event.type) {
            case LabelEvent::Type::Always: {
                shouldProcess = true;
            } break;
            case LabelEvent::Type::Custom: {
                auto res = rift::evaluate(event.condition, VariableManager::get().getVariables());
                if (res.isOk()) {
                    shouldProcess = res.unwrap().toBoolean();
                }
            } break;
            case LabelEvent::Type::OnDeath: {
                if (auto gjbgl = utils::get<GJBaseGameLayer>())
                    shouldProcess = gjbgl->m_player1->m_isDead;
            } break;
            case LabelEvent::Type::OnButtonHold: {
                if (auto gjbgl = utils::get<GJBaseGameLayer>())
                    shouldProcess = gjbgl->m_player1->m_jumpBuffered;
            } break;
            case LabelEvent::Type::OnNoclipDeath: {
                shouldProcess = config::getTemp<bool>("noclipDying", false);
            } break;
        }

        // process the queueing and dequeuing of the event based on the delay/duration
        auto& manager = EventManager::get();
        if (shouldProcess) manager.queueEvent(event, const_cast<LabelSettings*>(label));
        else manager.dequeueEvent(event, label);

        // return whether the event should be processed this frame
        return manager.getEvent(label->id, event.id);
    }

    Event::EventState LabelSettings::processEvents() const {
        Event::EventState state {
            .visible = visible,
            .scale = scale,
            .color = color,
            .font = font,
        };

        for (auto const& event : events) {
            auto eventState = handleEvent(event, this);
            if (!eventState) continue;
            eventState->processState(state);
        }

        return std::move(state);
    }

    bool LabelSettings::hasEvents() const {
        return !events.empty() && std::ranges::any_of(events, [](auto e) { return e.enabled; });
    }

    void LabelSettings::promptSave() const {
        geode::utils::file::FilePickOptions::Filter filter;
        filter.description = "Eclipse Label (*.ecl)";
        filter.files.insert("*.ecl");

        geode::async::spawn(
            geode::utils::file::pick(
                geode::utils::file::PickMode::SaveFile,
                { geode::Mod::get()->getSaveDir(), { std::move(filter) }}
            ),
            [this](geode::utils::file::PickResult res) {
                if (!res) return;
                auto pathOpt = std::move(res).unwrap();
                if (!pathOpt) return;

                auto path = std::move(*pathOpt);
                if (path.extension() != ".ecl") path.replace_extension(".ecl");

                auto res2 = geode::utils::file::writeToJson(path, *this);
                if (res2.isErr()) {
                    geode::log::error("Failed to save label file: {}", res2.unwrapErr());
                }
            }
        );
    }
}

matjson::Value matjson::Serialize<eclipse::labels::LabelSettings>::toJson(
    eclipse::labels::LabelSettings const& settings
) {
    return makeObject({
        {"name", settings.name},
        {"text", settings.text},
        {"font", settings.font},
        {"scale", settings.scale},
        {"color", settings.color},
        {"visible", settings.visible},
        {"absolutePosition", settings.absolutePosition},
        {"offset-x", settings.offset.x},
        {"offset-y", settings.offset.y},
        {"alignment", static_cast<int>(settings.alignment)},
        {"fontAlignment", static_cast<int>(settings.fontAlignment)},
        {"events", settings.events}
    });
}

geode::Result<eclipse::labels::LabelSettings> matjson::Serialize<eclipse::labels::LabelSettings>::fromJson(
    Value const& value
) {
    eclipse::labels::LabelSettings settings;
    GEODE_UNWRAP_INTO(settings.name, value["name"].as<std::string>());
    GEODE_UNWRAP_INTO(settings.text, value["text"].as<std::string>());
    GEODE_UNWRAP_INTO(settings.font, value["font"].as<std::string>());
    GEODE_UNWRAP_INTO(settings.scale, value["scale"].as<float>());
    GEODE_UNWRAP_INTO(settings.color, value["color"].as<eclipse::gui::Color>());
    GEODE_UNWRAP_INTO(settings.visible, value["visible"].as<bool>());
    GEODE_UNWRAP_INTO(settings.absolutePosition, value["absolutePosition"].as<bool>());
    GEODE_UNWRAP_INTO(settings.offset.x, value["offset-x"].as<float>());
    GEODE_UNWRAP_INTO(settings.offset.y, value["offset-y"].as<float>());
    GEODE_UNWRAP_INTO(int alignment, value["alignment"].as<int>());
    GEODE_UNWRAP_INTO(int fontAlignment, value["fontAlignment"].as<int>());
    GEODE_UNWRAP_INTO(settings.events, value["events"].as<std::vector<eclipse::labels::LabelEvent>>());
    settings.alignment = static_cast<eclipse::labels::LabelsContainer::Alignment>(alignment);
    settings.fontAlignment = static_cast<BMFontAlignment>(fontAlignment);
    return geode::Ok(std::move(settings));
}

matjson::Value matjson::Serialize<eclipse::labels::LabelEvent>::toJson(eclipse::labels::LabelEvent const& event) {
    Value json = makeObject({
        {"enabled", event.enabled},
        {"type", static_cast<int>(event.type)},
        {"condition", event.condition},
        {"delay", event.delay},
        {"duration", event.duration},
        {"easing", event.easing}
    });

    if (event.visible.has_value()) json["visible"] = event.visible.value();
    if (event.scale.has_value()) json["scale"] = event.scale.value();
    if (event.color.has_value()) json["color"] = event.color.value();
    if (event.opacity.has_value()) json["opacity"] = event.opacity.value();
    if (event.font.has_value()) json["font"] = event.font.value();

    return json;
}

geode::Result<eclipse::labels::LabelEvent> matjson::Serialize<eclipse::labels::LabelEvent>::fromJson(Value const& value) {
    eclipse::labels::LabelEvent event;
    GEODE_UNWRAP_INTO(event.enabled, value["enabled"].as<bool>());
    GEODE_UNWRAP_INTO(event.type, value["type"].as<eclipse::labels::LabelEvent::Type>());
    GEODE_UNWRAP_INTO(event.condition, value["condition"].as<std::string>());
    GEODE_UNWRAP_INTO(event.delay, value["delay"].as<float>());
    GEODE_UNWRAP_INTO(event.duration, value["duration"].as<float>());
    GEODE_UNWRAP_INTO(event.easing, value["easing"].as<float>());
    GEODE_UNWRAP_INTO(event.visible, value["visible"].as<std::optional<bool>>());
    GEODE_UNWRAP_INTO(event.scale, value["scale"].as<std::optional<float>>());
    GEODE_UNWRAP_INTO(event.color, value["color"].as<std::optional<eclipse::gui::Color>>());
    GEODE_UNWRAP_INTO(event.opacity, value["opacity"].as<std::optional<float>>());
    GEODE_UNWRAP_INTO(event.font, value["font"].as<std::optional<std::string>>());
    return geode::Ok(std::move(event));
}
