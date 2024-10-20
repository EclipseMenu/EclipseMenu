#pragma once
#include <modules/gui/gui.hpp>
#include <imgui.h>
#include "components/theme.hpp"
#include "layouts/layout.hpp"

namespace eclipse::gui::imgui {

    class FontManager {
    public:
        class FontMetadata {
            ImFont* m_font{};
            std::string m_name;
            std::filesystem::path m_path;

        public:
            FontMetadata(std::string name, std::filesystem::path path)
                : m_name(std::move(name)), m_path(std::move(path)) {}

            [[nodiscard]] std::string_view getName() const;

            /// @brief Returns ImGui Font object. First call initializes the font.
            ImFont* get();
            void load();

        private:
            friend class FontManager;
        };

        /// @brief Scan config and mod resources directory to find available fonts
        static std::vector<FontMetadata> fetchAvailableFonts();
        void fetchFonts();
        /// @brief Returns a vector of already found fonts, without rescanning
        const std::vector<FontMetadata>& getAvailableFonts();

        void init();
        FontMetadata& getFont() { return m_availableFonts[m_selectedFontIndex]; }
        void setFont(std::string_view name);

    private:
        std::vector<FontMetadata> m_availableFonts;
        int m_selectedFontIndex = 0;
    };

    class ImGuiRenderer : public Renderer {
    public:
        static std::shared_ptr<ImGuiRenderer> get() {
            auto engine = Engine::get();
            if (!engine->isInitialized() || engine->getRendererType() != RendererType::ImGui) return nullptr;
            return std::static_pointer_cast<ImGuiRenderer>(engine->getRenderer());
        }

        void init() override;
        void toggle() override;
        [[nodiscard]] bool isToggled() const override;
        void shutdown() override;
        void queueAfterDrawing(const std::function<void()>& func) override;
        void showPopup(const Popup &popup) override;
        [[nodiscard]] RendererType getType() const override { return RendererType::ImGui; }

    public:
        void draw();
        void setLayoutMode(LayoutMode mode);
        void setComponentTheme(ComponentTheme theme);

        void visitComponent(const std::shared_ptr<Component>& component) const;
        bool beginWindow(const std::string& title) const;
        void endWindow() const;
        void reload() const;

        FontManager& getFontManager() { return m_fontManager; }

    private:
        void drawFinished();
        void renderPopups();

        static bool s_initialized;
        bool m_isOpened = false;

        FontManager m_fontManager;

        std::unique_ptr<Theme> m_theme;
        std::unique_ptr<Layout> m_layout;

        bool m_insideDraw = false;
        LayoutMode m_queuedMode = LayoutMode::Tabbed;
        std::vector<std::function<void()>> m_runAfterDrawingQueue;
        std::vector<Popup> m_popups;
    };

}