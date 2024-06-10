#include "imgui.hpp"

#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>

#include <utils.hpp>
#include <modules/config/config.hpp>

#include "window.hpp"

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
            bool value = checkbox->getValue();
            if (ImGui::Checkbox(checkbox->getTitle().c_str(), &value)) {
                checkbox->setValue(value);
                checkbox->triggerCallback(value);
            }
            if (!checkbox->getDescription().empty()) {
                ImGui::SameLine();
                ImGui::TextDisabled("(?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::TextUnformatted(checkbox->getDescription().c_str());
                    ImGui::EndTooltip();
                }
            }
        } else if (auto* slider = dynamic_cast<SliderComponent*>(component)) {
            auto value = config::get<float>(slider->getId(), 0.0f);
            if (ImGui::SliderFloat(slider->getTitle().c_str(), &value, slider->getMin(), slider->getMax(), slider->getFormat().c_str())) {
                config::set(slider->getId(), value);
                slider->triggerCallback(value);
            }
        } else if (auto* radio = dynamic_cast<RadioButtonComponent*>(component)) {
            int value = config::get<int>(radio->getId(), radio->getValue());
            if (ImGui::RadioButton(radio->getTitle().c_str(), &value, radio->getValue())) {
                config::set(radio->getId(), value);
                radio->triggerCallback(value);
            }
        }
    }

    void ImGuiEngine::draw() {
        for (auto& window : m_windows) {
            window.draw();
        }
    }

    MenuTab* ImGuiEngine::findTab(const std::string& name) {
        for (const auto& tab : m_tabs) {
            if (tab->getTitle() == name) {
                return tab;
            }
        }

        // If the tab does not exist, create a new one.
        auto* tab = new MenuTab(name);
        m_tabs.push_back(tab);

        // Create a new window for the tab.
        m_windows.emplace_back(name, [tab]() {
            for (auto& component : tab->getComponents()) {
                visit(component);
            }
        });

        return tab;
    }

}