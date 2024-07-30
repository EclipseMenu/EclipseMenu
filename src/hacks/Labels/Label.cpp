#include "Label.hpp"

#include <modules/labels/variables.hpp>

namespace eclipse::hacks::Labels {

    bool SmartLabel::init(const std::string& text, const std::string& font) {
        if (!CCLabelBMFont::initWithString("", font.c_str()))
            return false;

        const auto& contentSize = this->getContentSize();
        m_contentSize = cocos2d::CCSize{ contentSize.width, contentSize.height * m_heightMultiplier };

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
        if (!isVisible()) {
            m_contentSize = cocos2d::CCSize{0, 0};

            return this->setContentSize({0, 0});
        }

        // Re-evaluate the script
        if (m_script) {
            auto text = m_script->run(labels::VariableManager::get().getVariables());
            this->setString(text.c_str());
        } else {
            this->setString(m_error.c_str());
        }

        // Update the content size if needed
        if (m_heightMultiplier != 1.0f)
            this->setContentSize(this->getContentSize());

        const auto& contentSize = this->getContentSize();
        m_contentSize = cocos2d::CCSize{ contentSize.width, contentSize.height * m_heightMultiplier };
    }

}