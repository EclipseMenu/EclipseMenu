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

        void onInit() override {}
        void onUpdate() override {}

        [[nodiscard]] std::string const& getId() const override;
        [[nodiscard]] std::string const& getTitle() const override;

        labels::LabelSettings* getSettings() const;

        LabelSettingsComponent* deleteCallback(std::function<void()>&& func);
        LabelSettingsComponent* editCallback(std::function<void()>&& func);
        LabelSettingsComponent* moveCallback(std::function<void(bool)>&& func);
        LabelSettingsComponent* exportCallback(std::function<void()>&& func);

        void triggerDeleteCallback() const;
        void triggerEditCallback() const;
        void triggerMoveCallback(bool up) const;
        void triggerExportCallback() const;

        /// @brief Allows to set keybinds for the label.
        LabelSettingsComponent* handleKeybinds();

        [[nodiscard]] bool hasKeybind() const;

    private:
        std::string m_id;
        labels::LabelSettings* m_settings;
        std::function<void()> m_deleteCallback;
        std::function<void()> m_editCallback;
        std::function<void(bool)> m_moveCallback;
        std::function<void()> m_exportCallback;

        bool m_hasKeybind = false;
    };
}
