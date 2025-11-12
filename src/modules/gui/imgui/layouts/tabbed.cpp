#include "tabbed.hpp"
#include <utils.hpp>
#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/base-component.hpp>
#include <modules/gui/imgui/imgui.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::imgui {

    void TabbedLayout::init() {
        m_windows.clear();

        // Load tabs
        auto& tabs = Engine::get()->getTabs();
        m_windows.reserve(tabs.size());
        for (auto& tab : tabs) {
            m_windows.emplace_back(tab->getTitle(), [tab] {
                for (auto& component : tab->getComponents()) {
                    if (component->getFlags() & ComponentFlags::DisableTabbed) continue;
                    ImGuiRenderer::get()->visitComponent(component);
                }
            });
        }

        // Load saved window states
        {
            auto windowStates = config::get("windows", std::vector<nlohmann::json>());
            for (auto &windowState: windowStates) {
                auto title = windowState.at("title").get<std::string>();
                auto window = std::ranges::find_if(m_windows, [&title](Window const& w) {
                    return w.getTitle() == title;
                });
                if (window != m_windows.end()) {
                    from_json(windowState, *window);
                    window->setDrawPosition(window->getPosition());
                }
            }
        }

        m_preloadStep = config::get("menu.animateWindows", true) ? 0 : 2;
    }

    bool TabbedLayout::shouldRender() const {
        return Engine::get()->isToggled() || wantStayVisible();
    }

    bool TabbedLayout::wantStayVisible() const {
        return !m_actions.empty() || m_preloadStep < 2;
    }

    void TabbedLayout::draw() {
        switch (m_preloadStep) {
            case 0:
                // Render windows once, to get the correct size
                ImGui::GetStyle().Alpha = 0.f;
                ImGui::GetStyle().WindowShadowSize = 0.f;
                for (auto &window: m_windows) {
                    window.draw();
                }
                // Load saved window states
                {
                    auto windowStates = config::get("windows", std::vector<nlohmann::json>());
                    for (auto& windowState: windowStates) {
                        auto title = windowState.at("title").get<std::string>();
                        auto window = std::ranges::find_if(m_windows, [&title](Window const& window) {
                            return window.getTitle() == title;
                        });
                        if (window != m_windows.end())
                            from_json(windowState, *window);
                    }
                }
                m_preloadStep = 1;
                return;
            case 1:
                // Move windows outside the screen to prepare for animation
                for (auto& window : m_windows) {
                    window.draw();
                    window.setDrawPosition(randomWindowPosition(window));
                }
                ImGui::GetStyle().Alpha = 1.f;
                m_preloadStep = 2;
                return;
            default:
                break;
        }

        // Run move actions
        auto deltaTime = ImGui::GetIO().DeltaTime;
        for (auto const& action : m_actions)
            action->update(deltaTime);

        // Remove finished actions
        std::erase_if(m_actions, [](auto& action) {
            return action->isFinished();
        });

        if (!shouldRender()) return;

        // Render windows
        for (auto& window : m_windows) {
            window.draw();
        }

        // Auto reset window positions
        auto isDragging = config::getTemp<"draggingWindow">(false);
        auto stackEnabled = config::get<"menu.stackWindows", bool>(true);
        if (m_actions.empty() && !isDragging && stackEnabled)
            stackWindows();

        // Reset dragging state
        config::setTemp("draggingWindow", false);

        if (m_actions.empty()) {
            m_canForceKeyboardFocus = false;
        }
    }

    void TabbedLayout::toggle(bool state) {
        m_canForceKeyboardFocus = state;

        if (!state) {
            // Save window states
            std::vector<nlohmann::json> windowStates;
            for (auto& window : m_windows) {
                nlohmann::json windowState;
                to_json(windowState, window);
                windowStates.push_back(windowState);
            }
            config::set("windows", windowStates);
        }

        if (config::get<bool>("menu.animateWindows", true)) {
            double duration = config::get("menu.animationDuration", 0.3);
            auto easingType = config::get("menu.animationEasingType", animation::Easing::Quadratic);
            auto easingMode = config::get("menu.animationEasingMode", animation::EasingMode::EaseInOut);
            auto easing = animation::getEasingFunction(easingType, easingMode);

            for (auto& window : m_windows) {
                auto target = state ? window.getPosition() : randomWindowPosition(window);
                m_actions.push_back(window.animateTo(target, duration, easing));
            }
        }
    }

    /// @brief Calculate a random window position outside the screen.
    ImVec2 TabbedLayout::randomWindowPosition(Window const& window) {
        // Calculate target position randomly to be outside the screen
        auto screenSize = ImGui::GetIO().DisplaySize;
        auto windowSize = window.getSize();
        ImVec2 target;

        // Pick a random side of the screen
        switch (utils::random(3)) {
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
    std::vector<std::pair<Window*, ImVec2>> TabbedLayout::getStackedPositions() {
        auto firstColumnLock = config::get<"menu.lockFirstColumn", bool>(false);
        constexpr std::array<std::string_view, 2> BuiltInWindows = {"Interface", "Keybinds"};

        auto tm = ThemeManager::get();
        auto const scale = tm->getGlobalScale();
        auto margin = tm->getWindowMargin() * scale;
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;

        float windowWidth = Window::MIN_SIZE.x * scale;
        auto columns = static_cast<int>((screenSize.x - margin) / (windowWidth + margin));
        auto freeSpace = (screenSize.x - margin - (columns * (windowWidth + margin))) * 0.5f + margin;

        // std::map<Window*, ImVec2> positions;
        std::vector<std::pair<Window*, ImVec2>> positions;
        positions.reserve(m_windows.size());

        // Built-ins go into first column
        float x = freeSpace;
        float y = margin;
        if (firstColumnLock) {
            for (auto title : BuiltInWindows) {
                auto it = std::ranges::find_if(m_windows, [&title](Window const& window) {
                    return window.getTitle() == title;
                });

                if (it != m_windows.end()) {
                    positions.emplace_back(&(*it), ImVec2(x, y));
                    y += it->getSize().y + margin;
                }
            }
        }

        if (columns <= 0) return positions;

        // Rest are stacked to take as little space as possible
        auto columnCount = firstColumnLock ? columns - 1 : columns;
        std::vector<float> heights(columnCount, margin);
        for (auto& window : m_windows) {
            // Skip built-in windows
            if (std::ranges::find(BuiltInWindows, window.getTitle()) != BuiltInWindows.end())
                continue;

            // Find the column with the smallest height
            auto min = std::ranges::min_element(heights);
            auto index = std::distance(heights.begin(), min);

            // Set the position
            auto windowColumn = firstColumnLock ? index + 1 : index;
            positions.emplace_back(&window, ImVec2(static_cast<float>(windowColumn) * (windowWidth + margin) + freeSpace, *min));
            *min += window.getSize().y + margin;

            // Update the height
            heights[index] = *min;
        }

        return positions;
    }

    void TabbedLayout::stackWindows() {
        double duration = config::get<"menu.animationDuration", float>(0.3);
        auto easingType = config::get<"menu.animationEasingType", animation::Easing>(animation::Easing::Quadratic);
        auto easingMode = config::get<"menu.animationEasingMode", animation::EasingMode>(animation::EasingMode::EaseInOut);
        auto easing = animation::getEasingFunction(easingType, easingMode);

        for (auto& [window, target] : getStackedPositions()) {
            // Check if the window is already in the correct position
            if (window->getPosition().x == target.x && window->getPosition().y == target.y)
                continue;

            m_actions.push_back(window->animateTo(target, duration, easing, true));
        }
    }
}
