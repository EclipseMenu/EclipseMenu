#include "gui.hpp"

#include <algorithm>
#include <modules/config/config.hpp>

#include "blur/blur.hpp"
#include "cocos/cocos.hpp"
#include "imgui/imgui.hpp"
#include "theming/manager.hpp"

#include "components/base-component.hpp"
#include "components/button.hpp"
#include "components/color.hpp"
#include "components/combo.hpp"
#include "components/filesystem-combo.hpp"
#include "components/float-toggle.hpp"
#include "components/input-float.hpp"
#include "components/input-int.hpp"
#include "components/input-text.hpp"
#include "components/int-toggle.hpp"
#include "components/keybind.hpp"
#include "components/label-settings.hpp"
#include "components/label.hpp"
#include "components/radio.hpp"
#include "components/slider.hpp"
#include "components/toggle.hpp"

namespace eclipse::gui {
    MenuTab::MenuTab(std::string title, bool isSearchedFor)
        : m_title(std::move(title)), m_isSearchedFor(isSearchedFor) {}

    Component* MenuTab::addComponent(ComponentPtr component) {
        return m_components.emplace_back(std::move(component)).get();
    }

    void MenuTab::removeComponent(Component* component) {
        std::erase_if(m_components, [component](auto& ptr) {
            return ptr.get() == component;
        });
    }

    LabelComponent* MenuTab::addLabel(std::string title) {
        return static_cast<LabelComponent*>(this->addComponent(
            std::make_unique<LabelComponent>(std::move(title))
        ));
    }

    ToggleComponent* MenuTab::addToggle(std::string id) {
        std::string name = id;
        return this->addToggle(std::move(name), std::move(id));
    }

    ToggleComponent* MenuTab::addToggle(std::string title, std::string id) {
        return static_cast<ToggleComponent*>(this->addComponent(
            std::make_unique<ToggleComponent>(std::move(id), std::move(title))
        ));
    }

    RadioButtonComponent* MenuTab::addRadioButton(
        std::string title, std::string id, int value
    ) {
        return static_cast<RadioButtonComponent*>(this->addComponent(
            std::make_unique<RadioButtonComponent>(std::move(id), std::move(title), value)
        ));
    }

    ComboComponent* MenuTab::addCombo(
        std::string title, std::string id, std::vector<std::string> items, int value
    ) {
        return static_cast<ComboComponent*>(this->addComponent(
            std::make_unique<ComboComponent>(std::move(id), std::move(title), std::move(items), value)
        ));
    }

    ComboComponent* MenuTab::addCombo(
        std::string id, std::vector<std::string> items, int value
    ) {
        std::string title = id;
        return this->addCombo(std::move(title), std::move(id), std::move(items), value);
    }

    FilesystemComboComponent* MenuTab::addFilesystemCombo(
        std::string title, std::string id, std::filesystem::path directory
    ) {
        return static_cast<FilesystemComboComponent*>(this->addComponent(
            std::make_unique<FilesystemComboComponent>(std::move(id), std::move(title), std::move(directory))
        ));
    }

    FilesystemComboComponent* MenuTab::addFilesystemCombo(
        std::string id, std::filesystem::path directory
    ) {
        std::string title = id;
        return this->addFilesystemCombo(std::move(title), std::move(id), std::move(directory));
    }

    SliderComponent* MenuTab::addSlider(
        std::string title, std::string id, float min, float max, std::string format
    ) {
        return static_cast<SliderComponent*>(this->addComponent(
            std::make_unique<SliderComponent>(std::move(title), std::move(id), min, max, std::move(format))
        ));
    }

    SliderComponent* MenuTab::addSlider(
        std::string id, float min, float max, std::string format
    ) {
        std::string title = id;
        return this->addSlider(std::move(title), std::move(id), min, max, std::move(format));
    }

    InputFloatComponent* MenuTab::addInputFloat(
        std::string title, std::string id, float min, float max, std::string format
    ) {
        return static_cast<InputFloatComponent*>(this->addComponent(
            std::make_unique<InputFloatComponent>(std::move(title), std::move(id), min, max, std::move(format))
        ));
    }

    InputFloatComponent* MenuTab::addInputFloat(
        std::string id, float min, float max, std::string format
    ) {
        std::string title = id;
        return this->addInputFloat(std::move(title), std::move(id), min, max, std::move(format));
    }

    InputIntComponent* MenuTab::addInputInt(
        std::string title, std::string id, int min, int max
    ) {
        return static_cast<InputIntComponent*>(this->addComponent(
            std::make_unique<InputIntComponent>(std::move(title), std::move(id), min, max)
        ));
    }

    InputIntComponent* MenuTab::addInputInt(std::string id, int min, int max) {
        std::string title = id;
        return this->addInputInt(std::move(title), std::move(id), min, max);
    }

    IntToggleComponent* MenuTab::addIntToggle(
        std::string title, std::string id, int min, int max
    ) {
        return static_cast<IntToggleComponent*>(this->addComponent(
            std::make_unique<IntToggleComponent>(std::move(title), std::move(id), min, max)
        ));
    }

    IntToggleComponent* MenuTab::addIntToggle(std::string id, int min, int max) {
        std::string title = id;
        return this->addIntToggle(std::move(title), std::move(id), min, max);
    }

