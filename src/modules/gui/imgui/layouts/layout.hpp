#pragma once

namespace eclipse::gui::imgui {

    enum class LayoutMode {
        Tabbed,
        Panel
    };

    /// @brief Base class for ImGui window layouts
    class Layout {
    public:
        virtual ~Layout() = default;

        virtual void init() = 0;
        virtual void draw() = 0;
        virtual void toggle(bool state) = 0;

        [[nodiscard]] LayoutMode getMode() const { return m_mode; }

    protected:
        LayoutMode m_mode = LayoutMode::Tabbed;
    };

}