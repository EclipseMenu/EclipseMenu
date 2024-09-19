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

    public:
        bool shouldRender() const;
        static ImVec2 randomWindowPosition(const Window& window);
        std::map<Window*, ImVec2> getStackedPositions();
        void stackWindows();

    private:
        std::vector<Window> m_windows;
        std::vector<std::shared_ptr<animation::MoveAction>> m_actions;
        int m_preloadStep = 0;
    };

}