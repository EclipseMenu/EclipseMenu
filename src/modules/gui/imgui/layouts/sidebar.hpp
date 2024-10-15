#pragma once
#include "layout.hpp"
#include "../core/tab.hpp"
#include "../animation/move-action.hpp"

namespace eclipse::gui::imgui {

    class SidebarLayout : public Layout {
    public:
        SidebarLayout() { m_mode = LayoutMode::Sidebar; }
        ~SidebarLayout() override = default;

        void init() override;
        void draw() override;
        void toggle(bool state) override;

    public:
        static void recalculateSize(bool second);

        std::vector<Tab> m_tabs;
        unsigned int m_selectedTab = 0;
    };

}