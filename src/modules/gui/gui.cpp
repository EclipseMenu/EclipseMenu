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

    std::shared_ptr<Component> MenuTab::addComponent(std::shared_ptr<Component> component) {
        auto& cmp = m_components.emplace_back(std::move(component));
        cmp->onInit();
        return cmp;
    }

    void MenuTab::removeComponent(std::weak_ptr<Component> component) {
        auto it = std::ranges::find_if(
            m_components, [&component](const std::shared_ptr<Component>& c) {
                return component.lock() == c;
            }
        );

        m_components[it - m_components.begin()].reset();
        m_components.erase(it);
    }

    std::shared_ptr<LabelComponent> MenuTab::addLabel(std::string title) {
        return std::static_pointer_cast<LabelComponent>(this->addComponent(
            std::make_shared<LabelComponent>(std::move(title))
        ));
    }

    std::shared_ptr<ToggleComponent> MenuTab::addToggle(std::string id) {
        std::string name = id;
        return this->addToggle(std::move(name), std::move(id));
    }

    std::shared_ptr<ToggleComponent> MenuTab::addToggle(std::string title, std::string id) {
        return std::static_pointer_cast<ToggleComponent>(this->addComponent(
            std::make_shared<ToggleComponent>(std::move(id), std::move(title))
        ));
    }

    std::shared_ptr<RadioButtonComponent> MenuTab::addRadioButton(
        std::string title, std::string id, int value
    ) {
        return std::static_pointer_cast<RadioButtonComponent>(this->addComponent(
            std::make_shared<RadioButtonComponent>(std::move(id), std::move(title), value)
        ));
    }

    std::shared_ptr<ComboComponent> MenuTab::addCombo(
        std::string title, std::string id, std::vector<std::string> items, int value
    ) {
        return std::static_pointer_cast<ComboComponent>(this->addComponent(
            std::make_shared<ComboComponent>(std::move(id), std::move(title), std::move(items), value)
        ));
    }

    std::shared_ptr<ComboComponent> MenuTab::addCombo(
        std::string id, std::vector<std::string> items, int value
    ) {
        std::string title = id;
        return this->addCombo(std::move(title), std::move(id), std::move(items), value);
    }

    std::shared_ptr<FilesystemComboComponent> MenuTab::addFilesystemCombo(
        std::string title, std::string id, std::filesystem::path directory
    ) {
        return std::static_pointer_cast<FilesystemComboComponent>(this->addComponent(
            std::make_shared<FilesystemComboComponent>(std::move(id), std::move(title), std::move(directory))
        ));
    }

    std::shared_ptr<FilesystemComboComponent> MenuTab::addFilesystemCombo(
        std::string id, std::filesystem::path directory
    ) {
        std::string title = id;
        return this->addFilesystemCombo(std::move(title), std::move(id), std::move(directory));
    }

    std::shared_ptr<SliderComponent> MenuTab::addSlider(
        std::string title, std::string id, float min, float max, std::string format
    ) {
        return std::static_pointer_cast<SliderComponent>(this->addComponent(
            std::make_shared<SliderComponent>(std::move(title), std::move(id), min, max, std::move(format))
        ));
    }

    std::shared_ptr<SliderComponent> MenuTab::addSlider(
        std::string id, float min, float max, std::string format
    ) {
        std::string title = id;
        return this->addSlider(std::move(title), std::move(id), min, max, std::move(format));
    }

    std::shared_ptr<InputFloatComponent> MenuTab::addInputFloat(
        std::string title, std::string id, float min, float max, std::string format
    ) {
        return std::static_pointer_cast<InputFloatComponent>(this->addComponent(
            std::make_shared<InputFloatComponent>(std::move(title), std::move(id), min, max, std::move(format))
        ));
    }

    std::shared_ptr<InputFloatComponent> MenuTab::addInputFloat(
        std::string id, float min, float max, std::string format
    ) {
        std::string title = id;
        return this->addInputFloat(std::move(title), std::move(id), min, max, std::move(format));
    }

    std::shared_ptr<InputIntComponent> MenuTab::addInputInt(
        std::string title, std::string id, int min, int max
    ) {
        return std::static_pointer_cast<InputIntComponent>(this->addComponent(
            std::make_shared<InputIntComponent>(std::move(title), std::move(id), min, max)
        ));
    }

    std::shared_ptr<InputIntComponent> MenuTab::addInputInt(std::string id, int min, int max) {
        std::string title = id;
        return this->addInputInt(std::move(title), std::move(id), min, max);
    }

    std::shared_ptr<IntToggleComponent> MenuTab::addIntToggle(
        std::string title, std::string id, int min, int max
    ) {
        return std::static_pointer_cast<IntToggleComponent>(this->addComponent(
            std::make_shared<IntToggleComponent>(std::move(title), std::move(id), min, max)
        ));
    }

    std::shared_ptr<IntToggleComponent> MenuTab::addIntToggle(std::string id, int min, int max) {
        std::string title = id;
        return this->addIntToggle(std::move(title), std::move(id), min, max);
    }

    std::shared_ptr<FloatToggleComponent> MenuTab::addFloatToggle(
        std::string title, std::string id, float min, float max, std::string format
    ) {
        return std::static_pointer_cast<FloatToggleComponent>(this->addComponent(
            std::make_shared<FloatToggleComponent>(std::move(title), std::move(id), min, max, std::move(format))
        ));
    }

    std::shared_ptr<FloatToggleComponent> MenuTab::addFloatToggle(
        std::string id, float min, float max, std::string format
    ) {
        std::string title = id;
        return this->addFloatToggle(std::move(title), std::move(id), min, max, std::move(format));
    }

    std::shared_ptr<InputTextComponent> MenuTab::addInputText(std::string title, std::string id) {
        return std::static_pointer_cast<InputTextComponent>(this->addComponent(
            std::make_shared<InputTextComponent>(std::move(title), std::move(id))
        ));
    }

    std::shared_ptr<InputTextComponent> MenuTab::addInputText(std::string id) {
        std::string title = id;
        return this->addInputText(std::move(title), std::move(id));
    }

    std::shared_ptr<ButtonComponent> MenuTab::addButton(std::string title) {
        return std::static_pointer_cast<ButtonComponent>(this->addComponent(
            std::make_shared<ButtonComponent>(std::move(title))
        ));
    }

    std::shared_ptr<ColorComponent> MenuTab::addColorComponent(
        std::string title, std::string id, bool hasOpacity
    ) {
        return std::static_pointer_cast<ColorComponent>(this->addComponent(
            std::make_shared<ColorComponent>(std::move(title), std::move(id), hasOpacity)
        ));
    }

    std::shared_ptr<ColorComponent> MenuTab::addColorComponent(std::string id, bool hasOpacity) {
        std::string title = id;
        return this->addColorComponent(std::move(title), std::move(id), hasOpacity);
    }

    std::shared_ptr<KeybindComponent> MenuTab::addKeybind(
        std::string title, std::string id, bool canDelete
    ) {
        return std::static_pointer_cast<KeybindComponent>(this->addComponent(
            std::make_shared<KeybindComponent>(std::move(title), std::move(id), canDelete)
        ));
    }

    std::shared_ptr<LabelSettingsComponent> MenuTab::addLabelSetting(labels::LabelSettings* settings) {
        auto labelSettings = std::make_shared<LabelSettingsComponent>(settings);
        addComponent(labelSettings);
        return labelSettings;
    }

    std::shared_ptr<MenuTab> MenuTab::find(std::string_view name) { return Engine::get()->findTab(name); }

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
                m_renderer = std::make_shared<imgui::ImGuiRenderer>();
                break;
            case RendererType::Cocos2d:
                m_renderer = std::make_shared<cocos::CocosRenderer>();
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
        auto engine = Engine::get();
        if (!engine->isInitialized()) return RendererType::None;
        if (!engine->m_renderer) return RendererType::None;
        return engine->m_renderer->getType();
    }

    std::shared_ptr<Engine> Engine::get() {
        static auto s_engine = std::make_shared<Engine>();
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

    std::shared_ptr<MenuTab> Engine::findTab(std::string_view name) {
        for (auto tab : m_tabs) {
            if (tab->getTitle() == name) {
                return tab;
            }
        }

        // If the tab does not exist, create a new one.
        auto tab = std::make_shared<MenuTab>(std::string(name), false);
        m_tabs.push_back(tab);

        // Make sure built-in tabs are sorted in a specific way and come first
        static constexpr std::array<std::string_view, 11> builtInTabs = {
            "tab.global", "tab.level", "tab.bypass", "tab.player", "tab.bot", "tab.creator",
            "tab.labels", "tab.shortcuts", "tab.keybinds", "tab.interface", "tab.recorder"
        };

        std::ranges::sort(m_tabs, [](const auto& a, const auto& b) {
            auto aIt = std::ranges::find(builtInTabs, a->getTitle());
            auto bIt = std::ranges::find(builtInTabs, b->getTitle());
            if (aIt != builtInTabs.end() && bIt != builtInTabs.end()) {
                return aIt - builtInTabs.begin() < bIt - builtInTabs.begin();
            }

            if (aIt != builtInTabs.end()) return true;
            if (bIt != builtInTabs.end()) return false;

            return a->getTitle() < b->getTitle();
        });

        // tell the renderer to update the tabs if we're past the initialization stage
        if (m_initialized && m_renderer) m_renderer->updateTabs();

        return tab;
    }
}
