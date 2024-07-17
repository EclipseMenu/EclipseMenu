/*#include "SmartLabel.hpp"

namespace eclipse::hacks::Labels {

    void SmartLabel::setScript(std::string scriptText) {
        m_scriptText = std::move(scriptText);
        delete m_script;
        m_script = rift::compile(m_scriptText);
    }

    void SmartLabel::update() {
        if (m_script) {
            m_label.setText(m_script->run());
        }
    }

}*/