#include "imgui.hpp"

#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>

#include <utils.hpp>
#include <modules/config/config.hpp>
#include <modules/gui/color.hpp>

#include "themes/megahack/megahack.hpp"

namespace eclipse::gui::imgui {

    void ImGuiEngine::init() {
        // TODO: swap layouts and themes at some point
        if (m_initialized) return;
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
        // TODO: do the layout thing with this too
        return dynamic_cast<WindowLayout*>(getTheme()->getLayout())->findTab(name);
    }

    Theme* ImGuiEngine::getTheme() {
        // TODO: change this for theme picker
        if (!m_theme) m_theme = new Theme(geode::Mod::get()->getResourcesDir() / "basic.json", new WindowLayout()); 
        return m_theme;
    }

    // TODO: make themes work through json and then convert this to a theme json
    /*void ImGuiEngine::setup() {
        auto &style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(4, 4);
        style.WindowRounding = config::get<float>("menu.windowRounding", 0.f);
        style.FramePadding = ImVec2(4, 2);
        style.FrameRounding = config::get<float>("menu.frameRounding", 0.f);
        style.PopupRounding = config::get<float>("menu.frameRounding", 0.f);
        style.ItemSpacing = ImVec2(12, 2);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.IndentSpacing = 25.0f;
        style.ScrollbarSize = 15.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabMinSize = 5.0f;
        style.GrabRounding = 3.0f;
        style.WindowBorderSize = config::get<float>("menu.borderSize", 0.f);
        style.WindowMinSize = ImVec2(32, 32);
        style.DisplayWindowPadding = ImVec2(0, 0);
        //style.ScaleAllSizes(config::get<float>("UIScale"));
        style.WindowMenuButtonPosition = ImGuiDir_Left;

        auto &colors = style.Colors;
        colors[ImGuiCol_Text] = config::get<Color>("menu.color.text", Color(1.0f, 1.0f, 1.0f, 1.0f));
        colors[ImGuiCol_TextDisabled] = config::get<Color>("menu.color.textDisabled", Color(0.49f, 0.5f, 0.5f, 1.0f));

        colors[ImGuiCol_WindowBg] = config::get<Color>("menu.color.background", Color(0.16f, 0.16f, 0.16f, 1.0f));
        colors[ImGuiCol_FrameBg] = config::get<Color>("menu.color.secondary", Color(0.13f, 0.13f, 0.13f, 1.0f));
        colors[ImGuiCol_TitleBg] =
        colors[ImGuiCol_TitleBgActive] =
        colors[ImGuiCol_TitleBgCollapsed] =
                config::get<Color>("menu.color.accent", Color(0.3f, 0.75f, 0.61f, 1.0f));

        colors[ImGuiCol_Button] = config::get<Color>("menu.color.primary", Color(0.11f, 0.11f, 0.11f, 1.0f));
        colors[ImGuiCol_ButtonHovered] = config::get<Color>("menu.color.hovered", Color(0.3f, 0.76f, 0.6f, 1.0f));
        colors[ImGuiCol_ButtonActive] = config::get<Color>("menu.color.clicked", Color(0.22f, 0.55f, 0.44f, 1.0f));

        colors[ImGuiCol_FrameBgHovered] = config::get<Color>("menu.color.secondary", Color(0.13f, 0.13f, 0.13f, 1.0f));

        colors[ImGuiCol_ScrollbarBg] = config::get<Color>("menu.color.secondary", Color(0.13f, 0.13f, 0.13f, 1.0f));
        colors[ImGuiCol_ScrollbarGrab] = config::get<Color>("menu.color.primary", Color(0.11f, 0.11f, 0.11f, 1.0f));
        colors[ImGuiCol_ScrollbarGrabHovered] = config::get<Color>("menu.color.hovered", Color(0.3f, 0.76f, 0.6f, 1.0f));
        colors[ImGuiCol_ScrollbarGrabActive] = config::get<Color>("menu.color.clicked", Color(0.22f, 0.55f, 0.44f, 1.0f));

        colors[ImGuiCol_CheckMark] = config::get<Color>("menu.color.primary", Color(0.11f, 0.11f, 0.11f, 1.0f));
        colors[ImGuiCol_SliderGrab] = config::get<Color>("menu.color.primary", Color(0.11f, 0.11f, 0.11f, 1.0f));
        colors[ImGuiCol_SliderGrabActive] = config::get<Color>("menu.color.clicked", Color(0.22f, 0.55f, 0.44f, 1.0f));

        colors[ImGuiCol_Border] = config::get<Color>("menu.color.border", Color(0.0f, 0.0f, 0.0f, 1.0f));
        colors[ImGuiCol_BorderShadow] = config::get<Color>("menu.color.border", Color(0.0f, 0.0f, 0.0f, 1.0f));

        colors[ImGuiCol_PopupBg] = config::get<Color>("menu.color.background", Color(0.16f, 0.16f, 0.16f, 1.0f));
        colors[ImGuiCol_Header] = config::get<Color>("menu.color.primary", Color(0.11f, 0.11f, 0.11f, 1.0f));
        colors[ImGuiCol_HeaderHovered] = config::get<Color>("menu.color.hovered", Color(0.3f, 0.76f, 0.6f, 1.0f));
        colors[ImGuiCol_HeaderActive] = config::get<Color>("menu.color.clicked", Color(0.22f, 0.55f, 0.44f, 1.0f));

        ImFont* fnt = ImGui::GetIO().Fonts->AddFontFromFileTTF((geode::Mod::get()->getResourcesDir() / "Rubik-Regular.ttf").c_str(), 15.0f);
        ImGui::GetIO().FontDefault = fnt;
    }*/

}