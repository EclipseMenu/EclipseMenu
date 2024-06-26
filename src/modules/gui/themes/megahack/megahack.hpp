#pragma once
#include <modules/gui/gui.hpp>
#include "window/window.hpp"
#include "animation/move-action.hpp"

namespace eclipse::gui::imgui {

    class MegahackEngine : public Engine {
    public:
        void init() override;
        void toggle() override;
        MenuTab* findTab(const std::string& name) override;
        bool isToggled() override;

    private:
        /// @brief ImGui draw function.
        void draw();

        /// @brief ImGui setup function.
        void setup();

        /// @brief Component visitor function.
        static void visit(Component* component);

        /// @brief Whether the GUI should be drawn.
        [[nodiscard]] bool shouldRender();

    private:
        bool m_initialized = false;
        std::vector<MenuTab*> m_tabs;
        std::vector<Window> m_windows;
        std::vector<animation::MoveAction*> m_actions;

        bool m_isOpened = false;
        bool m_isAnimating = false;
    };

}