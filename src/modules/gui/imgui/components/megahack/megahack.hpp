#pragma once
#include <modules/gui/imgui/components/theme.hpp>

namespace eclipse::gui::imgui::themes {

	class Megahack : public Theme {

		bool checkbox(const std::string &label, bool &value, const std::function<void()> &postDraw) const override;
		bool checkboxWithSettings(const std::string &label, bool &value, const std::function<void()> &callback, const std::function<void()> &postDraw) const override;
		bool button(const std::string &text) const override;

	};

}

