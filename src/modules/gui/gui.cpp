#include "gui.hpp"

#include <modules/config/config.hpp>
#include <algorithm>

#include "imgui.hpp"

namespace eclipse::gui {

    bool ToggleComponent::getValue() const {
        return config::get<bool>(m_id, false);
    }

    void ToggleComponent::setValue(bool value) {
        config::set(m_id, value);
    }

    void ToggleComponent::addOptions(const std::function<void(std::shared_ptr<MenuTab>)>& options) {
        if (!m_options)
            m_options = std::make_shared<MenuTab>("Options");

        options(m_options);
    }

    ToggleComponent* ToggleComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(m_id, m_title, [this](){
            bool value = !getValue();
            setValue(value);
            this->triggerCallback(value);
        });
        m_hasKeybind = true;
        return this;
    }

    RadioButtonComponent* RadioButtonComponent::handleKeybinds() {
        auto specialId = fmt::format("{}-{}", m_id, m_value);
        keybinds::Manager::get()->registerKeybind(specialId, m_title, [this](){
            auto value = getValue();
            config::set(this->getId(), value);
            this->triggerCallback(value);
        });
        m_hasKeybind = true;
        return this;
    }

    FloatToggleComponent* FloatToggleComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(m_id, m_title, [this](){
            bool value = !config::get<bool>(this->getId() + ".toggle", false);
            config::set(this->getId() + ".toggle", value);
            this->triggerCallback();
        });
        m_hasKeybind = true;
        return this;
    }

    void MenuTab::addComponent(std::shared_ptr<Component> component) {
        m_components.push_back(component);
    }

    void MenuTab::removeComponent(std::weak_ptr<Component> component) {
        auto it = std::find_if(m_components.begin(), m_components.end(), [&component](const std::shared_ptr<Component>& c) {
            return component.lock() == c;
        });

        m_components[it - m_components.begin()].reset();
        m_components.erase(it);
    }

    std::shared_ptr<MenuTab> MenuTab::find(const std::string& name) {
        return Engine::get()->findTab(name);
    }

    std::shared_ptr<Engine> Engine::get() {
        // TODO: Make this return the correct engine based on platform,
        // or even switch between engines at runtime.
        static auto instance = std::make_shared<imgui::ImGuiEngine>();
        return std::static_pointer_cast<Engine>(instance);
    }

#define SUPPORT_COMPONENT(type) (auto* component##__LINE__ = dynamic_cast<type*>(component)) this->visit(component##__LINE__)
    
    void Style::visit(Component* component) {
        if SUPPORT_COMPONENT(ToggleComponent);
        else if SUPPORT_COMPONENT(SliderComponent);
        else if SUPPORT_COMPONENT(LabelComponent);
        else if SUPPORT_COMPONENT(InputFloatComponent);
        else if SUPPORT_COMPONENT(InputIntComponent);
        else if SUPPORT_COMPONENT(InputTextComponent);
        else if SUPPORT_COMPONENT(FloatToggleComponent);
        else if SUPPORT_COMPONENT(RadioButtonComponent);
        else if SUPPORT_COMPONENT(ComboComponent);
        else if SUPPORT_COMPONENT(ButtonComponent);
        else if SUPPORT_COMPONENT(ColorComponent);
        else if SUPPORT_COMPONENT(KeybindComponent);
        else if SUPPORT_COMPONENT(LabelSettingsComponent);
    }

#undef SUPPORT_COMPONENT

}
