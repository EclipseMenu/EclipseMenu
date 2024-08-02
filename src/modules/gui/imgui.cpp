#include "imgui.hpp"

#include <imgui-cocos.hpp>

#include <utils.hpp>
#include <modules/config/config.hpp>
#include <modules/gui/color.hpp>

#include "layouts/window/window.hpp"

namespace eclipse::gui::imgui {

    void ImGuiEngine::init() {
        if (m_initialized) return;

        // fixes some random crashes on android
        GEODE_ANDROID(
            ImGuiCocos::get().setForceLegacy(true);
        )

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

    std::shared_ptr<MenuTab> ImGuiEngine::findTab(const std::string& name) {
        return std::static_pointer_cast<WindowLayout>(getTheme()->getLayout())->findTab(name);
    }

    std::shared_ptr<Theme> ImGuiEngine::getTheme() {
        // TODO: change this for theme picker
        if (!m_theme) {
            if (std::filesystem::exists(geode::Mod::get()->getSaveDir() / "themes" / "megahack.json"))
                m_theme = std::make_shared<Theme>(geode::Mod::get()->getSaveDir() / "themes" / "megahack.json");
            else
                m_theme = std::make_shared<Theme>(geode::Mod::get()->getResourcesDir() / "megahack.zip");
        }

        return m_theme;
    }
}