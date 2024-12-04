#include "imgui.hpp"
#include <modules/config/config.hpp>
#include <imgui-cocos.hpp>
#include <utils.hpp>
#include <modules/gui/theming/manager.hpp>
#include <modules/gui/gui.hpp>
#include <misc/cpp/imgui_stdlib.h>

#include "components/megahack/megahack.hpp"
#include "components/megaoverlay/megaoverlay.hpp"
#include "layouts/tabbed.hpp"
#include "layouts/panel.hpp"
#include "layouts/sidebar.hpp"
#include "modules/gui/gui.hpp"

namespace eclipse::gui::imgui {

    bool ImGuiRenderer::s_initialized = false;

    std::string_view FontManager::FontMetadata::getName() const {
        return m_name;
    }

    ImFont* FontManager::FontMetadata::get() {
        if (m_font) return m_font;

        this->load();
        return m_font;
    }

    void FontManager::FontMetadata::load() {
        auto fontSize = ThemeManager::get()->getFontSize() * DEFAULT_SCALE;
        m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(m_path.string().c_str(), fontSize);
    }

    std::vector<FontManager::FontMetadata> FontManager::fetchAvailableFonts() {
        std::vector<FontMetadata> result;
        auto globFonts = [&](std::filesystem::path const& path) {
            std::filesystem::create_directories(path);
            for (auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.path().extension() != ".ttf") continue;
                auto filename = entry.path().stem().string();
                FontMetadata font {filename, entry.path()};
                result.push_back(font);
            }
        };

        globFonts(geode::Mod::get()->getResourcesDir());
        globFonts(geode::Mod::get()->getConfigDir() / "fonts");

