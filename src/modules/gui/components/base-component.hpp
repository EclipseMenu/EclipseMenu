#pragma once
#include <memory>
#include <string>

namespace eclipse::gui {
    enum class ComponentType {
        Unknown = -1,
        Label, Toggle, RadioButton,
        Combo, Slider, InputFloat, InputInt,
        FloatToggle, InputText, Color,
        Button, Keybind, LabelSettings, FilesystemCombo, IntToggle
    };

    class Component {
    public:
        virtual ~Component() = default;

        Component();

        /// @brief Get the component's unique identifier.
        [[nodiscard]] size_t getUID() const { return m_uid; }

        /// @brief Find a component by its unique identifier.
        static std::shared_ptr<Component> find(size_t uid);

        /// @brief Initialize the component.
        virtual void onInit() = 0;

        /// @brief Update the component.
        virtual void onUpdate() = 0;

        /// @brief Get the component's ID. (unique identifier)
        [[nodiscard]] virtual const std::string& getId() const = 0;

        /// @brief Get the component's title. (used for sorting in the menu)
        [[nodiscard]] virtual const std::string& getTitle() const;

        /// @brief Get the component's description.
        [[nodiscard]] const std::string& getDescription() const;

        /// @brief Get the component's type (used to skip dynamic casting)
        [[nodiscard]] ComponentType getType() const;

        /// @brief Excludes value from being saved into main configuration file
        /// Useful for some internal values. (Uses temporary storage).
        void disableSaving();

        /// @brief Whether current component should use temporary storage
        [[nodiscard]] bool isSaveDisabled() const;

        /// @brief Set the component's description.
        virtual Component* setDescription(std::string description);

        /// @brief Whether the component is being searched for or not
        bool isSearchedFor() const;;

        /// @brief Sets the component's search state
        void setSearchedFor(bool state);;

    protected:
        static size_t m_uniqueID;
        size_t m_uid;
        ComponentType m_type = ComponentType::Unknown;
        bool m_noSave = false;
        bool m_isSearchedFor = false;
        std::string m_description;
    };
}
