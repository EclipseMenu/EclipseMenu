#include "panel.hpp"
#include <modules/gui/imgui/imgui.hpp>
#include <modules/gui/gui.hpp>

namespace eclipse::gui::imgui {

    void PanelLayout::init() {

    }

    void PanelLayout::recalculateSize() {
        auto screenSize = ImGui::GetIO().DisplaySize;

        auto initialSize = ImVec2(800, 600);
        auto xRatio = screenSize.x / 1280;
        auto yRatio = screenSize.y / 720;
        auto ratio = std::min(xRatio, yRatio);
        auto scaledSize = ImVec2(initialSize.x * ratio, initialSize.y * ratio);

        ImGui::SetWindowSize(scaledSize);
        ImGui::SetWindowPos(ImVec2(screenSize.x / 2 - scaledSize.x / 2, screenSize.y / 2 - scaledSize.y / 2));
    }

    void PanelLayout::draw() {
        if (!Engine::get()->isToggled()) return;

        ImGui::Begin(" ", nullptr,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        recalculateSize();

        ImGui::Columns(2);
        ImGui::SetColumnOffset(1, 200);

        ImGui::Text("Eclipse Menu");
        for (auto& tab : Engine::get()->getTabs()) {
            if (ImGui::Button(tab->getTitle().c_str())) {
                geode::log::info("Switching to tab: {}", tab->getTitle());
            }
        }

        if (ImGui::Button("Toggle layout")) {
            ImGuiRenderer::get()->setLayoutMode(LayoutMode::Tabbed);
        }

        ImGui::End();
    }

    void PanelLayout::toggle(bool state) {

    }
}