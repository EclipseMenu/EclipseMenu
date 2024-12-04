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

        auto tm = ThemeManager::get();
        auto scale = tm->getGlobalScale();
        ImGuiStyle* style = &ImGui::GetStyle();
        style->WindowRounding = 15.f * scale;

        // window beginning

        ImGui::Begin(" ", nullptr,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        recalculateSize();

        ImGui::Columns(2);
        ImGui::SetColumnOffset(1, 183 * scale);

        style->Colors[ImGuiCol_WindowShadow] = ImVec4(0.f, 0.f, 0.f, 1.f);
        style->WindowShadowSize = 50.f;

        // Logo (not really)

        ImGui::BeginGroup(); {
            ImGui::BeginChild("Logo", ImVec2(188 * scale, 50 * scale));
            ImGui::SameLine();

            ImGui::SetCursorPosY(11 * scale);
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
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();
        }

        ImGui::PopStyleVar();

        ImGui::EndChild();

        // user thing

        ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 80 * scale - style->ItemSpacing.y));
        ImGui::BeginChild("User", ImVec2(188 * scale, 80 * scale));

        ImGui::EndChild();

        // right column

        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
		ImGui::BeginChild("modules-wrapper", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);
		ImGui::PopStyleColor();

        m_tabs[m_selectedTab].draw();

        ImGui::EndChild();

        ImGui::End();
    }

    void PanelLayout::toggle(bool state) {

    }
}
