#pragma once
#include "layout.hpp"
#include "../core/window.hpp"
#include "../animation/move-action.hpp"

namespace eclipse::gui::imgui {

    class PanelLayout : public Layout {
    public:
        ~PanelLayout() override = default;

        void init() override;
        void draw() override;
        void toggle(bool state) override;

    public:
        static void recalculateSize();
    };

}