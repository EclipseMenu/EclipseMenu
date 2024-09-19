#pragma once
#include <modules/gui/gui.hpp>

#include "tab/tab.hpp"

namespace eclipse::gui::imgui {
    class PanelLayout : public Layout {
    public:
        void toggle() override;
        void draw() override;

        bool shouldRender();

        std::shared_ptr<MenuTab> findTab(const std::string& name);

    private:
        std::vector<std::shared_ptr<MenuTab>> m_tabs;
        std::vector<Tab> m_realTabs;

        unsigned int m_selectedTab = 0;
    };
}