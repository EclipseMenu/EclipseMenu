#pragma once

#include "Label.hpp"

namespace eclipse::hacks::Labels {

    /// @brief Class that contains multiple labels and can manage them.
    class LabelsContainer {
    public:
        enum class Alignment {
            TopLeft, TopCenter, TopRight,
            BottomLeft, BottomCenter, BottomRight
        };

        LabelsContainer() = default;
        explicit LabelsContainer(Alignment alignment) : m_alignment(alignment) {}

        /// @brief Add a label to the container.
        /// @param label The label to add.
        /// @param update Custom update function.
        void addLabel(Label* label, const std::function<void(Label*)>& update);

        /// @brief Remove a label from the container.
        /// @param label The label to remove.
        void removeLabel(Label* label);

        /// @brief Update all labels in the container.
        void update();

        /// @brief Set the alignment of the container.
        /// @param alignment The alignment to set.
        void setAlignment(Alignment alignment) { m_alignment = alignment; }

        /// @brief Get the alignment of the container.
        /// @return The alignment of the container.
        [[nodiscard]] Alignment getAlignment() const { return m_alignment; }

        /// @brief Set the visibility of the container.
        /// @param visible The visibility to set.
        void setVisible(bool visible) { m_visible = visible; }

        /// @brief Get the visibility of the container.
        /// @return The visibility of the container.
        [[nodiscard]] bool isVisible() const { return m_visible; }

        /// @brief Recalculate the positions of the labels.
        void recalculatePositions();

        /// @brief Reconstructs all labels in the container. Should be called once after PlayLayer::init
        void recreateLabels();

        /// @brief Deletes all labels in the container. Should be called once before PlayLayer::init
        void clear();

        /// @brief Same as clear, but also removes the labels from the scene (used for hot reload)
        void deleteFromScene();

    private:
        std::vector<std::pair<Label*, std::function<void(Label*)>>> m_labels;
        Alignment m_alignment = Alignment::TopLeft;
        bool m_visible = true;
    };

}