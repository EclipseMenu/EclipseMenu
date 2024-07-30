#include "window.hpp"

#include "window/window.hpp"
#include "animation/move-action.hpp"

#include <utils.hpp>
#include <modules/config/config.hpp>

#include <imgui-cocos.hpp>

namespace eclipse::gui::imgui {
    /// @brief Calculate a random window position outside the screen.
    ImVec2 WindowLayout::randomWindowPosition(Window& window) {
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

    /// @brief Auto-stacks windows on screen.
    std::map<Window*, ImVec2> WindowLayout::getStackedPositions() {
        auto firstColumnLock = config::get<bool>("menu.lockFirstColumn", false);
        static std::array<std::string, 1> s_builtInWindows = {"Keybinds"}; // TODO: Add all primary windows
        std::vector<std::string> builtInWindows(s_builtInWindows.begin(), s_builtInWindows.end());
        if (!firstColumnLock) {
            builtInWindows.clear();
        }

        auto snap = config::get<float>("menu.windowSnap", 4.f);
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;

        const auto scale = config::getTemp<float>("UIScale", 1.f);
        float windowWidth = Window::MIN_SIZE.x * scale;
        auto columns = static_cast<int>((screenSize.x - snap) / (windowWidth + snap));

        std::map<Window*, ImVec2> positions;

        // Built-ins go into first column
        float x = snap;
        float y = snap;
        for (auto& title : builtInWindows) {
            auto it = std::find_if(m_windows.begin(), m_windows.end(), [&title](const Window& window) {
                return window.getTitle() == title;
            });

            if (it != m_windows.end()) {
                positions[&(*it)] = ImVec2(x, y);
                y += it->getSize().y + snap;
            }
        }

        if (columns <= 0) return positions;

        // Rest are stacked to take as little space as possible
        auto columnCount = firstColumnLock ? columns - 1 : columns;
        std::vector<float> heights(columnCount, snap);
        for (auto& window : m_windows) {
            // Skip built-in windows
            if (std::find(builtInWindows.begin(), builtInWindows.end(), window.getTitle()) != builtInWindows.end())
                continue;

            // Find the column with the smallest height
            auto min = std::min_element(heights.begin(), heights.end());
            auto index = std::distance(heights.begin(), min);

            // Set the position
            auto windowColumn = firstColumnLock ? index + 1 : index;
            positions[&window] = ImVec2(static_cast<float>(windowColumn) * (windowWidth + snap) + snap, *min);
            *min += window.getSize().y + snap;

            // Update the height
            heights[index] = *min;
        }

        return positions;
    }

    void WindowLayout::stackWindows() {
        double duration = config::get("menu.animationDuration", 0.3);
        auto easingType = config::get("menu.animationEasingType", animation::Easing::Quadratic);
        auto easingMode = config::get("menu.animationEasingMode", animation::EasingMode::EaseInOut);
        auto easing = animation::getEasingFunction(easingType, easingMode);

        auto positions = getStackedPositions();
        for (auto& [window, target] : positions) {
            // Check if the window is already in the correct position
            if (window->getPosition().x == target.x && window->getPosition().y == target.y)
                continue;

            m_actions.push_back(window->animateTo(target, duration, easing, true));
        }
    }

    void WindowLayout::toggle() {
        m_isToggled = !m_isToggled;

        if (!m_isToggled) {
            // Save window states
            std::vector<nlohmann::json> windowStates;
            for (auto& window : m_windows) {
                nlohmann::json windowState;
                to_json(windowState, window);
                windowStates.push_back(windowState);
            }
            config::set("windows", windowStates);
        }

        double duration = config::get("menu.animationDuration", 0.3);
        auto easingType = config::get("menu.animationEasingType", animation::Easing::Quadratic);
        auto easingMode = config::get("menu.animationEasingMode", animation::EasingMode::EaseInOut);
        auto easing = animation::getEasingFunction(easingType, easingMode);

        for (auto& window : m_windows) {
            auto target = m_isToggled ? window.getPosition() : randomWindowPosition(window);
            m_actions.push_back(window.animateTo(target, duration, easing));
        }

        //updateCursorState(m_isToggled);

        m_isAnimating = true;
    }

    bool WindowLayout::shouldRender() {
        // If the GUI is not opened and there are no actions, do not render
        return m_isToggled || !m_actions.empty();
    }

    void WindowLayout::draw() {
        // Setup windows on first draw
        static int frame = 0;
        switch (frame) {
            case 0:
                // Render windows once, to get the correct size
                ImGui::GetStyle().Alpha = 0.f;
                for (auto& window : m_windows) {
                    window.draw();
                }
                {
                    // Load window states
                    auto windowStates = config::get("windows", std::vector<nlohmann::json>());
                    for (auto& windowState : windowStates) {
                        auto title = windowState.at("title").get<std::string>();
                        auto window = std::find_if(m_windows.begin(), m_windows.end(), [&title](const Window& window) {
                            return window.getTitle() == title;
                        });
                        if (window != m_windows.end())
                            from_json(windowState, *window);
                    }
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
        for (auto action : m_actions)
            action->update(deltaTime);

        // Remove finished actions
        m_actions.erase(std::remove_if(m_actions.begin(), m_actions.end(), [](auto action) {
            if (action->isFinished()) {
                action.reset();
                return true;
            }

            return false;
        }), m_actions.end());

        if (!shouldRender()) return;

        //updateCursorState(m_isOpened);

        // Render windows
        for (auto& window : m_windows) {
            window.draw();
        }

        // Auto reset window positions
        auto isDragging = config::getTemp("draggingWindow", false);
        auto stackEnabled = config::get<bool>("menu.stackWindows", true);
        if (m_actions.empty() && !isDragging && stackEnabled)
            stackWindows();

        // Reset dragging state
        config::setTemp("draggingWindow", false);
    }

    std::shared_ptr<MenuTab> WindowLayout::findTab(const std::string& name) {
        for (auto tab : m_tabs) {
            if (tab->getTitle() == name) {
                return tab;
            }
        }

        // If the tab does not exist, create a new one.
        auto tab = std::make_shared<MenuTab>(name);
        m_tabs.push_back(tab);

        // Create a new window for the tab.
        m_windows.emplace_back(name, [this, tab]() {
            for (auto component : tab->getComponents())
                m_style->visit(component.get());
        });

        return tab;
    }
}