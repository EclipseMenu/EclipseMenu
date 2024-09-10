#pragma once

namespace eclipse::gui::imgui {

    /// @brief Base class for ImGui window layouts
    class Layout {
    public:
        virtual ~Layout() = default;

        virtual void init() = 0;
        virtual void draw() = 0;
        virtual void toggle(bool state) = 0;
    };

}