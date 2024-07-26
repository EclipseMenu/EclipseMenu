#pragma once
#include <modules/gui/gui.hpp>
#include "theme.hpp"

namespace eclipse::gui::imgui {

    class ImGuiEngine : public Engine {
    public:
        void init() override;
        void toggle() override;
        bool isToggled() override;

        std::shared_ptr<MenuTab> findTab(const std::string& name) override;

        std::shared_ptr<Theme> getTheme();

    private:
        bool m_initialized = false;
        bool m_isOpened = false;

        std::shared_ptr<Theme> m_theme;
    };

}