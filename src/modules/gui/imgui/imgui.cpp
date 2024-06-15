#include "imgui.hpp"

#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>

#include <utils.hpp>
#include <modules/config/config.hpp>

#include "animation/easing.hpp"
#include "window.hpp"

namespace eclipse::gui::imgui {

    /// @brief Calculate a random window position outside the screen.
    ImVec2 randomWindowPosition(Window &window) {
        // Calculate target position randomly to be outside the screen
        auto screenSize = ImGui::GetIO().DisplaySize;
        auto windowSize = window.getSize();
        ImVec2 target;

        // Pick a random side of the screen
        auto side = utils::random(3);
        switch (side) {
            case 0:
                target = ImVec2(utils::random(screenSize.x - windowSize.x), -windowSize.y);
                break;
            case 1:
                target = ImVec2(utils::random(screenSize.x - windowSize.x), screenSize.y);
                break;
            case 2:
                target = ImVec2(-windowSize.x, utils::random(screenSize.y - windowSize.y));
                break;
            default:
                target = ImVec2(screenSize.x, utils::random(screenSize.y - windowSize.y));
                break;
        }

        return target;
    }

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

    void ImGuiEngine::toggle() {
        m_isOpened = !m_isOpened;

        if (!m_isOpened) {
            // TODO: save window positions
        }

        double duration = config::get("menu.animationDuration", 0.3);
        auto easingType = config::get("menu.animationEasingType", animation::Easing::Quadratic);
        auto easingMode = config::get("menu.animationEasingMode", animation::EasingMode::EaseInOut);
        auto easing = animation::getEasingFunction(easingType, easingMode);

        for (auto& window : m_windows) {
            auto target = m_isOpened ? window.getPosition() : randomWindowPosition(window);
            m_actions.push_back(window.animateTo(target, duration, easing));
        }

        updateCursorState(m_isOpened);

        m_isAnimating = true;
    }

    bool ImGuiEngine::shouldRender() {
        // If the GUI is not opened and there are no actions, do not render
        return m_isOpened || !m_actions.empty();
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
        // Run move actions
        auto deltaTime = ImGui::GetIO().DeltaTime;
        for (auto &action: m_actions) {
            action->update(deltaTime);
        }

        // Remove finished actions
        m_actions.erase(std::remove_if(m_actions.begin(), m_actions.end(), [](auto action) {
            if (action->isFinished()) {
                delete action;
                return true;
            }
            return false;
        }), m_actions.end());

        if (!shouldRender()) return;

        updateCursorState(m_isOpened);

        // Render windows
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