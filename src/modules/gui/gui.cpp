#include "gui.hpp"
#include "imgui/imgui.hpp"

namespace eclipse::gui {

    MenuTab* MenuTab::find(const std::string& name) {
        return Engine::get()->findTab(name);
    }

    Engine* Engine::get() {
        // TODO: Make this return the correct engine based on platform,
        // or even switch between engines at runtime.
        static imgui::ImGuiEngine instance;
        return &instance;
    }

}