#include "Label.hpp"

#include <modules/labels/variables.hpp>
#include "LabelContainer.hpp"

namespace eclipse::hacks::Labels {

    bool SmartLabel::init(const std::string& text, const std::string& font) {
        if (!CCLabelBMFont::initWithString("", font.c_str()))
            return false;

        auto res = rift::compile(text);
        if (res)
            m_script = res.getValue();
        else
            m_error = res.getMessage();

        return true;
    }

    void SmartLabel::setScript(const std::string& script) {
        if (script == m_text) return;

        delete m_script;
        m_script = nullptr;
        m_text = script;

        auto res = rift::compile(script);
        if (res)
            m_script = res.getValue();
        else
            m_error = res.getMessage();
    }

    void SmartLabel::update() {
        if (!isVisible()) return this->setContentSize({0, 0});

        // Re-evaluate the script
        if (m_script) {
            auto text = m_script->run(labels::VariableManager::get().getVariables());
            this->setString(text.c_str());

            // whether to update the parent container
            bool empty = text.empty();
            if (empty != m_wasEmpty && m_parentContainer) {
                m_parentContainer->updateLayout(false);
                m_wasEmpty = empty;
            }
        } else {
            this->setString(m_error.c_str());
        }

        // Update the content size if needed
        if (m_heightMultiplier != 1.0f)
            this->setContentSize({ getContentWidth(), getContentHeight() * m_heightMultiplier });
    }

}