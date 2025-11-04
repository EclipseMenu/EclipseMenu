#include "openhack.hpp"

#include <imgui.h>
#include <modules/gui/color.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::imgui::themes {
    static bool ModernCheckbox(
        std::string const& label, bool& value, bool isSearchedFor, bool hasSettings,
        FunctionRef<void()> postDraw
    ) {
        auto tm = ThemeManager::get();
        auto textColor = value ? tm->getCheckboxCheckmarkColor() : tm->getDisabledColor();
        if (isSearchedFor) textColor = tm->getSearchedColor();

        ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(textColor));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

        auto availWidth = ImGui::GetContentRegionAvail().x;
        if (hasSettings) availWidth -= availWidth * 0.13f;

        bool clicked = ImGui::Button(label.c_str(), ImVec2(availWidth, 0));
        if (clicked) value = !value;
        postDraw();

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();

        return clicked;
    }

    bool OpenHack::checkbox(
        std::string const& label, bool& value, bool isSearchedFor, FunctionRef<void()> postDraw
    ) const {
        return ModernCheckbox(label, value, isSearchedFor, false, postDraw);
    }

    bool OpenHack::checkboxWithSettings(
        std::string const& label, bool& value, bool isSearchedFor, FunctionRef<void()> callback,
        FunctionRef<void()> postDraw, std::string const& popupId
    ) const {
        auto tm = ThemeManager::get();

        bool result = ModernCheckbox(label, value, isSearchedFor, true, std::move(postDraw));

        auto textColor = value ? tm->getCheckboxCheckmarkColor() : tm->getDisabledColor();
        if (isSearchedFor) textColor = tm->getSearchedColor();
        ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(textColor));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));
        ImGui::PushItemWidth(-1);
        auto availWidth = ImGui::GetContentRegionAvail().x;
        auto arrowSize = ImVec2(availWidth * 0.18f, 0);
        ImGui::SameLine(availWidth - (arrowSize.x / 2.f), 0);
        ImGui::SetNextItemWidth(arrowSize.x);
        bool openPopup = ImGui::ArrowButton(fmt::format("##open_{}", label).c_str(), ImGuiDir_Right);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(4);

        std::string popupName = popupId.empty() ? fmt::format("##{}", label) : popupId;
        if (openPopup)
            ImGui::OpenPopup(popupName.c_str());

        ImGui::SetNextWindowSizeConstraints(ImVec2(240 * tm->getGlobalScale(), 0), ImVec2(FLT_MAX, FLT_MAX));
        if (ImGui::BeginPopup(popupName.c_str()/*, ImGuiWindowFlags_NoMove */)) {
            callback();
            ImGui::EndPopup();
        }

        return result;
    }

    bool OpenHack::button(std::string const& text, bool isSearchedFor) const {
        ImGui::PushItemWidth(-1);

        auto tm = ThemeManager::get();

        if (isSearchedFor) {
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(tm->getSearchedColor()));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(tm->getButtonForegroundColor()));
        }

        auto bgColor = tm->getButtonBackgroundColor(); bgColor.a *= 0.4f;
        auto hoveredBgColor = tm->getButtonHoveredBackground(); hoveredBgColor.a *= 0.5f;
        auto activatedBgColor = tm->getButtonActivatedBackground(); activatedBgColor.a *= 0.6f;

        ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(bgColor));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(hoveredBgColor));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(activatedBgColor));

        bool pressed = ImGui::Button(text.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0));

        ImGui::PopStyleColor(4);
        ImGui::PopItemWidth();

        return pressed;
    }

    void OpenHack::init() const {
        auto& style = ImGui::GetStyle();
        style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
        style.WindowMenuButtonPosition = ImGuiDir_Right;
    }

    static void prepareInput(float widthFactor) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));

        // Add a line border on the bottom
        auto drawList = ImGui::GetWindowDrawList();
        auto cursorPos = ImGui::GetCursorScreenPos();
        auto width = ImGui::GetContentRegionAvail().x * widthFactor;

        auto scale = ThemeManager::get()->getGlobalScale();
        auto color = ThemeManager::get()->getButtonBackgroundColor();
        color.a *= 0.4f;

        drawList->AddLine(ImVec2(cursorPos.x, cursorPos.y + 24 * scale), ImVec2(cursorPos.x + width, cursorPos.y + 24 * scale), color);
    }

    void OpenHack::visitInputText(std::shared_ptr<InputTextComponent> const& inputText) const {
        prepareInput(0.5f);
        Theme::visitInputText(inputText);
        ImGui::PopStyleColor(3);
    }

    void OpenHack::visitInputFloat(std::shared_ptr<InputFloatComponent> const& inputFloat) const {
        prepareInput(0.35f);
        Theme::visitInputFloat(inputFloat);
        ImGui::PopStyleColor(3);
    }

    void OpenHack::visitInputInt(std::shared_ptr<InputIntComponent> const& inputInt) const {
        prepareInput(0.35f);
        Theme::visitInputInt(inputInt);
        ImGui::PopStyleColor(3);
    }

    void OpenHack::visitFloatToggle(std::shared_ptr<FloatToggleComponent> const& floatToggle) const {
        prepareInput(0.35f);
        Theme::visitFloatToggle(floatToggle);
        ImGui::PopStyleColor(3);
    }

    void OpenHack::visitIntToggle(std::shared_ptr<IntToggleComponent> const& intToggle) const {
        prepareInput(0.35f);
        Theme::visitIntToggle(intToggle);
        ImGui::PopStyleColor(3);
    }
}