        return result;
    }

    void FontManager::fetchFonts() {
        m_availableFonts = fetchAvailableFonts();
    }

    const std::vector<FontManager::FontMetadata>& FontManager::getAvailableFonts() {
        return m_availableFonts;
    }

    void FontManager::init() {
        this->fetchFonts();

        // load all fonts
        for (auto& font : m_availableFonts) {
            font.load();
        }

        // pick current font
        this->setFont(ThemeManager::get()->getSelectedFont());
    }

    void FontManager::setFont(std::string_view name) {
        if (m_availableFonts.empty()) {
            geode::log::warn("Tried to select a non-existent font: {}", name);
            return;
        }

        for (int i = 0; i < m_availableFonts.size(); i++) {
            if (m_availableFonts[i].getName() != name)
                continue;

            m_selectedFontIndex = i;
            ImGui::GetIO().FontDefault = getFont().get();
            config::setTemp("fontIndex", i);
            return;
        }
    }

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
                m_fontManager.init();
            })
            .draw([this] {
                if (!s_initialized) return;
                ImGuiCocos::get().setInputMode(ImGuiCocos::InputMode::Default);
                draw();
                drawFinished();
            });

        s_initialized = true;
    }

    void ImGuiRenderer::draw() {
        auto scale = ImGui::GetIO().DisplaySize.x / 1920.f;
        config::setTemp("ui.scale", scale);

        m_insideDraw = true;
        if (m_theme) m_theme->update();
        if (m_layout) m_layout->draw();
        renderPopups();
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
        if (m_layout && m_layout->getMode() == mode) return;
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
            case LayoutMode::Sidebar:
                m_layout = std::make_unique<SidebarLayout>();
                break;
        }
        m_layout->init();
        if (s_initialized) m_layout->toggle(m_isOpened);
        m_queuedMode = mode;
    }

    void ImGuiRenderer::setComponentTheme(ComponentTheme theme) {
        auto tm = ThemeManager::get();
        if (m_theme && theme == m_theme->getTheme()) return;
        switch (theme) {
            case ComponentTheme::ImGui:
                m_theme = std::make_unique<Theme>();
                break;
            case ComponentTheme::MegaHack:
            default:
                m_theme = std::make_unique<themes::Megahack>();
                break;
            case ComponentTheme::MegaOverlay:
                m_theme = std::make_unique<themes::MegaOverlay>();
                break;
        }
        if (s_initialized) m_theme->init();
    }

    void ImGuiRenderer::visitComponent(const std::shared_ptr<Component>& component) const {
        if (!m_theme) return;
        component->onUpdate();
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

    void ImGuiRenderer::reload() const {
        ImGuiCocos::get().reload();
    }

    void ImGuiRenderer::queueAfterDrawing(const std::function<void()>& func) {
        m_runAfterDrawingQueue.push_back(func);
    }

    void ImGuiRenderer::showPopup(const Popup& popup) {
        m_popups.push_back(popup);
    }

    void ImGuiRenderer::drawFinished() {
        for (const auto& f : m_runAfterDrawingQueue) {
            f();
        }
        m_runAfterDrawingQueue.clear();
    }

    // https://stackoverflow.com/a/70073137/16349466
    void TextCentered(std::string const& text) {
        float win_width = ImGui::GetWindowSize().x;
        float text_width = ImGui::CalcTextSize(text.c_str()).x;

        // calculate the indentation that centers the text on one line, relative
        // to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
        float text_indentation = (win_width - text_width) * 0.5f;

        // if text is too long to be drawn on one line, `text_indentation` can
        // become too small or even negative, so we check a minimum indentation
        float min_indentation = 20.0f;
        if (text_indentation <= min_indentation) {
            text_indentation = min_indentation;
        }

        ImGui::SameLine(text_indentation);
        ImGui::PushTextWrapPos(win_width - text_indentation);
        ImGui::TextWrapped("%s", text.c_str());
        ImGui::PopTextWrapPos();
    }

    void ImGuiRenderer::renderPopups() {
        std::vector<Popup*> toRemove;

        auto scale = ThemeManager::get()->getGlobalScale();
        constexpr float MIN_POPUP_WIDTH = 400.f;
        constexpr float MAX_POPUP_WIDTH = 600.f;
        constexpr float MIN_POPUP_HEIGHT = 1.f;
        constexpr float MAX_POPUP_HEIGHT = 800.f;

        for (auto& popup : m_popups) {
            auto popupName = fmt::format("{}##{}", popup.getTitle(), popup.getId());
            bool isOpen = false;

            ImGui::OpenPopup(popupName.c_str(), ImGuiPopupFlags_NoOpenOverExistingPopup | ImGuiPopupFlags_NoOpenOverItems);
            ImGui::SetNextWindowSizeConstraints({MIN_POPUP_WIDTH * scale, MIN_POPUP_HEIGHT * scale},
                                                {MAX_POPUP_WIDTH * scale, MAX_POPUP_HEIGHT * scale});
            if (ImGui::BeginPopupModal(popupName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                isOpen = true;
                // if (keybinds::isKeyPressed(keybinds::Keys::Escape)) {
                //     ImGui::CloseCurrentPopup();
                //     ImGui::EndPopup();
                //     toRemove.push_back(&popup);
                //     continue;
                // }

                // Main render logic

                // center text horizontally


                if (!popup.isPrompt()) {
                    TextCentered(popup.getMessage());
                    if (m_theme->button(popup.getButton1(), false)) {
                        popup.getCallback()(true);
                        toRemove.push_back(&popup);
                        ImGui::CloseCurrentPopup();
                    }
                    if (!popup.getButton2().empty()) {
                        // TODO: Make an option for same-line buttons
                        // ImGui::SameLine(0, 2);
                        if (m_theme->button(popup.getButton2(), false)) {
                            popup.getCallback()(false);
                            toRemove.push_back(&popup);
                            ImGui::CloseCurrentPopup();
                        }
                    }
                } else {
                    TextCentered(popup.getMessage());
                    ImGui::PushItemWidth(ImGui::GetWindowWidth() - 20);
                    ImGui::InputText("##prompt", &popup.getPromptValue());
                    ImGui::PopItemWidth();
                    if (m_theme->button(popup.getButton1(), false)) {
                        popup.getPromptCallback()(true, popup.getPromptValue());
                        toRemove.push_back(&popup);
                        ImGui::CloseCurrentPopup();
                    }
                    if (!popup.getButton2().empty()) {
                        // TODO: Make an option for same-line buttons
                        // ImGui::SameLine(0, 2);
                        if (m_theme->button(popup.getButton2(), false)) {
                            popup.getPromptCallback()(false, popup.getPromptValue());
                            toRemove.push_back(&popup);
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }

                ImGui::EndPopup();
            }

            if (!isOpen) {
                toRemove.push_back(&popup);
            }
        }

        // remove all popups that are done (compare by id)
        m_popups.erase(std::ranges::remove_if(m_popups, [&](const Popup& p) {
            for (auto* r : toRemove) {
                if (r->getId() == p.getId()) return true;
            }
            return false;
        }).begin(), m_popups.end());
    }
}
