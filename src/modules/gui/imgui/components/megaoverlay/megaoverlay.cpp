#include "megaoverlay.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <modules/gui/gui.hpp>
#include <modules/gui/color.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::imgui::themes {

	bool MegaOverlay::checkbox(const std::string &label, bool &value, bool isSearchedFor, const std::function<void()> &postDraw) const {
		auto tm = ThemeManager::get();
		auto textColor = value ? tm->getCheckboxForegroundColor() : tm->getDisabledColor();
		auto checkboxColor = value ? tm->getCheckboxCheckmarkColor() : tm->getDisabledColor().darken(0.2f);

		using namespace ImGui;
		ImGuiIO& io = ImGui::GetIO();
		const float cc_sz = 3.0f * io.FontGlobalScale;
		constexpr float cc_pad = 10.0f;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label.c_str());
		const ImVec2 label_size = CalcTextSize(label.c_str(), nullptr, true);

		const ImRect check_bb(
		window->DC.CursorPos,
		ImVec2(label_size.y + style.FramePadding.y * 2 + window->DC.CursorPos.x,
				   label_size.y + style.FramePadding.y * 2 + window->DC.CursorPos.y)
		);
		ItemSize({check_bb.GetWidth(), check_bb.GetHeight() - 3}, style.FramePadding.y);

		ImRect total_bb = check_bb;
		if (label_size.x > 0)
			SameLine(0, style.ItemInnerSpacing.x);
		const ImRect text_bb(
		    ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y),
			ImVec2(label_size.x + window->DC.CursorPos.x,
					   label_size.y + window->DC.CursorPos.y + style.FramePadding.y)
		);
		if (label_size.x > 0) {
			ItemSize({check_bb.GetWidth(), check_bb.GetHeight() - 3}, style.FramePadding.y);
			total_bb = ImRect(check_bb.Min, ImVec2(text_bb.Max.x - cc_pad * 2.0f * io.FontGlobalScale, check_bb.Max.y));
		}

		if (!ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		postDraw();
		if (pressed) value = !value;
		ImVec2 text_pos = text_bb.GetTL();
		if (hovered) {
			ImVec2 text_size = text_bb.GetBR();
			auto fill_color = tm->getButtonHoveredBackground();
			window->DrawList->AddRectFilled(
				ImVec2(check_bb.Min.x, check_bb.Min.y),
				ImVec2(check_bb.Min.x + cc_sz + text_size.x - text_pos.x + cc_pad * 2.0f, check_bb.Max.y), fill_color,
				style.FrameRounding);
		}

		window->DrawList->AddRectFilled(
		    ImVec2(check_bb.Min.x, check_bb.Min.y),
		    ImVec2(check_bb.Min.x + cc_sz, check_bb.Max.y),
		    checkboxColor,
		    style.FrameRounding);

		if (label_size.x > 0.0f) {
			PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(isSearchedFor ? tm->getSearchedColor() : textColor));
			RenderText(ImVec2(check_bb.Min.x + cc_sz + cc_pad, text_pos.y), label.c_str());
			PopStyleColor();
		}

		return pressed;
	}
}
