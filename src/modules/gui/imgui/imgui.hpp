#pragma once
#include <modules/gui/gui.hpp>
#include "components/theme.hpp"
#include "layouts/layout.hpp"

namespace eclipse::gui::imgui {

    enum class LayoutMode {
        Tabbed,
        Panel
    };

    class ImGuiRenderer : public Renderer {
    public:
        static std::shared_ptr<ImGuiRenderer> get() {
            auto engine = Engine::get();
            if (engine->getRendererType() != RendererType::ImGui) return nullptr;
            return std::static_pointer_cast<ImGuiRenderer>(engine->getRenderer());
        }

        void init() override;
        void toggle() override;
        [[nodiscard]] bool isToggled() const override;
        void shutdown() override;

    public:
        void draw();
        void setLayoutMode(LayoutMode mode);
        void setComponentTheme(ComponentTheme theme);

        void visitComponent(const std::shared_ptr<Component>& component) const;
        bool beginWindow(const std::string& title) const;
        void endWindow() const;

    private:
        static bool s_initialized;
        bool m_isOpened = false;

        std::unique_ptr<Theme> m_theme;
        std::unique_ptr<Layout> m_layout;

        bool m_insideDraw = false;
        LayoutMode m_queuedMode = LayoutMode::Tabbed;
    };

}