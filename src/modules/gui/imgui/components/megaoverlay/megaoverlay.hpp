#pragma once
#include <modules/gui/imgui/components/theme.hpp>

namespace eclipse::gui::imgui::themes {
    class MegaOverlay final : public Theme {
        bool checkbox(
            std::string const& label, bool& value, bool isSearchedFor,
            std23::function_ref<void()> postDraw
        ) const override;

        ComponentTheme getTheme() const override { return ComponentTheme::MegaOverlay; }
    };

    inline static constexpr MegaOverlay MEGAOVERLAY_THEME;
}
