/*#pragma once
#include "Label.hpp"

#include <rift.hpp>
#include <string>
#include <utility>

namespace eclipse::hacks::Labels {

    /// @brief Class that controls a Label object and has access to RIFT scripting.
    class SmartLabel {
    public:
        SmartLabel(std::string text, std::string font)
            : m_label(std::move(text), std::move(font)) {}

        /// @brief Get the label
        /// @return The label
        [[nodiscard]] Label& getLabel() { return m_label; }

        /// @brief Set the script for the label.
        /// @param script The script to set.
        void setScript(std::string scriptText);

        /// @brief Reruns the script and updates the label.
        void update();

    private:
        Label m_label;
        rift::Script* m_script = nullptr;
        std::string m_scriptText;
    };


}*/