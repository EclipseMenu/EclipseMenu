#pragma once
#include "layout.hpp"
#include "../core/window.hpp"
#include "../animation/move-action.hpp"

namespace eclipse::gui::imgui {
    class TabbedLayout : public Layout {
    public:
        TabbedLayout() { m_mode = LayoutMode::Tabbed; }
        ~TabbedLayout() override = default;

        void init() override;
        void draw() override;
        void toggle(bool state) override;
        bool wantStayVisible() const override;

    public:
        bool shouldRender() const;
        static ImVec2 randomWindowPosition(Window const& window);
        std::map<Window*, ImVec2> getStackedPositions();
        void stackWindows();

    private:
        std::vector<Window> m_windows;
        std::vector<std::unique_ptr<animation::MoveAction>> m_actions;
        uint8_t m_preloadStep = 0;
    };
}
