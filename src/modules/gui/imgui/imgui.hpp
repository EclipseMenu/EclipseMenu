#pragma once
#include <modules/gui/gui.hpp>
#include "window.hpp"

namespace eclipse::gui::imgui {

    class ImGuiEngine : public Engine {
    public:
        void init() override;
        MenuTab* findTab(const std::string& name) override;

    private:
        /// @brief ImGui draw function.
        void draw();

        /// @brief Component visitor function.
        static void visit(Component* component);

    private:
        bool m_initialized = false;
        std::vector<MenuTab*> m_tabs;
        std::vector<Window> m_windows;
    };

}