#include "Label.hpp"

#include <modules/labels/variables.hpp>
#include "LabelContainer.hpp"

namespace eclipse::hacks::Labels {

    bool SmartLabel::init(const std::string& text, const std::string& font) {
        if (!EmojiLabel::init("", font.c_str()))
            return false;

        auto res = rift::compile(text);
        if (res.isErr()) {
            m_error = res.unwrapErr().message();
            m_script = nullptr;
        } else {
            m_script = std::move(res.unwrap());
        }

        return true;
    }

    void SmartLabel::setScript(const std::string& script) {
        if (script == m_text) return;
        m_text = script;

        auto res = rift::compile(script);
        if (res.isErr()) {
            m_error = res.unwrapErr().message();
            m_script = nullptr;
        } else {
            m_script = std::move(res.unwrap());
        }
    }

    void SmartLabel::update() {
        if (!isVisible()) {
            if (m_wasVisible) {
                m_wasVisible = false;
                if (m_parentContainer) m_parentContainer->invalidate();
            }
            return;
        }

        if (!m_wasVisible) {
            m_wasVisible = true;
            if (m_parentContainer) m_parentContainer->invalidate();
        }

        // Re-evaluate the script
        if (m_script) {
            auto res = m_script->run(labels::VariableManager::get().getVariables());
            if (res.isOk()) {
                auto text = res.unwrap();
                this->setString(text);
            } else {
                this->setString(res.unwrapErr().message());
            }
        } else {
            this->setString(m_error);
        }

        auto size = this->getContentSize();
        size.height *= m_fScaleY;

        // If the height has changed, update the container
        if (size.height != m_lastHeight) {
            if (m_heightMultiplier != 1.0f) {
                size.height *= m_heightMultiplier;
                this->setContentSize({ size.width, size.height / m_fScaleY });
            }

            m_lastHeight = size.height;
            if (m_parentContainer) {
                m_parentContainer->invalidate();
            }
        }
    }
}
