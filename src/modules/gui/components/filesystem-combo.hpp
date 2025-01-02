#pragma once
#include <filesystem>
#include <functional>
#include <vector>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Combo component for selecting entries from a path.
    class FilesystemComboComponent : public Component {
    public:
        explicit FilesystemComboComponent(std::string id, std::string title, std::filesystem::path directory);

        void onInit() override;

        void onUpdate() override;

        /// @brief Set a callback function to be called when the component value changes.
        FilesystemComboComponent* callback(const std::function<void(int)>& func);

        /// @brief Get the combo value.
        [[nodiscard]] std::filesystem::path getValue() const;

        /// @brief Get the combo items.
        [[nodiscard]] const std::vector<std::filesystem::path>& getItems() const;

        /// @brief Set the combo value.
        void setValue(std::filesystem::path path) const;

        /// @brief Set the combo value.
        void setValue(int index) const;

        [[nodiscard]] const std::string& getId() const override;

        [[nodiscard]] const std::string& getTitle() const override;

        [[nodiscard]] std::string* getSearchBuffer();

        FilesystemComboComponent* setDescription(std::string description) override;

        void triggerCallback(int value) const;

    private:
        void globFiles();

    private:
        std::string m_id;
        std::string m_title;
        std::filesystem::path m_directory;
        std::vector<std::filesystem::path> m_items;
        std::function<void(int)> m_callback;

        std::string m_searchBuffer;
    };
}
