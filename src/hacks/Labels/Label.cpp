#include "Label.hpp"

#include <modules/config/config.hpp>
#include <modules/labels/variables.hpp>
#include "LabelContainer.hpp"

namespace eclipse::hacks::Labels {

    bool SmartLabel::init(std::string_view text, std::string const& font) {
        if (!EmojiLabel::init("", font))
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

    static void updateCheatIndicator(SmartLabel* label) {
        bool visible = config::get<"labels.cheat-indicator.visible">(false);

        if (!visible) {
            label->setVisible(false);
            return;
        }

        bool isCheating = config::getTemp<"hasCheats">(false);
        bool hasTripped = config::getTemp<"trippedSafeMode">(false);
        bool showOnlyCheating = config::get<"labels.cheat-indicator.only-cheating">(false);

        if (showOnlyCheating && !(isCheating || hasTripped)) {
            label->setVisible(false);
            return;
        }

        label->setVisible(true);
        label->setScale(config::get<"labels.cheat-indicator.scale", float>(0.5f));
        label->setOpacity(
            static_cast<GLubyte>(config::get<"labels.cheat-indicator.opacity", float>(0.35f) * 255)
        );

        // Cheating - Red, Tripped - Orange, Normal - Green
        auto color = isCheating ? gui::Color::RED : hasTripped
                                ? gui::Color{0.72f, 0.37f, 0.f}
                                : gui::Color::GREEN;
        label->setColor(color.toCCColor3B());
    }

    void SmartLabel::update() {
        if (m_settings) {
            if (m_settings->hasEvents()) {
                auto [visible, scale, color, font] = m_settings->processEvents();
                this->setFont(font);
                this->setScale(scale);
                this->setColor(color.toCCColor3B());
                this->setOpacity(color.getAlphaByte());
                this->setVisible(visible);
            }
        } else {
            updateCheatIndicator(this);
        }

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
