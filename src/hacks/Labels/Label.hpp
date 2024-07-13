#pragma once
#include <Geode/Geode.hpp>
#include <modules/gui/color.hpp>

namespace eclipse::hacks::Labels {

    /// @brief Class that represents a CCLabelBMFont object with additional functionality.
    class Label {
    public:
        /// @brief Constructor.
        /// @param text The text to display.
        /// @param font The font to use.
        Label(std::string text, std::string font);

        /// @brief Creates a new CCLabelBMFont object and adds it to the specified layer.
        /// @param layer The layer to add the label to.
        void addToLayer(cocos2d::CCLayer* layer);

        /// @brief Removes the label from the layer and deletes it.
        void removeFromLayer();

        /// @brief Set the label's node ID.
        void setID(const std::string& id) { m_id = id; }
        [[nodiscard]] const std::string& getID() const { return m_id; }

        /// @brief Get the label's height.
        [[nodiscard]] float getHeight() const;

        // Getters and setters
        // update flag is used to determine whether to call update() after setting the value
        // (useful when setting multiple properties at once)

        [[nodiscard]] const std::string& getText() const { return m_text; }
        [[nodiscard]] const std::string& getFont() const { return m_font; }
        [[nodiscard]] const gui::Color& getColor() const { return m_color; }
        [[nodiscard]] const cocos2d::CCPoint& getAnchor() const { return m_anchor; }
        [[nodiscard]] const cocos2d::CCPoint& getPosition() const { return m_position; }
        [[nodiscard]] float getScale() const { return m_scale; }
        [[nodiscard]] bool isVisible() const { return m_visible; }

        void setText(const std::string& text, bool update = true) { m_text = text; if (update) this->update(); }
        void setFont(const std::string& font, bool update = true) { m_font = font; if (update) this->update(); }
        void setColor(const gui::Color& color, bool update = true) { m_color = color; if (update) this->update(); }
        void setAnchor(const cocos2d::CCPoint& anchor, bool update = true) { m_anchor = anchor; if (update) this->update(); }
        void setPosition(const cocos2d::CCPoint& position, bool update = true) { m_position = position; if (update) this->update(); }
        void setScale(float scale, bool update = true) { m_scale = scale; if (update) this->update(); }
        void setVisible(bool visible, bool update = true) { m_visible = visible; if (update) this->update(); }

        /// @brief Set the height multiplier.
        void setHeightMultiplier(float multiplier) { m_heightMultiplier = multiplier; }

    private:
        cocos2d::CCLabelBMFont* m_label = nullptr;
        cocos2d::CCLayer* m_layer = nullptr;

        bool m_visible = true;
        cocos2d::CCPoint m_position = {0, 0};
        cocos2d::CCPoint m_anchor = {0, 0};
        float m_scale = 1.0f;
        gui::Color m_color = gui::Color::WHITE;

        std::string m_text;
        std::string m_font;
        std::string m_id;

        float m_heightMultiplier = 1.0f;

        void update();
    };

}