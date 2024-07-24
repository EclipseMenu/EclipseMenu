#include "imgui.hpp"

#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>

#include <utils.hpp>
#include <modules/config/config.hpp>
#include <modules/gui/color.hpp>

#include "layouts/window/window.hpp"
#include "styles/megahack.hpp"

namespace eclipse::gui::imgui {

    void ImGuiEngine::init() {
        if (m_initialized) return;
        GEODE_ANDROID(ImGuiCocos::get().setForceLegacy(true);) // fixes some random crashes on android
        ImGuiCocos::get()
            .setup([&]() {
                getTheme()->setup();
            })
            .draw([&]() {
                ImGuiCocos::get().setInputMode(ImGuiCocos::InputMode::Default);
                getTheme()->getLayout()->draw();
            });
        m_initialized = true;
    }

    void updateCursorState(bool isOpened) {
        bool canShowInLevel = true;
        if (auto* playLayer = PlayLayer::get()) {
            canShowInLevel = playLayer->m_hasCompletedLevel ||
                             playLayer->m_isPaused ||
                             GameManager::sharedState()->getGameVariable("0024");
        }
        if (isOpened || canShowInLevel)
            PlatformToolbox::showCursor();
        else
            PlatformToolbox::hideCursor();
    }

    bool ImGuiEngine::isToggled() {
        return m_isOpened;
    }

    void ImGuiEngine::toggle() {
        m_isOpened = !m_isOpened;

        updateCursorState(m_isOpened);
        m_theme->getLayout()->toggle();
    }

    MenuTab* ImGuiEngine::findTab(const std::string& name) {
        return dynamic_cast<WindowLayout*>(getTheme()->getLayout())->findTab(name);
    }

    Theme* ImGuiEngine::getTheme() {
        // TODO: change this for theme picker
        if (!m_theme) {
            if (std::filesystem::exists(geode::Mod::get()->getSaveDir() / "themes" / "megahack.json"))
                m_theme = new Theme(geode::Mod::get()->getSaveDir() / "themes" / "megahack.json");
            else
                m_theme = new Theme(geode::Mod::get()->getResourcesDir() / "megahack.zip");
        }

        return m_theme;
    }
}