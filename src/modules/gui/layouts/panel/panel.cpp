#include "panel.hpp"

#include "tab/tab.hpp"

#include <utils.hpp>
#include <modules/config/config.hpp>

#include <imgui-cocos.hpp>

namespace eclipse::gui::imgui {
    void PanelLayout::toggle() {
        m_isToggled = !m_isToggled;
    }

    bool PanelLayout::shouldRender() {
        // If the GUI is not opened, do not render
        return m_isToggled;
    }

    void PanelLayout::draw() {  
        if (!shouldRender()) return;

        ImGuiStyle* style = &ImGui::GetStyle();

        // window stuff

        ImGui::SetNextWindowSize({ 800, 600 });
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        style->WindowRounding = 15.f;
        style->Colors[ImGuiCol_FrameBg] = {0, 0, 0, 0};

        ImGui::Begin(" ", 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
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

        for (int i = 0; i < m_realTabs.size(); i++) {
            std::string it = m_realTabs[i].getTitle();
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
        for (int i = 0; i < m_realTabs.size(); i++) {
            if (m_selectedTab == i) m_realTabs[i].draw();
        }

        ImGui::EndChild();

        ImGui::End();
    }

    std::shared_ptr<MenuTab> PanelLayout::findTab(const std::string& name) {
        for (auto tab : m_tabs) {
            if (tab->getTitle() == name) {
                return tab;
            }
        }

        // If the tab does not exist, create a new one.
        auto tab = std::make_shared<MenuTab>(name);
        m_tabs.push_back(tab);

        // Create a new window for the tab.
        m_realTabs.emplace_back(name, [this, tab]() {
            for (auto component : tab->getComponents())
                m_style->visit(component.get());
        });

        return tab;
    }
}