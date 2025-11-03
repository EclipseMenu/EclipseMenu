#pragma once

#include <string>
#include <functional>
#include <imgui.h>
#include <nlohmann/json.hpp>
#include "../animation/easing.hpp"
#include "../animation/move-action.hpp"

namespace eclipse::gui::imgui {

    /// @brief A window that can contain components, has its own title and position.
    /// Supports drag and drop, can be minimized, supports animations.
    class Window {
    public:
        static constexpr ImVec2 MIN_SIZE{220, 1}; // Minimum window size
        static constexpr ImVec2 MAX_SIZE{220, 1000}; // Maximum window size

    public:
        /// @brief Create new instance of `Window` with default title and draw callback
        Window() : m_title("Window"), m_isOpen(true), m_position(0, 0), m_drawPosition(0, 0), m_size(MIN_SIZE) {}

        /// @brief Create new instance of `Window` with set title and draw callback
        /// @param title Title of the window
        /// @param onDraw Callback which will be called when the window is drawn
        Window(std::string title, std::function<void()> onDraw);

        /// @brief Draw the window
        void draw();

        /// @brief Check whether the window is opened
        /// @return True if the window is opened
        [[nodiscard]] bool isOpen() const;

        /// @brief Set whether the window is opened
        /// @param open Whether the window should be opened
        void setOpen(bool open);

        /// @brief Check whether the window is located in the visible area
        /// @return True if the window is located in the visible area
        [[nodiscard]] bool isOnScreen() const;

        [[nodiscard]] std::string const& getTitle() const;
        void setTitle(std::string title);

        [[nodiscard]] ImVec2 const& getPosition() const;
        void setPosition(ImVec2 const& position);

        [[nodiscard]] ImVec2 const& getDrawPosition() const;
        void setDrawPosition(ImVec2 const& position);

        [[nodiscard]] ImVec2 const& getSize() const;
        void setSize(ImVec2 const& size);

        /// @brief Create new `MoveAction` instance for the window
        /// @param target Target position
        /// @param duration How long the animation should last in seconds
        /// @param easing Easing mode (see "animation/easing.hpp")
        /// @param useRealPosition Whether to change the actual position of the window
        std::unique_ptr<animation::MoveAction> animateTo(
            ImVec2 const& target,
                double duration,
                animation::EasingFunction easing,
                bool useRealPosition = false);

    private:
        std::string m_title; // Window title
        bool m_isOpen;       // Whether the window is collapsed or not

        ImVec2 m_position;     // Window position in opened state
        ImVec2 m_drawPosition; // Window position used for drawing (used for animations)
        ImVec2 m_size;         // Window size

        std::function<void()> m_drawCallback; // Callback which will be called when the window is drawn
    };

    void to_json(nlohmann::json& j, Window const& e);
    void from_json(nlohmann::json const& j, Window& e);

}