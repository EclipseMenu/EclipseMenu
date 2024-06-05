#pragma once
#include <modules/gui/gui.hpp>

namespace eclipse::gui::imgui {

    class ImGuiEngine : public Engine {
    public:
        void init() override;
        MenuTab* findTab(const std::string& name) override;

    private:
        /// @brief ImGui draw function.
        void draw();

        /// @brief Component visitor function.
        void visit(Component* component);

    private:
        bool m_initialized = false;
        std::vector<MenuTab> m_tabs;
    };

}