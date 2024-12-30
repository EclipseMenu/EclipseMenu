#include "events.hpp"
#include <modules/gui/imgui/animation/easing.hpp>

#include "setting.hpp"

namespace eclipse::labels {
    LabelEvent::~LabelEvent() {
        EventManager::get().removeEvent(this);
    }

    float Event::getProgress() const {
        auto now = std::chrono::steady_clock::now();
        auto delay = event->delay * 1000;       // how long to wait after the event starts
        auto duration = event->duration * 1000; // how long the event should last after the condition is no longer met
        auto easing = event->easing * 1000;     // smoothing for the animation

        if (event->type == LabelEvent::Type::Always) {
            return 1.f;
        }

        auto timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(now - beginTime).count();

        // if we're still in the delay, return 0%
        if (timePassed <= delay) return 0.f;

        if (ended) {
            auto timePassedExit = std::chrono::duration_cast<std::chrono::milliseconds>(now - endTime).count();

            // calculate progress for ending the event
            //       |          |             |
            //    endTime    duration       easing
            //     (100%)     (100%)  (50%)  (0%)

            // if we're still in the exit duration, return 100%
            if (timePassedExit <= duration) return 1.f;

            // otherwise, calculate the easing
            auto progress = 1 - std::clamp((timePassedExit - duration) / easing, 0.f, 1.f);
            return std::clamp(gui::animation::easing::easeOutQuad(progress), 0.0, 1.0);
        }

        // calculate progress for starting the event
        //        |          |              |
        //    beginTime    delay          easing
        //       (0%)      (0%)   (50%)   (100%)

        auto progress = std::clamp((timePassed - delay) / easing, 0.f, 1.f);
        return std::clamp(gui::animation::easing::easeInQuad(progress), 0.0, 1.0);
    }

    bool Event::hasEnded() const {
        return ended && getProgress() == 0.f;
    }

    static float ease(float start, float end, float progress) {
        return start + (end - start) * progress;
    }

    void Event::processState(EventState& state) const {
        auto progress = getProgress();

        if (event->visible.has_value() && progress == 1.f) {
            state.visible = event->visible.value();
        }

        if (event->scale.has_value()) {
            state.scale = ease(state.scale, event->scale.value(), progress);
        }

        auto color = state.color;
        if (event->opacity.has_value()) {
            color.a = ease(state.color.a, event->opacity.value(), progress);
        }

        if (event->color.has_value()) {
            color = gui::Color{
                ease(state.color.r, event->color->r, progress),
                ease(state.color.g, event->color->g, progress),
                ease(state.color.b, event->color->b, progress),
                color.a
            };
        }

        state.color = color;

        if (event->font.has_value() && progress == 1.f) {
            state.font = event->font.value();
        }
    }

    EventManager& EventManager::get() {
        static EventManager instance;
        return instance;
    }

    void EventManager::queueEvent(const LabelEvent& event, LabelSettings* label) {
        // check if the event is already queued
        if (auto* existing = getEvent(label->id, event.id)) {
            // if it has ended, recreate it
            if (existing->hasEnded()) {
                removeEvent(existing->event);
                m_events.emplace_back(Event{label->id, event.id, const_cast<LabelEvent*>(&event), label});
            }

            existing->start();
            return;
        }

        auto& e = m_events.emplace_back(Event{label->id, event.id, const_cast<LabelEvent*>(&event), label});
        e.start();
    }

    void EventManager::dequeueEvent(const LabelEvent& event, const LabelSettings* label) {
        if (auto* e = getEvent(label->id, event.id)) {
            e->end();
            if (e->hasEnded()) {
                removeEvent(e->event);
            }
        }
    }

    void EventManager::removeEvents(const LabelSettings* label) {
        std::erase_if(m_events, [label](const Event& event) {
            return event.settings == label;
        });
    }

    void EventManager::removeEvent(const LabelEvent* event) {
        std::erase_if(m_events, [event](const Event& e) {
            return e.event == event;
        });
    }

    Event* EventManager::getEvent(size_t labelId, size_t eventId) {
        for (auto& event : m_events) {
            if (event.labelId == labelId && event.eventId == eventId) {
                return &event;
            }
        }
        return nullptr;
    }
}
