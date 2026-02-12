#pragma once
#include <array>
#include <chrono>
#include <optional>
#include <vector>
#include <modules/gui/color.hpp>

namespace eclipse::labels {
    inline static std::array eventNames = {
        "Always", "Custom", "On Death", "On Button Hold", "On Noclip Death"
    };

    inline static std::array visibleNames = {
        "Don't Modify", "Show", "Hide"
    };

    /// @brief If a specified requirement is met, modify the label properties.
    struct LabelEvent {
        enum class Type {
            Always        = 0, // Unconditional (useful for testing, etc.)
            Custom        = 1, // Use RIFT to check for a condition
            OnDeath       = 2, // When the player is dead
            OnButtonHold  = 3, // When the player is holding a button
            OnNoclipDeath = 4, // When the player is dying in noclip mode
        };

        ~LabelEvent();

        inline static size_t instanceCount = 0;

        bool enabled = true;      // whether the event is enabled
        Type type = Type::Custom; // type of the event

        std::string condition; // RIFT condition (e.g. "progress >= bestPercent")

        // Properties to modify (optional means don't modify)
        std::optional<bool> visible;
        std::optional<float> scale;
        std::optional<gui::Color> color;
        std::optional<float> opacity;
        std::optional<std::string> font;

        // Animation properties
        float delay = 0.f;    // delay before the animation starts
        float duration = 0.f; // how long the new state should last after condition is no longer met
        float easing = 0.f;   // time to ease in/out the animation (only for scale and color)

        size_t id = instanceCount++;
    };

    /// @brief Holds information about event that happened.
    struct Event {
        size_t labelId;
        size_t eventId;
        LabelEvent* event;
        struct LabelSettings* settings;

        Event(size_t labelId, size_t eventId, LabelEvent* event, LabelSettings* settings)
            : labelId(labelId), eventId(eventId), event(event), settings(settings) {}

        std::chrono::time_point<std::chrono::steady_clock> beginTime;
        std::chrono::time_point<std::chrono::steady_clock> endTime;
        bool started = false;
        bool ended = false;

        float getProgress() const;
        bool hasEnded() const;

        struct EventState {
            bool visible;
            float scale;
            gui::Color color;
            std::string font;
        };

        void processState(EventState& state) const;

        void start() {
            if (started) return;
            beginTime = std::chrono::steady_clock::now();
            started = true;
        }

        void end() {
            if (ended) return;
            endTime = std::chrono::steady_clock::now();
            ended = true;
        }
    };

    /// @brief Manager for label events, used to queue and process events.
    class EventManager {
    public:
        static EventManager& get();

        void queueEvent(LabelEvent const& event, LabelSettings* label);
        void dequeueEvent(LabelEvent const& event, LabelSettings const* label);

        /// @brief Remove all events for a label. Call this when the label is deleted.
        void removeEvents(LabelSettings const* label);
        /// @brief Remove a specific event.
        void removeEvent(LabelEvent const* event);

        Event* getEvent(size_t labelId, size_t eventId);
        void processEvents();

    private:
        std::vector<Event> m_events;
    };
}

template <>
struct matjson::Serialize<eclipse::labels::LabelEvent::Type> {
    static Value toJson(eclipse::labels::LabelEvent::Type const& type) {
        return static_cast<int>(type);
    }
    static geode::Result<eclipse::labels::LabelEvent::Type> fromJson(Value const& value) {
        GEODE_UNWRAP_INTO(auto intValue, value.as<int>());
        return geode::Ok(static_cast<eclipse::labels::LabelEvent::Type>(intValue));
    }
};
