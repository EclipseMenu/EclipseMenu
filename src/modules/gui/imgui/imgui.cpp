#include "imgui.hpp"
#include <modules/config/config.hpp>
#include <imgui-cocos.hpp>
#include <utils.hpp>
#include <modules/gui/theming/manager.hpp>

#include "components/megahack/megahack.hpp"
#include "layouts/tabbed.hpp"
#include "layouts/panel.hpp"

namespace eclipse::gui::imgui {

    bool ImGuiRenderer::s_initialized = false;

    void ImGuiRenderer::init() {
        if (s_initialized) return;

        // first call will initialize the layout/theme
        setLayoutMode(ThemeManager::get()->getLayoutMode());
        setComponentTheme(ThemeManager::get()->getComponentTheme());

        ImGuiCocos::get()
            .setup([this] {
                auto& io = ImGui::GetIO();
                auto& style = ImGui::GetStyle();
                io.IniFilename = nullptr;
                style.DisplaySafeAreaPadding = ImVec2(0, 0);
                if (m_theme) m_theme->init();
            })
            .draw([this] {
                if (!s_initialized) return;
                ImGuiCocos::get().setInputMode(ImGuiCocos::InputMode::Default);
                draw();
            });

        s_initialized = true;
    }

    void ImGuiRenderer::draw() {
        auto scale = ImGui::GetIO().DisplaySize.x / 1920.f;
        config::setTemp("ui.scale", scale);

        m_insideDraw = true;
        if (m_insideDraw) m_theme->update();
        if (m_layout) m_layout->draw();
        m_insideDraw = false;
        setLayoutMode(m_queuedMode);
    }

    void ImGuiRenderer::toggle() {
        m_isOpened = !m_isOpened;

        utils::updateCursorState(m_isOpened);
        m_layout->toggle(m_isOpened);
    }

    bool ImGuiRenderer::isToggled() const {
        return m_isOpened;
    }

    void ImGuiRenderer::shutdown() {
        ImGuiCocos::get().destroy();
        s_initialized = false;
    }

    void ImGuiRenderer::setLayoutMode(LayoutMode mode) {
        auto tm = ThemeManager::get();
        if (tm->getLayoutMode() == mode && m_layout) return;
        if (m_insideDraw) {
            // replacing layout during rendering is a bad idea
            // so we queue the change after the frame is finished
            m_queuedMode = mode;
            return;
        }
        switch (mode) {
            case LayoutMode::Tabbed:
                m_layout = std::make_unique<TabbedLayout>();
                break;
            case LayoutMode::Panel:
                m_layout = std::make_unique<PanelLayout>();
                break;
        }
        m_layout->init();
        if (s_initialized) m_layout->toggle(m_isOpened);

        tm->setLayoutMode(mode);
        m_queuedMode = mode;
    }

    void ImGuiRenderer::setComponentTheme(ComponentTheme theme) {
        auto tm = ThemeManager::get();
        if (tm->getComponentTheme() == theme && m_theme) return;
        switch (theme) {
            case ComponentTheme::ImGui:
                m_theme = std::make_unique<Theme>();
                break;
            case ComponentTheme::MegaHack:
            default:
                m_theme = std::make_unique<themes::Megahack>();
                break;
        }
        if (s_initialized) m_theme->init();
        tm->setComponentTheme(theme);
    }

    void ImGuiRenderer::visitComponent(const std::shared_ptr<Component>& component) const {
        if (!m_theme) return;
        m_theme->visit(component);
    }

    bool ImGuiRenderer::beginWindow(const std::string &title) const {
        if (!m_theme) {
            geode::log::error("beginWindow called without initialized theme");
            return false;
        }
        return m_theme->beginWindow(title);
    }

    void ImGuiRenderer::endWindow() const {
        if (!m_theme) return;
        m_theme->endWindow();
    }
}
