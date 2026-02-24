#pragma once

#include <functional.hpp>
#include <string>
#include <functional>
#include <imgui.h>

namespace eclipse::gui::imgui {

    /// @brief A Tab that can contain components, has its own title and position.
    /// Supports drag and drop, can be minimized, supports animations.
    class Tab {
    public:
        /// @brief Create new instance of `Tab` with default title and draw callback
        Tab() : m_title("Tab"), m_isOpen(false) {}

        /// @brief Create new instance of `Tab` with set title and draw callback
        /// @param title Title of the Tab
        /// @param onDraw Callback which will be called when the Tab is drawn
        Tab(std::string title, Function<void()>&& onDraw);

        /// @brief Draw the Tab
        void draw();

        /// @brief Check whether the Tab is opened
        /// @return True if the Tab is opened
        [[nodiscard]] bool isOpen() const;

        /// @brief Set whether the Tab is opened
        /// @param open Whether the Tab should be opened
        void setOpen(bool open);

        [[nodiscard]] std::string const& getTitle() const;
        void setTitle(std::string title);

    private:
        Function<void()> m_drawCallback; // Callback which will be called when the Tab is drawn
        std::string m_title; // Tab title
        bool m_isOpen;       // Whether the Tab is collapsed or not
    };

}