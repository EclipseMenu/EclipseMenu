#pragma once
#include <Geode/Geode.hpp>
#include <modules/gui/color.hpp>
#include <modules/gui/cocos/nodes/FallbackBMFont.hpp>
#include <rift.hpp>
#include <modules/labels/setting.hpp>

namespace eclipse::hacks::Labels {

    class LabelsContainer;

    /// @brief Label with RIFT scripting support.
    class SmartLabel : public gui::cocos::EmojiLabel {
    public:
        static SmartLabel* create(std::string_view text, std::string const& font) {
            auto ret = new SmartLabel();
            if (ret->init(text, font)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        /// @brief Initialize the label with the specified text and font.
        bool init(std::string_view text, std::string const& font);

        /// @brief Set the parent container of the label.
        void setParentContainer(LabelsContainer* container) { m_parentContainer = container; }

        /// @brief Set the height multiplier.
        void setHeightMultiplier(float multiplier) { m_heightMultiplier = multiplier; }

        /// @brief Set the custom position of the label.
        void setCustomPosition(const cocos2d::CCPoint& position) { m_customPosition = position; }

        /// @brief Set the settings associated with the label.
        void setSettings(labels::LabelSettings* settings) { m_settings = settings; }

        /// @brief Get the custom position of the label.
        [[nodiscard]] cocos2d::CCPoint getCustomPosition() const { return m_customPosition; }

        /// @brief Update the label.
        void update();

    private:
        cocos2d::CCPoint m_customPosition = {0, 0};
        float m_heightMultiplier = 1.0f;
        std::unique_ptr<rift::Script> m_script = nullptr;
        std::string m_error;
        labels::LabelSettings const* m_settings = nullptr;

        // Used to check if the label should call updateLayout on the parent container
        float m_lastHeight = 0.f;
        bool m_wasVisible = false;
        LabelsContainer* m_parentContainer = nullptr;
    };

}
