#include "sidebar.hpp"
#include <modules/gui/imgui/imgui.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::imgui {

    void SidebarLayout::init() {
        auto& tabs = Engine::get()->getTabs();
        for (auto& tab : tabs) {
            m_tabs.emplace_back(tab->getTitle(), [tab] {
                for (auto& component : tab->getComponents()) {
                    ImGuiRenderer::get()->visitComponent(component);
                }
            });
        }
    }

    void SidebarLayout::recalculateSize(bool first) {
        auto screenSize = ImGui::GetIO().DisplaySize;

        auto initialSize = ImVec2(first ? 165 * ThemeManager::get()->getGlobalScale() : 250, 700);
        auto xRatio = screenSize.x / 1280;
        auto yRatio = screenSize.y / 720;
        auto ratio = std::min(xRatio, yRatio);
        auto scaledSize = ImVec2(initialSize.x * ratio, initialSize.y * ratio);

        ImGui::SetWindowSize(scaledSize);
        ImGui::SetWindowPos(ImVec2(first ? 10 : (screenSize.x - scaledSize.x - 10), screenSize.y / 2 - scaledSize.y / 2));
    }

    void SidebarLayout::draw() {
        if (!Engine::get()->isToggled()) return;

        auto tm = ThemeManager::get();
        auto scale = tm->getGlobalScale();
        ImGuiStyle* style = &ImGui::GetStyle();
        style->WindowRounding = 15.f * scale;

        // window beginning
        ImGui::Begin("window1", nullptr,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        recalculateSize(true);

        style->Colors[ImGuiCol_WindowShadow] = ImVec4(0.f, 0.f, 0.f, 1.f);
        style->WindowShadowSize = 50.f;

        // Logo (not really)

        ImGui::BeginGroup(); {
            ImGui::BeginChild("Logo", ImVec2(188 * scale, 50 * scale));
            ImGui::SameLine();

            ImGui::SetCursorPosY(11);
            ImGui::TextUnformatted("Eclipse Menu");

            ImGui::EndChild();

            if (ImGui::IsItemClicked(true)) {}

            ImGui::EndGroup();
        }

        ImGui::Spacing();

        // tab buttons

        ImGui::PushStyleColor(ImGuiCol_ChildBg, {0,0,0,0});
        ImGui::BeginChild("tabs");
        ImGui::PopStyleColor();

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10 * scale);

        Tabs currentTabs = Engine::get()->getTabs();

        for (int i = 0; i < currentTabs.size(); i++) {
            std::string it = currentTabs[i]->getTitle().c_str();
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5, 0.5));
            ImGui::PushStyleColor(ImGuiCol_Button, m_selectedTab == i ? style->Colors[ImGuiCol_Button] : ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_Text, currentTabs[i]->isSearchedFor() ? static_cast<ImVec4>(tm->getSearchedColor()) : style->Colors[ImGuiCol_Text]);
            if (ImGui::Button(it.c_str(), ImVec2(160 * scale, 40 * scale))) {
                m_selectedTab = i;
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }

        ImGui::PopStyleVar();

        ImGui::EndChild();

        // user thing

        ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 80 * scale - style->ItemSpacing.y));
        ImGui::BeginChild("User", ImVec2(188, 80));

        ImGui::EndChild();

        ImGui::End();

        // right window

        ImGui::Begin("window2", nullptr,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        recalculateSize(false);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
		ImGui::BeginChild("modules-wrapper", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);
		ImGui::PopStyleColor();

        // Render tabs
        for (int i = 0; i < m_tabs.size(); i++) {
            if (m_selectedTab == i) m_tabs[i].draw();
        }

        ImGui::EndChild();

        ImGui::End();
    }

    void SidebarLayout::toggle(bool state) {

    }
}
