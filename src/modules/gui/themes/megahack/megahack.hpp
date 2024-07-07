#pragma once
#include <modules/gui/gui.hpp>

#include "window/window.hpp"
#include "animation/move-action.hpp"

namespace eclipse::gui::imgui {
    class WindowLayout : public Layout {
    public:
        void toggle() override;
        void visit(Component* component) override;
        void draw() override;

        bool shouldRender();

        MenuTab* findTab(const std::string& name);

    private:
        ImVec2 randomWindowPosition(Window &window);
        std::map<Window*, ImVec2> getStackedPositions();
        void stackWindows();

        std::vector<MenuTab*> m_tabs;
        std::vector<Window> m_windows;
        std::vector<animation::MoveAction*> m_actions;

        bool m_isAnimating = false;
    };
}