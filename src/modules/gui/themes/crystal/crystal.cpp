#include "crystal.hpp"

#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include <misc/cpp/imgui_stdlib.h>

#include <utils.hpp>
#include <modules/config/config.hpp>

/*namespace eclipse::gui::imgui {
    void CrystalEngine::init() {
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

    void updateCursorState(bool isOpened) {
        bool canShowInLevel = true;
        if (auto* playLayer = PlayLayer::get()) {
            canShowInLevel = playLayer->m_hasCompletedLevel ||
                             playLayer->m_isPaused ||
                             GameManager::sharedState()->getGameVariable("0024");
        }
        if (isOpened || canShowInLevel)
            PlatformToolbox::showCursor();
        else
            PlatformToolbox::hideCursor();
    }

    bool CrystalEngine::isToggled() {
        return m_isOpened;
    }

    void CrystalEngine::toggle() {
        m_isOpened = !m_isOpened;

        if (!m_isOpened) {
            // TODO: save window positions
        }

        updateCursorState(m_isOpened);

        m_isAnimating = true;
    }

    bool CrystalEngine::shouldRender() {
        // If the GUI is not opened and there are no actions, do not render
        return m_isOpened;
    }

    void CrystalEngine::visit(Component* component) {
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
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.4f);
            if (ImGui::SliderFloat(slider->getTitle().c_str(), &value, slider->getMin(), slider->getMax(), slider->getFormat().c_str())) {
                config::set(slider->getId(), value);
                slider->triggerCallback(value);
            }
            ImGui::PopItemWidth();
        } else if (auto* inputfloat = dynamic_cast<InputFloatComponent*>(component)) {
            auto value = config::get<float>(slider->getId(), 0.0f);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
            if (ImGui::InputFloat(inputfloat->getTitle().c_str(), &value, 0, 0, inputfloat->getFormat().c_str())) {

                if(value < inputfloat->getMin()) value = inputfloat->getMin();
                if(value > inputfloat->getMax()) value = inputfloat->getMax();

                config::set(inputfloat->getId(), value);
                inputfloat->triggerCallback(value);
            }
            ImGui::PopItemWidth();
        } else if (auto* floattoggle = dynamic_cast<FloatToggleComponent*>(component)) {
            float value = config::get<float>(floattoggle->getId(), 0.0f);
            bool toggle = config::get<bool>(floattoggle->getId() + ".toggle", false);

            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
            if (ImGui::InputFloat(("##" + floattoggle->getTitle()).c_str(), &value, 0, 0, floattoggle->getFormat().c_str())) {
                if(value < floattoggle->getMin()) value = floattoggle->getMin();
                if(value > floattoggle->getMax()) value = floattoggle->getMax();

                config::set(floattoggle->getId(), value);
                floattoggle->triggerCallback(value);
            }
            ImGui::PopItemWidth();

            ImGui::SameLine();
            if (ImGui::Checkbox(floattoggle->getTitle().c_str(), &toggle)) {
                config::set(floattoggle->getId() + ".toggle", toggle);
                floattoggle->triggerCallback();
            }

        } else if (auto* radio = dynamic_cast<RadioButtonComponent*>(component)) {
            int value = config::get<int>(radio->getId(), radio->getValue());
            if (ImGui::RadioButton(radio->getTitle().c_str(), &value, radio->getValue())) {
                config::set(radio->getId(), value);
                radio->triggerCallback(value);
            }
        } else if (auto* inputtext = dynamic_cast<InputTextComponent*>(component)) {
            std::string value = config::get<std::string>(inputtext->getId(), "");
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            if (ImGui::InputText(inputtext->getTitle().c_str(), &value)) {
                config::set(inputtext->getId(), value);
                inputtext->triggerCallback(value);
            }
            ImGui::PopItemWidth();
        } else if (auto* button = dynamic_cast<ButtonComponent*>(component)) {
            if (ImGui::Button(button->getTitle().c_str())) {
                button->triggerCallback();
            }
        }
    }

    void CrystalEngine::draw() {
        updateCursorState(m_isOpened);

        // Render windows
        for (auto& window : m_windows) {
            window.draw();
        }
    }

    MenuTab* CrystalEngine::findTab(const std::string& name) {
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

}*/