#include "gruvbox.hpp"

#include <imgui.h>
#include <modules/gui/color.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::imgui::themes {
    bool Gruvbox::checkbox(
        std::string const& label, bool& value, bool isSearchedFor, std23::function_ref<void()> postDraw
    ) const {
        auto tm = ThemeManager::get();
        auto* drawList = ImGui::GetWindowDrawList();
        auto pos = ImGui::GetCursorScreenPos();
        auto scale = tm->getGlobalScale();
        auto padding = 3.f * scale;
        auto size = ImGui::GetFrameHeight() - padding * 2;
        auto frameColor = tm->getFrameBackground();

        auto start = ImVec2(pos.x + padding, pos.y + padding);
        auto end = ImVec2(start.x + size, start.y + size);

        drawList->AddRectFilled(start, end, frameColor, scale * 6);
        if (value) {
            auto checkColor = tm->getCheckboxCheckmarkColor();
            auto checkStart = ImVec2(start.x + 3, start.y + 3);
            auto checkEnd = ImVec2(end.x - 3, end.y - 3);
            drawList->AddRectFilled(checkStart, checkEnd, checkColor, scale * 3);
        }

        auto leftMargin = size + padding * 2;
        auto width = ImGui::GetContentRegionAvail().x;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));
        bool clicked = ImGui::Button("", ImVec2(width - leftMargin, 0));
        postDraw();

        auto buttonPos = ImGui::GetItemRectMin();
        auto buttonSize = ImGui::GetItemRectMax();
        auto fontSize = ImGui::GetFontSize();

        auto buttonMidHeight = buttonPos.y + (buttonSize.y - buttonPos.y) / 2;

        ImGui::PushStyleColor(ImGuiCol_Text,
            static_cast<ImVec4>(isSearchedFor ? tm->getSearchedColor() : tm->getCheckboxForegroundColor()));
        drawList->AddText(ImVec2(buttonPos.x + leftMargin, buttonMidHeight - fontSize / 2),
                          ImGui::GetColorU32(ImGuiCol_Text), label.c_str());

        ImGui::PopStyleColor(4);

        if (clicked) {
            value = !value;
        }

        return clicked;
    }
}
