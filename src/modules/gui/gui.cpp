#include "gui.hpp"

#include <modules/config/config.hpp>
#include <algorithm>

#include "imgui/imgui.hpp"
#include "cocos/cocos.hpp"

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

    void MenuTab::addComponent(const std::shared_ptr<Component>& component) {
        m_components.push_back(component);
    }

    void MenuTab::removeComponent(std::weak_ptr<Component> component) {
        auto it = std::find_if(m_components.begin(), m_components.end(), [&component](const std::shared_ptr<Component>& c) {
            return component.lock() == c;
        });

        m_components[it - m_components.begin()].reset();
        m_components.erase(it);
    }

    std::shared_ptr<MenuTab> MenuTab::find(std::string_view name) {
        auto engine = Engine::get();
        return engine->findTab(name);
    }

    void Engine::setRenderer(RendererType type) {
        if (type == m_rendererType && m_renderer) return;
        if (m_renderer) m_renderer->shutdown();

        switch (type) {
            case RendererType::ImGui:
            default:
                m_renderer = std::make_shared<imgui::ImGuiRenderer>();
                break;
            case RendererType::Cocos2d:
                m_renderer = std::make_shared<cocos::CocosRenderer>();
                break;
        }

        m_rendererType = type;
        m_renderer->init();
        config::set("menu.renderer", static_cast<int>(type));
    }

    std::shared_ptr<Engine> Engine::get() {
        static std::shared_ptr<Engine> s_engine = std::make_shared<Engine>();
        return s_engine;
    }

    void Engine::init() {
        setRenderer(config::get<RendererType>("menu.renderer", RendererType::ImGui));
    }

    void Engine::toggle() {
        if (!m_renderer) return;
        m_renderer->toggle();
    }

    std::shared_ptr<MenuTab> Engine::findTab(std::string_view name) {
        for (auto tab : m_tabs) {
            if (tab->getTitle() == name) {
                return tab;
            }
        }

        // If the tab does not exist, create a new one.
        auto tab = std::make_shared<MenuTab>(std::string(name));
        m_tabs.push_back(tab);

        return tab;
    }

}
