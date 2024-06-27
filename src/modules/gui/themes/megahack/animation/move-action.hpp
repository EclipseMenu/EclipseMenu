#pragma once

#include "easing.hpp"
#include <imgui.h>
#include <functional>

namespace eclipse::gui::animation {
    /// @brief Class which controls the animation process
    class MoveAction {
    public:
        /// @brief Create new instance of `MoveAction` with set
        /// @param duration How long the animation should last in seconds
        /// @param vector Pointer to the vector which will get modified
        /// @param target Target position
        /// @param easing Easing mode (see "easing.hpp")
        /// @return Created `MoveAction` instance
        static MoveAction* create(double duration, ImVec2* vector, ImVec2 target, EasingFunction easing);

        /// @brief Need to be called every frame to update the value
        /// @param deltaTime How much time passed since last update in seconds
        void update(double deltaTime);

        /// @brief Get current progress of the animation
        /// @return Progress in range [0, 1] (note that it can be out of this range if the animation is overshooting)
        [[nodiscard]] double getProgress();

        /// @brief Check whether animation has finished
        /// @return True if it has finished
        [[nodiscard]] bool isFinished() const;

    private:
        ImVec2* m_target{};
        ImVec2 m_start;
        ImVec2 m_end;
        ImVec2 m_delta;

        double m_duration{}, m_totalTime{};
        EasingFunction m_easing{};
    };
}