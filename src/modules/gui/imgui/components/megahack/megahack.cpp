#include "megahack.hpp"

#include <imgui.h>
#include <modules/gui/gui.hpp>
#include <modules/gui/color.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::imgui::themes {

    void Megahack::visitToggle(const std::shared_ptr<ToggleComponent>& toggle) const {
        auto tm = ThemeManager::get();

        bool value = toggle->getValue();
        auto textColor = value ? tm->getButtonForegroundColor() : tm->getButtonDisabledForeground();
        auto scale = tm->getGlobalScale();
        ImGui::PushItemWidth(-1);

        if (auto options = toggle->getOptions().lock()) {
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(textColor));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

            auto availWidth = ImGui::GetContentRegionAvail().x;
            const auto arrowWidth = availWidth * 0.1f * scale;
            auto buttonSize = ImVec2(availWidth - arrowWidth, 0);
            auto arrowSize = ImVec2(arrowWidth, 0);

            if (ImGui::Button(toggle->getTitle().c_str(), buttonSize)) {
                toggle->setValue(!value);
                toggle->triggerCallback(!value);
            }

            handleTooltip(toggle->getDescription());
            if (toggle->hasKeybind())
                handleKeybindMenu(toggle->getId());

            ImGui::SameLine(0, 0);

            ImGui::PopStyleVar(2);
            bool openPopup = ImGui::Button(fmt::format("##open_{}", toggle->getId()).c_str(), arrowSize);
            ImGui::PopItemWidth();
            ImGui::PopStyleColor(4);

            auto top = ImGui::GetItemRectMin().y + (4.5f * scale);
            auto bottom = ImGui::GetItemRectMax().y - (4.5f * scale);
            auto right = ImGui::GetItemRectMax().x - (4.5f * scale);
            auto side = bottom - top;
            auto left = right - side;
            textColor.a *= ImGui::GetStyle().Alpha;
            ImU32 triangleColor = textColor;
            ImGui::GetWindowDrawList()->AddTriangleFilled(
                ImVec2(right, top),
                ImVec2(left, bottom),
                ImVec2(right, bottom),
                triangleColor
            );

            std::string popupName = fmt::format("##{}", toggle->getId());
            if (openPopup)
                ImGui::OpenPopup(popupName.c_str());

            ImGui::SetNextWindowSizeConstraints(ImVec2(240 * tm->getGlobalScale(), 0), ImVec2(FLT_MAX, FLT_MAX));
            if (ImGui::BeginPopup(popupName.c_str(), ImGuiWindowFlags_NoMove)) {
                for (auto& comp : options->getComponents())
                    visit(comp);

                ImGui::EndPopup();
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(textColor));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

            if (ImGui::Button(toggle->getTitle().c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                toggle->setValue(!value);
                toggle->triggerCallback(!value);
            }

            handleTooltip(toggle->getDescription());
            if (toggle->hasKeybind())
                handleKeybindMenu(toggle->getId());

            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar();

            textColor.a *= ImGui::GetStyle().Alpha;
            ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(ImGui::GetItemRectMax().x - 5 * scale, ImGui::GetItemRectMin().y + 1 * scale),
                    ImVec2(ImGui::GetItemRectMax().x - 2 * scale, ImGui::GetItemRectMax().y - 1 * scale),
                    textColor);

            ImGui::PopItemWidth();
        }
    }

    void Megahack::visitButton(const std::shared_ptr<ButtonComponent>& button) const {
        ImGui::PushItemWidth(-1);

        auto tm = ThemeManager::get();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

        if (ImGui::Button(button->getTitle().c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            button->triggerCallback();
        }

        ImGui::PopStyleColor(3);

        handleTooltip(button->getDescription());
        if (button->hasKeybind())
            handleKeybindMenu(fmt::format("button.{}", button->getId()));

        // Draw two lines
        bool isMouseOver = ImGui::IsItemHovered();
        bool isItemActive = ImGui::IsItemActive();

        Color color;
        if (isMouseOver) {
            color = isItemActive ? tm->getButtonActivatedBackground() : tm->getButtonHoveredBackground();
        } else {
            color = tm->getButtonBackgroundColor();
        }

        color.a *= ImGui::GetStyle().Alpha;
        auto scale = tm->getGlobalScale();

        ImGui::GetWindowDrawList()->AddLine(
                ImVec2(ImGui::GetItemRectMin().x + 1, ImGui::GetItemRectMin().y + 1),
                ImVec2(ImGui::GetItemRectMin().x + 1, ImGui::GetItemRectMax().y - 3),
                color, 2.5f * scale);

        ImGui::GetWindowDrawList()->AddLine(
                ImVec2(ImGui::GetItemRectMax().x - 2, ImGui::GetItemRectMin().y + 1),
                ImVec2(ImGui::GetItemRectMax().x - 2, ImGui::GetItemRectMax().y - 3),
                color, 2.5f * scale);

        ImGui::PopItemWidth();
    }

}
