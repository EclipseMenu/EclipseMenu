#pragma once
#include <modules/gui/imgui/components/theme.hpp>

namespace eclipse::gui::imgui::themes {
    class Gruvbox final : public Theme {
        bool checkbox(
            std::string const& label, bool& value, bool isSearchedFor,
            std23::function_ref<void()> postDraw
        ) const override;

        ComponentTheme getTheme() const override { return ComponentTheme::Gruvbox; }
    };

    inline static constexpr Gruvbox GRUVBOX_THEME;
}
