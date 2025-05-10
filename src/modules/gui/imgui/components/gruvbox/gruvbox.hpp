#pragma once
#include <modules/gui/imgui/components/theme.hpp>

namespace eclipse::gui::imgui::themes {
    class Gruvbox : public Theme {
        bool checkbox(
            const std::string& label, bool& value, bool isSearchedFor,
            const std::function<void()>& postDraw
        ) const override;

        ComponentTheme getTheme() const override { return ComponentTheme::Gruvbox; }
    };
}
