#pragma once
#include <modules/gui/imgui/components/theme.hpp>

namespace eclipse::gui::imgui::themes {
    class Megahack final : public Theme {
        bool checkbox(
            std::string const& label, bool& value, bool isSearchedFor,
            std23::function_ref<void()> postDraw
        ) const override;
        bool checkboxWithSettings(
            std::string const& label, bool& value, bool isSearchedFor,
            std23::function_ref<void()> callback,
            std23::function_ref<void()> postDraw,
            std::string const& popupId
        ) const override;
        bool button(std::string const& text, bool isSearchedFor) const override;

        ComponentTheme getTheme() const override { return ComponentTheme::MegaHack; }
    };

    inline static constexpr Megahack MEGAHACK_THEME;
}
