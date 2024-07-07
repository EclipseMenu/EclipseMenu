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

    ToggleComponent* ToggleComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(m_id, m_title, [this](){
            setValue(!getValue());
        });
        return this;
    }

    void MenuTab::addComponent(Component* component) {
        m_components.push_back(component);
    }

    void MenuTab::removeComponent(Component* component) {
        m_components.erase(std::remove(m_components.begin(), m_components.end(), component), m_components.end());
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