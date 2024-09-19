#pragma once
#include <modules/gui/imgui/components/theme.hpp>

namespace eclipse::gui::imgui::themes {

	class Megahack : public Theme {

		void visitToggle(const std::shared_ptr<ToggleComponent>& toggle) const override;
		void visitButton(const std::shared_ptr<ButtonComponent>& button) const override;

	};

}

