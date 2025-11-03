#include "tab.hpp"
#include <modules/config/config.hpp>

namespace eclipse::gui::imgui {

    Tab::Tab(std::string title, std::function<void()> onDraw) {
        m_title = std::move(title);
        m_drawCallback = std::move(onDraw);
        m_isOpen = false;
    }

    void Tab::draw() {
        ImGui::Columns(1, nullptr, false);

        ImGui::PushStyleVar(7, 15.f);
        ImGui::BeginChild((m_title + "-tab").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);
        ImGui::PopStyleVar();

        m_drawCallback();

        ImGui::EndChild();
    }

    bool Tab::isOpen() const { return m_isOpen; }

    void Tab::setOpen(bool open) { m_isOpen = open; }

    std::string const& Tab::getTitle() const { return m_title; }

    void Tab::setTitle(std::string title) { m_title = std::move(title); }
}