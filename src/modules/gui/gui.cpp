#include "gui.hpp"

#include <modules/config/config.hpp>
#include <algorithm>

#include "themes/megahack/megahack.hpp"
#include "imgui.hpp"

namespace eclipse::gui {

    bool ToggleComponent::getValue() const {
        return config::get<bool>(m_id, false);
    }

    void ToggleComponent::setValue(bool value) {
        config::set(m_id, value);
    }

    void ToggleComponent::addOptions(std::function<void(MenuTab*)> options) {
        if(!m_options)
            m_options = new MenuTab("Options");
        options(m_options);
    }
        

    void MenuTab::addComponent(Component* component) {
        m_components.push_back(component);
    }

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