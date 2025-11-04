#include "window.hpp"
#include <modules/config/config.hpp>
#include <modules/gui/imgui/imgui.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::imgui {

    Window::Window(std::string title, Function<void()>&& onDraw) {
        m_title = std::move(title);
        m_drawCallback = std::move(onDraw);
        m_position = ImVec2(0, 0);
        m_drawPosition = m_position;
        m_size = ImVec2(MIN_SIZE);
        m_isOpen = true;
    }

    void Window::draw() {
        if (!isOnScreen())
            return;

        auto tm = ThemeManager::get();
        auto scale = tm->getGlobalScale();

        // Calculate distance to the bottom to make window fit the screen
        auto screenSize = ImGui::GetIO().DisplaySize;
        auto maxSizeH = MAX_SIZE.y * scale;
        auto bottomMargin = tm->getWindowMargin();
        if (m_position.y + m_size.y > screenSize.y - bottomMargin - 1) // -1 to prevent flickering
            maxSizeH = screenSize.y - m_position.y - bottomMargin;

        ImGui::SetNextWindowSizeConstraints({MIN_SIZE.x * scale, MIN_SIZE.y * scale},
                                            {MAX_SIZE.x * scale, maxSizeH});

        ImGui::SetNextWindowCollapsed(!m_isOpen, ImGuiCond_Always);


        if (ImGuiRenderer::get()->beginWindow(m_title)) {
            m_drawCallback();
        }

        // Allow moving the window
        if (ImGui::IsMouseDragging(0) && ImGui::IsWindowHovered()) {
            m_position = ImGui::GetWindowPos();

            m_drawPosition = m_position;

            config::setTemp("draggingWindow", true);
        }

        m_size = ImGui::GetWindowSize();
        m_isOpen = !ImGui::IsWindowCollapsed();

        ImGui::SetWindowPos(m_drawPosition);

        ImGuiRenderer::get()->endWindow();
    }

    bool Window::isOpen() const { return m_isOpen; }

    void Window::setOpen(bool open) { m_isOpen = open; }

    bool Window::isOnScreen() const {
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        return m_position.x + m_size.x >= 0 &&
               m_position.y + m_size.y >= 0 &&
               m_position.x < screenSize.x &&
               m_position.y < screenSize.y;
    }

    std::string const& Window::getTitle() const { return m_title; }

    void Window::setTitle(std::string title) { m_title = std::move(title); }

    ImVec2 const& Window::getPosition() const { return m_position; }

    void Window::setPosition(ImVec2 const& position) { m_position = position; }

    ImVec2 const& Window::getDrawPosition() const { return m_drawPosition; }

    void Window::setDrawPosition(ImVec2 const& position) { m_drawPosition = position; }

    ImVec2 const& Window::getSize() const { return m_size; }

    void Window::setSize(ImVec2 const& size) { m_size = size; }

    std::unique_ptr<animation::MoveAction> Window::animateTo(ImVec2 const& target, double duration, animation::EasingFunction easing, bool useRealPosition) {
        auto action = animation::MoveAction::create(duration, &m_drawPosition, target, easing);

        if (useRealPosition)
            m_position = target;

        return std::move(action);
    }

    void to_json(nlohmann::json& j, Window const& e) {
        auto pos = nlohmann::json{
            {"x", e.getPosition().x},
            {"y", e.getPosition().y}
        };
        j = nlohmann::json{
            {"pos",   std::move(pos)},
            {"open",  e.isOpen()},
            {"title", e.getTitle()}
        };
    }

    void from_json(nlohmann::json const& j, Window& e) {
        auto pos = j.at("pos");
        e.setPosition({pos.at("x").get<float>(), pos.at("y").get<float>()});
        e.setOpen(j.at("open").get<bool>());
        e.setTitle(j.at("title").get<std::string>());
    }

}
