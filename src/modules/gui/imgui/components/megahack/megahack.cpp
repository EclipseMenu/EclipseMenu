#include "megahack.hpp"

#include <imgui.h>
#include <modules/gui/gui.hpp>
#include <modules/gui/color.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::imgui::themes {
    bool Megahack::checkbox(const std::string &label, bool &value, bool isSearchedFor, const std::function<void()> &postDraw) const {
        auto tm = ThemeManager::get();
        auto textColor = value ? tm->getCheckboxForegroundColor() : tm->getButtonDisabledForeground();
        auto scale = tm->getGlobalScale();

        ImGui::PushItemWidth(-1);

        ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(isSearchedFor ? tm->getSearchedColor() : textColor));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

        bool toggled = ImGui::Button(label.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0));
        if (toggled) value = !value;
        postDraw();

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();

        textColor.a *= ImGui::GetStyle().Alpha;
        ImGui::GetWindowDrawList()->AddRectFilled(
                ImVec2(ImGui::GetItemRectMax().x - 5 * scale, ImGui::GetItemRectMin().y + 1 * scale),
                ImVec2(ImGui::GetItemRectMax().x - 2 * scale, ImGui::GetItemRectMax().y - 1 * scale),
                textColor);

        ImGui::PopItemWidth();

        return toggled;
    }

    bool Megahack::checkboxWithSettings(const std::string &label, bool &value, bool isSearchedFor,
                                        const std::function<void()> &callback,
                                        const std::function<void()> &postDraw,
                                        const std::string& popupId) const {
        auto tm = ThemeManager::get();
        auto textColor = value ? tm->getCheckboxForegroundColor() : tm->getButtonDisabledForeground();
        auto scale = tm->getGlobalScale();

        ImGui::PushItemWidth(-1);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

        ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(isSearchedFor ? tm->getSearchedColor() : textColor));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

        auto availWidth = ImGui::GetContentRegionAvail().x;
        const auto arrowWidth = availWidth * 0.1f;
        auto buttonSize = ImVec2(availWidth - arrowWidth, 0);
        auto arrowSize = ImVec2(arrowWidth, 0);

        bool toggled = ImGui::Button(label.c_str(), buttonSize);
        if (toggled) value = !value;
        postDraw();

        ImGui::SameLine(0, 0);

        ImGui::PopStyleVar(2);
        bool openPopup = ImGui::Button(fmt::format("##open_{}", label).c_str(), arrowSize);
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

        std::string popupName = popupId.empty() ? fmt::format("##{}", label) : popupId;
        if (openPopup)
            ImGui::OpenPopup(popupName.c_str());

        ImGui::SetNextWindowSizeConstraints(ImVec2(240 * tm->getGlobalScale(), 0), ImVec2(FLT_MAX, FLT_MAX));
        if (ImGui::BeginPopup(popupName.c_str(), ImGuiWindowFlags_NoMove)) {
            callback();
            ImGui::EndPopup();
        }

        return toggled;
    }

    bool Megahack::button(const std::string &text, bool isSearchedFor) const {
        ImGui::PushItemWidth(-1);

        auto tm = ThemeManager::get();

        if (isSearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(tm->getSearchedColor()));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

        bool pressed = ImGui::Button(text.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0));

        ImGui::PopStyleColor(isSearchedFor ? 4 : 3);

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

        return pressed;
    }

}
