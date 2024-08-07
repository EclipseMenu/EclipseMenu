#include "imgui.hpp"
#include <modules/config/config.hpp>
#include <imgui-cocos.hpp>
#include <utils.hpp>

#include "layouts/tabbed.hpp"
#include "layouts/panel.hpp"

namespace eclipse::gui::imgui {

    bool ImGuiRenderer::s_initialized = false;

    void ImGuiRenderer::init() {
        if (s_initialized) return;

        setLayoutMode(static_cast<LayoutMode>(config::get("menu.layout", static_cast<int>(LayoutMode::Tabbed))));

        ImGuiCocos::get()
            .setup([]() {

            })
            .draw([&]() {
                if (!s_initialized) return;
                ImGuiCocos::get().setInputMode(ImGuiCocos::InputMode::Default);
                draw();
            });

        s_initialized = true;
    }

    void ImGuiRenderer::draw() {
        m_insideDraw = true;
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
        if (m_layoutMode == mode && m_layout) return;
        if (m_insideDraw) {
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

        m_layoutMode = mode;
        m_queuedMode = mode;
        config::set("menu.layout", static_cast<int>(mode));
    }

}