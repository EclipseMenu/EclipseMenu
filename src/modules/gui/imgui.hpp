#pragma once
#include <modules/gui/gui.hpp>

namespace eclipse::gui::imgui {

    class ImGuiEngine : public Engine {
    public:
        void init() override;
        void toggle() override;
        bool isToggled() override;

        MenuTab* findTab(const std::string& name) override;

        Theme* getTheme();

    private:
        bool m_initialized = false;
        bool m_isOpened = false;

        Theme* m_theme;
    };

}