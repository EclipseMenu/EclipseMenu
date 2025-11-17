#pragma once
#include <functional>
#include "base-component.hpp"

namespace eclipse::labels {
    struct LabelSettings;
}

namespace eclipse::gui {
    /// @brief Component that allows to change label settings.
    class LabelSettingsComponent : public Component {
    public:
        explicit LabelSettingsComponent(labels::LabelSettings* settings);

        void onUpdate() override {}

        [[nodiscard]] std::string const& getId() const override;
        [[nodiscard]] std::string const& getTitle() const override;

        labels::LabelSettings* getSettings() const;

        LabelSettingsComponent* deleteCallback(Function<void()>&& func);
        LabelSettingsComponent* editCallback(Function<void()>&& func);
        LabelSettingsComponent* moveCallback(Function<void(bool)>&& func);
        LabelSettingsComponent* exportCallback(Function<void()>&& func);

        void triggerDeleteCallback();
        void triggerEditCallback();
        void triggerMoveCallback(bool up);
        void triggerExportCallback();

        /// @brief Allows to set keybinds for the label.
        LabelSettingsComponent* handleKeybinds();

        [[nodiscard]] bool hasKeybind() const;

    private:
        std::string m_id;
        labels::LabelSettings* m_settings;
        Function<void()> m_deleteCallback;
        Function<void()> m_editCallback;
        Function<void(bool)> m_moveCallback;
        Function<void()> m_exportCallback;

        bool m_hasKeybind = false;
    };
}
