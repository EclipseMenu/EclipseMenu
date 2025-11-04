#pragma once
#include "layout.hpp"
#include "../core/tab.hpp"

namespace eclipse::gui::imgui {

    class PanelLayout : public Layout {
    public:
        PanelLayout() { m_mode = LayoutMode::Panel; }
        ~PanelLayout() override = default;

        void init() override;
        void draw() override;
        void toggle(bool state) override;

    public:
        static void recalculateSize();

        std::vector<Tab> m_tabs;
        unsigned int m_selectedTab = 0;
    };

}