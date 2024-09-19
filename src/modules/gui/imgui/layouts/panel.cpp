#include "panel.hpp"
#include <modules/gui/imgui/imgui.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::imgui {

    void PanelLayout::init() {
        auto& tabs = Engine::get()->getTabs();
        for (auto& tab : tabs) {
            m_tabs.emplace_back(tab->getTitle(), [tab] {
                for (auto& component : tab->getComponents()) {
                    ImGuiRenderer::get()->visitComponent(component);
                }
            });
        }
    }

    void PanelLayout::recalculateSize() {
        auto screenSize = ImGui::GetIO().DisplaySize;

        auto initialSize = ImVec2(800, 600);
        auto xRatio = screenSize.x / 1280;
        auto yRatio = screenSize.y / 720;
        auto ratio = std::min(xRatio, yRatio);
        auto scaledSize = ImVec2(initialSize.x * ratio, initialSize.y * ratio);

        ImGui::SetWindowSize(scaledSize);
        ImGui::SetWindowPos(ImVec2(screenSize.x / 2 - scaledSize.x / 2, screenSize.y / 2 - scaledSize.y / 2));
    }

    void PanelLayout::draw() {
        if (!Engine::get()->isToggled()) return;

        ImGuiStyle* style = &ImGui::GetStyle();
        style->WindowRounding = 15.f;

        // window beginning

        ImGui::Begin(" ", nullptr,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        recalculateSize();

        ImGui::Columns(2);
        ImGui::SetColumnOffset(1, 183);

        // Logo (not really)

        ImGui::BeginGroup(); {
            ImGui::BeginChild("Logo", ImVec2(188, 50));
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
        ImGui::BeginChild("tabs", ImVec2(186, 480));
        ImGui::PopStyleColor();

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);

        Tabs currentTabs = Engine::get()->getTabs();

        for (int i = 0; i < currentTabs.size(); i++) {
            std::string it = currentTabs[i]->getTitle().c_str();
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5, 0.5));
            ImGui::PushStyleColor(ImGuiCol_Button, m_selectedTab == i ? style->Colors[ImGuiCol_Button] : ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_Text, style->Colors[ImGuiCol_Text]);
            if (ImGui::Button(it.c_str(), ImVec2(160, 40))) {
                m_selectedTab = i;
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }

        ImGui::PopStyleVar();

        ImGui::EndChild();

        // user thing

        ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 80 - style->ItemSpacing.y));
        ImGui::BeginChild("User", ImVec2(188, 80));

        ImGui::EndChild();

        // right column

        ImGui::NextColumn();
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

    void PanelLayout::toggle(bool state) {

    }
}
