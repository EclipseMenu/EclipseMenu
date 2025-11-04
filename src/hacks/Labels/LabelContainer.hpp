#pragma once

namespace eclipse::hacks::Labels {

    class SmartLabel;

    /// @brief Class that contains multiple labels and can manage them.
    class LabelsContainer : public cocos2d::CCNode {
    public:
        enum class Alignment {
            TopLeft = 0, TopCenter = 1, TopRight = 2,
            CenterLeft = 3, Center = 4, CenterRight = 5,
            BottomLeft = 6, BottomCenter = 7, BottomRight = 8
        };

        static LabelsContainer* create(Alignment alignment = Alignment::TopLeft) {
            auto ret = new LabelsContainer();
            if (ret->init(alignment)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        /// @brief Initialize the container with the specified alignment.
        bool init(Alignment alignment);

        /// @brief Add a label to the container.
        /// @param label The label to add.
        void addLabel(SmartLabel* label);

        /// @brief Remove a label from the container.
        /// @param label The label to remove.
        void removeLabel(SmartLabel* label);

        /// @brief Update all labels in the container.
        void update();

        /// @brief Invalidate the container. This will mark it as dirty and update it after update is finished.
        void invalidate() { m_dirty = true; }

        /// @brief Set the alignment of the container.
        /// @param alignment The alignment to set.
        void setAlignment(Alignment alignment) {
            m_alignment = alignment;
            updatePosition();
            this->updateLayout(false);
        }

        /// @brief Get the alignment of the container.
        /// @return The alignment of the container.
        [[nodiscard]] Alignment getAlignment() const { return m_alignment; }

        /// @brief Update the position of the container.
        void updatePosition();

    private:
        /// @brief Performs a layout update.
        void recalculateLayout();

        std::vector<SmartLabel*> m_labels;
        Alignment m_alignment = Alignment::TopLeft;
        bool m_dirty = false;
    };

}
