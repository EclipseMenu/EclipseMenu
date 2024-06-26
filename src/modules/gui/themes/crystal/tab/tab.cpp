#include "tab.hpp"
#include <modules/config/config.hpp>
/*
namespace eclipse::gui::imgui {

    Tab::Tab(const std::string &title, std::function<void()> onDraw) {
        m_title = title;
        m_drawCallback = std::move(onDraw);
        m_isOpen = true;
    }

    void Tab::draw() {
        ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
        // auto scale = config::getGlobal<float>("UIScale");
        auto scale = 1.0f;

        // Calculate distance to the bottom to make window fit the screen
        auto screenSize = ImGui::GetIO().DisplaySize;
        auto maxSizeH = MAX_SIZE.y * scale;
        // auto bottomMargin = config::get<float>("menu.windowSnap");
        auto bottomMargin = 4.0f;
        if (m_position.y + m_size.y > screenSize.y - bottomMargin - 1) // -1 to prevent flickering
            maxSizeH = screenSize.y - m_position.y - bottomMargin;

        ImGui::SetNextWindowSizeConstraints({MIN_SIZE.x * scale, MIN_SIZE.y * scale},
                                            {MAX_SIZE.x * scale, maxSizeH});

        ImGui::SetNextWindowCollapsed(!m_isOpen, ImGuiCond_Always);

        if (ImGui::Begin(m_title.c_str(), nullptr, flags)) {
            m_drawCallback();
        }

        // Allow moving the window
        if (ImGui::IsMouseDragging(0) && ImGui::IsWindowHovered()) {
            m_position = ImGui::GetWindowPos();

            m_drawPosition = m_position;
        }

        m_size = ImGui::GetWindowSize();
        m_isOpen = !ImGui::IsWindowCollapsed();

        ImGui::SetWindowPos(m_drawPosition);

        ImGui::End();
    }

    void Tab::drawButton() {}

    bool Tab::isOpen() const { return m_isOpen; }

    void Tab::setOpen(bool open) { m_isOpen = open; }

    const std::string &Tab::getTitle() const { return m_title; }

    void Tab::setTitle(const std::string &title) { m_title = title; }
}*/