#include "megahack.hpp"

#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include <misc/cpp/imgui_stdlib.h>

#include <utils.hpp>
#include <modules/config/config.hpp>

#include "animation/easing.hpp"
#include "window/window.hpp"
#include <modules/gui/color.hpp>

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

    void MegahackEngine::init() {
        if (m_initialized) return;
        ImGuiCocos::get()
            .setup([&]() {
                setup();
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

    bool MegahackEngine::isToggled() {
        return m_isOpened;
    }

    void MegahackEngine::toggle() {
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

    bool MegahackEngine::shouldRender() {
        // If the GUI is not opened and there are no actions, do not render
        return m_isOpened || !m_actions.empty();
    }

    void MegahackEngine::visit(Component* component) {
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
            auto value = config::get<float>(inputfloat->getId(), 0.0f);
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

    void MegahackEngine::setup() {
        auto &style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(4, 4);
        style.WindowRounding = config::get<float>("menu.windowRounding", 0.f);
        style.FramePadding = ImVec2(4, 2);
        style.FrameRounding = config::get<float>("menu.frameRounding", 0.f);
        style.PopupRounding = config::get<float>("menu.frameRounding", 0.f);
        style.ItemSpacing = ImVec2(12, 2);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.IndentSpacing = 25.0f;
        style.ScrollbarSize = 15.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabMinSize = 5.0f;
        style.GrabRounding = 3.0f;
        style.WindowBorderSize = config::get<float>("menu.borderSize", 0.f);
        style.WindowMinSize = ImVec2(32, 32);
        style.DisplayWindowPadding = ImVec2(0, 0);
        //style.ScaleAllSizes(config::get<float>("UIScale"));
        style.WindowMenuButtonPosition = ImGuiDir_Left;

        auto &colors = style.Colors;
        colors[ImGuiCol_Text] = config::get<Color>("menu.color.text", Color(1.0f, 1.0f, 1.0f, 1.0f));
        colors[ImGuiCol_TextDisabled] = config::get<Color>("menu.color.textDisabled", Color(0.49f, 0.5f, 0.5f, 1.0f));

        colors[ImGuiCol_WindowBg] = config::get<Color>("menu.color.background", Color(0.16f, 0.16f, 0.16f, 1.0f));
        colors[ImGuiCol_FrameBg] = config::get<Color>("menu.color.secondary", Color(0.13f, 0.13f, 0.13f, 1.0f));
        colors[ImGuiCol_TitleBg] =
        colors[ImGuiCol_TitleBgActive] =
        colors[ImGuiCol_TitleBgCollapsed] =
                config::get<Color>("menu.color.accent", Color(0.3f, 0.75f, 0.61f, 1.0f));

        colors[ImGuiCol_Button] = config::get<Color>("menu.color.primary", Color(0.11f, 0.11f, 0.11f, 1.0f));
        colors[ImGuiCol_ButtonHovered] = config::get<Color>("menu.color.hovered", Color(0.3f, 0.76f, 0.6f, 1.0f));
        colors[ImGuiCol_ButtonActive] = config::get<Color>("menu.color.clicked", Color(0.22f, 0.55f, 0.44f, 1.0f));

        colors[ImGuiCol_FrameBgHovered] = config::get<Color>("menu.color.secondary", Color(0.13f, 0.13f, 0.13f, 1.0f));

        colors[ImGuiCol_ScrollbarBg] = config::get<Color>("menu.color.secondary", Color(0.13f, 0.13f, 0.13f, 1.0f));
        colors[ImGuiCol_ScrollbarGrab] = config::get<Color>("menu.color.primary", Color(0.11f, 0.11f, 0.11f, 1.0f));
        colors[ImGuiCol_ScrollbarGrabHovered] = config::get<Color>("menu.color.hovered", Color(0.3f, 0.76f, 0.6f, 1.0f));
        colors[ImGuiCol_ScrollbarGrabActive] = config::get<Color>("menu.color.clicked", Color(0.22f, 0.55f, 0.44f, 1.0f));

        colors[ImGuiCol_CheckMark] = config::get<Color>("menu.color.primary", Color(0.11f, 0.11f, 0.11f, 1.0f));
        colors[ImGuiCol_SliderGrab] = config::get<Color>("menu.color.primary", Color(0.11f, 0.11f, 0.11f, 1.0f));
        colors[ImGuiCol_SliderGrabActive] = config::get<Color>("menu.color.clicked", Color(0.22f, 0.55f, 0.44f, 1.0f));

        colors[ImGuiCol_Border] = config::get<Color>("menu.color.border", Color(0.0f, 0.0f, 0.0f, 1.0f));
        colors[ImGuiCol_BorderShadow] = config::get<Color>("menu.color.border", Color(0.0f, 0.0f, 0.0f, 1.0f));

        colors[ImGuiCol_PopupBg] = config::get<Color>("menu.color.background", Color(0.16f, 0.16f, 0.16f, 1.0f));
        colors[ImGuiCol_Header] = config::get<Color>("menu.color.primary", Color(0.11f, 0.11f, 0.11f, 1.0f));
        colors[ImGuiCol_HeaderHovered] = config::get<Color>("menu.color.hovered", Color(0.3f, 0.76f, 0.6f, 1.0f));
        colors[ImGuiCol_HeaderActive] = config::get<Color>("menu.color.clicked", Color(0.22f, 0.55f, 0.44f, 1.0f));

        ImFont* fnt = ImGui::GetIO().Fonts->AddFontFromFileTTF(geode::prelude::string::wideToUtf8((geode::Mod::get()->getResourcesDir() / "Rubik-Regular.ttf").wstring()).c_str(), 15.0f);
        ImGui::GetIO().FontDefault = fnt;
    }

    void MegahackEngine::draw() {
        // Setup windows on first draw
        static int frame = 0;
        switch (frame) {
            case 0:
                // Render windows once, to get the correct size
                ImGui::GetStyle().Alpha = 0.f;
                for (auto& window : m_windows) {
                    window.draw();
                }
                frame = 1;
                break;
            case 1:
                // Move windows outside the screen to prepare for animation
                for (auto& window : m_windows) {
                    window.draw();
                    window.setDrawPosition(randomWindowPosition(window));
                }
                ImGui::GetStyle().Alpha = 1.f;
                frame = 2;
                break;
            default:
                break;
        }

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

    MenuTab* MegahackEngine::findTab(const std::string& name) {
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