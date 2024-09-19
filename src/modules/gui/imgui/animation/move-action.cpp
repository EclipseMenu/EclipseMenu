#include "move-action.hpp"

namespace eclipse::gui::animation {
    std::shared_ptr<MoveAction> MoveAction::create(double duration, ImVec2* vector, ImVec2 target, EasingFunction easing) {
        auto instance = std::make_shared<MoveAction>();
        instance->m_duration = duration;
        instance->m_target = vector;
        instance->m_start = {vector->x, vector->y};
        instance->m_end = target;
        instance->m_delta = {target.x - vector->x, target.y - vector->y};
        instance->m_easing = easing;
        instance->m_totalTime = 0;
        return instance;
    }

    void MoveAction::update(double deltaTime) {
        // Make sure to move even when duration is set to 0
        if (m_duration == 0) {
            m_target->x = m_end.x;
            m_target->y = m_end.y;
            return;
        }

        if (isFinished())
            return;

        m_totalTime += deltaTime;

        if (isFinished()) {
            m_target->x = m_end.x;
            m_target->y = m_end.y;
            return;
        }

        double time = m_totalTime / m_duration;
        auto eased = static_cast<float>(m_easing(time));

        ImVec2 current = {m_delta.x * eased, m_delta.y * eased};
        m_target->x = m_start.x + current.x;
        m_target->y = m_start.y + current.y;
    }

    double MoveAction::getProgress() {
        return m_easing(m_totalTime / m_duration);
    }

    bool MoveAction::isFinished() const {
        return m_totalTime >= m_duration;
    }
}