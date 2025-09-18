#include "setting.hpp"

#include <modules/config/config.hpp>
#include <modules/utils/SingletonCache.hpp>
#include <utils.hpp>

#include "variables.hpp"

namespace eclipse::labels {
    size_t LabelSettings::instanceCount = 0;

    // Handle the event based on the type and condition
    Event* handleEvent(const LabelEvent& event, const LabelSettings* label) {
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

        for (const auto& event : events) {
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
        using FileEvent = geode::Task<geode::Result<std::filesystem::path>>;
        static geode::EventListener<FileEvent> s_listener;
        geode::utils::file::FilePickOptions::Filter filter;
        filter.description = "Eclipse Label (*.ecl)";
        filter.files.insert("*.ecl");
        s_listener.bind([this](FileEvent::Event* event) {
            if (auto value = event->getValue()) {
                auto path = value->unwrapOr("");
                if (path.empty()) return;

                // ensure the file has the correct extension
                if (path.extension() != ".ecl") path.replace_extension(".ecl");

                auto data = nlohmann::json(*this).dump(4, ' ', false, nlohmann::detail::error_handler_t::ignore);
                auto res = geode::utils::file::writeString(path, data);
                if (res.isErr()) {
                    geode::log::error("Failed to save label file: {}", res.unwrapErr());
                }
            }
        });

        s_listener.setFilter(geode::utils::file::pick(
            geode::utils::file::PickMode::SaveFile,
            { geode::Mod::get()->getSaveDir(), { filter }}
        ));
    }

    void from_json(const nlohmann::json& json, LabelSettings& settings) {
        settings.name = json.value("name", fmt::format("New label {}", settings.id));
        settings.text = json.value("text", "");
        settings.font = json.value("font", "bigFont.fnt");
        settings.scale = json.value("scale", 0.6f);
        settings.color = json.value("color", gui::Color::WHITE);
        settings.visible = json.value("visible", true);
        settings.absolutePosition = json.value("absolutePosition", false);
        settings.offset.x = json.value("offset-x", 0.f);
        settings.offset.y = json.value("offset-y", 0.f);
        settings.alignment = static_cast<LabelsContainer::Alignment>(json.value("alignment", 0));
        settings.fontAlignment = static_cast<BMFontAlignment>(json.value("fontAlignment", 0));
        settings.events = json.value("events", std::vector<LabelEvent>());
    }

    void to_json(nlohmann::json& json, const LabelSettings& settings) {
        json = nlohmann::json{
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
        };
    }

    #define READ_OPTIONAL(key) if (json.contains(#key)) event.key = json.at(#key).get<decltype(event.key)::value_type>()
    #define STORE_OPTIONAL(key) if (event.key.has_value()) json[#key] = event.key.value()

    void from_json(const nlohmann::json& json, LabelEvent& event) {
        event.enabled = json.value("enabled", true);
        event.type = static_cast<LabelEvent::Type>(json.value("type", 1));
        event.condition = json.value("condition", "");
        event.delay = json.value("delay", 0.f);
        event.duration = json.value("duration", 0.f);
        event.easing = json.value("easing", 0.f);

        READ_OPTIONAL(visible);
        READ_OPTIONAL(scale);
        READ_OPTIONAL(color);
        READ_OPTIONAL(opacity);
        READ_OPTIONAL(font);
    }

    void to_json(nlohmann::json& json, const LabelEvent& event) {
        json = nlohmann::json{
            {"enabled", event.enabled},
            {"type", static_cast<int>(event.type)},
            {"condition", event.condition},
            {"delay", event.delay},
            {"duration", event.duration},
            {"easing", event.easing}
        };

        STORE_OPTIONAL(visible);
        STORE_OPTIONAL(scale);
        STORE_OPTIONAL(color);
        STORE_OPTIONAL(opacity);
        STORE_OPTIONAL(font);
    }
}