    FloatToggleComponent* MenuTab::addFloatToggle(
        std::string title, std::string id, float min, float max, std::string format
    ) {
        return static_cast<FloatToggleComponent*>(this->addComponent(
            std::make_unique<FloatToggleComponent>(std::move(title), std::move(id), min, max, std::move(format))
        ));
    }

    FloatToggleComponent* MenuTab::addFloatToggle(
        std::string id, float min, float max, std::string format
    ) {
        std::string title = id;
        return this->addFloatToggle(std::move(title), std::move(id), min, max, std::move(format));
    }

    InputTextComponent* MenuTab::addInputText(std::string title, std::string id) {
        return static_cast<InputTextComponent*>(this->addComponent(
            std::make_unique<InputTextComponent>(std::move(title), std::move(id))
        ));
    }

    InputTextComponent* MenuTab::addInputText(std::string id) {
        std::string title = id;
        return this->addInputText(std::move(title), std::move(id));
    }

    ButtonComponent* MenuTab::addButton(std::string title) {
        return static_cast<ButtonComponent*>(this->addComponent(
            std::make_unique<ButtonComponent>(std::move(title))
        ));
    }

    ColorComponent* MenuTab::addColorComponent(
        std::string title, std::string id, bool hasOpacity
    ) {
        return static_cast<ColorComponent*>(this->addComponent(
            std::make_unique<ColorComponent>(std::move(title), std::move(id), hasOpacity)
        ));
    }

    ColorComponent* MenuTab::addColorComponent(std::string id, bool hasOpacity) {
        std::string title = id;
        return this->addColorComponent(std::move(title), std::move(id), hasOpacity);
    }

    KeybindComponent* MenuTab::addKeybind(
        std::string title, std::string id, bool canDelete
    ) {
        return static_cast<KeybindComponent*>(this->addComponent(
            std::make_unique<KeybindComponent>(std::move(title), std::move(id), canDelete)
        ));
    }

    LabelSettingsComponent* MenuTab::addLabelSetting(labels::LabelSettings* settings) {
        return static_cast<LabelSettingsComponent*>(this->addComponent(
            std::make_unique<LabelSettingsComponent>(settings)
        ));
    }

    MenuTab* MenuTab::find(std::string_view name) { return &Engine::get().findTab(name); }

    void Engine::setRenderer(RendererType type) {
        // technical debt: think about how to better handle renderer switching
        type = geode::getMod()->getSettingValue<std::string>("menu-style") == "ImGui"
            ? RendererType::ImGui
            : RendererType::Cocos2d;

        auto tm = ThemeManager::get();
        if (m_renderer && type == m_renderer->getType()) return;
        if (m_renderer) m_renderer->shutdown();

        switch (type) {
            default:
            case RendererType::ImGui:
                m_renderer = std::make_unique<imgui::ImGuiRenderer>();
                break;
            case RendererType::Cocos2d:
                m_renderer = std::make_unique<cocos::CocosRenderer>();
                break;
        }

        m_renderer->init();

        // update blur state
        blur::toggle(
            config::getTemp("blurEnabled", false)
            && type != RendererType::Cocos2d
        );
    }

    RendererType Engine::getRendererType() {
        auto& engine = Engine::get();
        if (!engine.isInitialized()) return RendererType::None;
        if (!engine.m_renderer) return RendererType::None;
        return engine.m_renderer->getType();
    }

    Engine& Engine::get() {
        static Engine s_engine;
        return s_engine;
    }

    void Engine::init() {
        m_initialized = true;
        setRenderer(ThemeManager::get()->getRenderer());
    }

    void Engine::toggle() const {
        if (!m_renderer) return;
        m_renderer->toggle();
    }

    MenuTab& Engine::findTab(std::string_view name) {
        for (auto& tab : m_tabs) {
            if (tab.getTitle() == name) {
                return tab;
            }
        }

        // If the tab does not exist, create a new one.
        m_tabs.emplace_back(std::string(name), false);

        // Make sure built-in tabs are sorted in a specific way and come first
        static constexpr std::array<std::string_view, 11> builtInTabs = {
            "tab.global", "tab.level", "tab.bypass", "tab.player", "tab.bot", "tab.creator",
            "tab.labels", "tab.shortcuts", "tab.keybinds", "tab.interface", "tab.recorder"
        };

        std::ranges::sort(m_tabs, [](auto const& a, auto const& b) {
            auto aIt = std::ranges::find(builtInTabs, a.getTitle());
            auto bIt = std::ranges::find(builtInTabs, b.getTitle());
            if (aIt != builtInTabs.end() && bIt != builtInTabs.end()) {
                return aIt - builtInTabs.begin() < bIt - builtInTabs.begin();
            }

            if (aIt != builtInTabs.end()) return true;
            if (bIt != builtInTabs.end()) return false;

            return a.getTitle() < b.getTitle();
        });

        // tell the renderer to update the tabs if we're past the initialization stage
        if (m_initialized && m_renderer) m_renderer->updateTabs();

        // find the tab again (now that we sorted them)
        return findTab(name);
    }

    Engine::Engine() {
        m_tabs.reserve(16);
    }
}
