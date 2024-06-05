#include "imgui.hpp"
#include <modules/config/config.hpp>

#include <imgui-cocos.hpp>

namespace eclipse::gui::imgui {

    void ImGuiEngine::init() {
        if (m_initialized) return;
        ImGuiCocos::get()
            .setup([]() {
                // TODO: Load fonts, styles, etc.
            })
            .draw([&]() {
                draw();
            });
        m_initialized = true;
    }

    void ImGuiEngine::visit(Component* component) {
        // TODO: Move this to a separate file for easier theme customization.
        if (auto* label = dynamic_cast<LabelComponent*>(component)) {
            ImGui::TextWrapped("%s", label->getTitle().c_str());
        } else if (auto* checkbox = dynamic_cast<ToggleComponent*>(component)) {
            bool value = config::get<bool>(checkbox->getId(), false);
            if (ImGui::Checkbox(checkbox->getTitle().c_str(), &value)) {
                config::set(checkbox->getId(), value);
            }
        }
    }

    void ImGuiEngine::draw() {
        for (auto& tab : m_tabs) {
            // TODO: Make this an actual window for better customization.
            if (ImGui::Begin(tab.getTitle().c_str())) {
                for (auto& element : tab.getComponents()) {
                    visit(element);
                }
            }
            ImGui::End();
        }
    }

    MenuTab* ImGuiEngine::findTab(const std::string& name) {
        for (auto& tab : m_tabs) {
            if (tab.getTitle() == name) {
                return &tab;
            }
        }

        // If the tab does not exist, create a new one.
        m_tabs.emplace_back(name);
        return &m_tabs.back();
    }

}