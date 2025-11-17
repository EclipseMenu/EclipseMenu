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

    enum class ComponentFlags : uint8_t {
        None = 0, // No flags (default)

        DisableCocos = 1 << 0, // Hide the component in cocos renderer mode

        DisableTabbed = 1 << 1, // Hide the component in tabbed layout (imgui)
        DisablePanel = 1 << 2, // Hide the component in panel layout (imgui)
        DisableSidebar = 1 << 3, // Hide the component in sidebar layout (imgui)
        StartWithKeyboardFocus = 1 << 4, // Starts the component with keyboard focus (imgui)

        SearchedFor = 1 << 5, // The component is being searched for

        OnlyTabbed = DisablePanel | DisableSidebar | DisableCocos, // Display exclusively in tabbed layout (imgui)
        OnlyPanel = DisableTabbed | DisableSidebar | DisableCocos, // Display exclusively in panel layout (imgui)
        OnlySidebar = DisableTabbed | DisablePanel | DisableCocos, // Display exclusively in sidebar layout (imgui)

        DisableImGui = DisableTabbed | DisablePanel | DisableSidebar, // Hide the component in imgui renderer mode
    };

    bool operator&(ComponentFlags lhs, ComponentFlags rhs);
    ComponentFlags operator|(ComponentFlags lhs, ComponentFlags rhs);

    class Component {
    public:
        virtual ~Component() = default;

        Component();

        Component(Component&&) = default;
        Component& operator=(Component&&) = default;

        Component(Component const&) = delete;
        Component& operator=(Component const&) = delete;

        /// @brief Get the component's unique identifier.
        [[nodiscard]] size_t getUID() const { return m_uid; }

        /// @brief Find a component by its unique identifier.
        static Component* find(size_t uid);

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
        Component* disableSaving();

        /// @brief Whether current component should use temporary storage
        [[nodiscard]] bool isSaveDisabled() const;

        /// @brief Set the component's description.
        virtual Component* setDescription(std::string description);

        /// @brief Get the component's flags
        [[nodiscard]] ComponentFlags getFlags() const;

        /// @brief Adds a flag to the component
        Component* addFlag(ComponentFlags flag);

        /// @brief Removes a flag from the component
        Component* removeFlag(ComponentFlags flag);

        /// @brief Set the component's flags
        Component* setFlags(ComponentFlags flags);

    protected:
        static size_t m_uniqueID;
        size_t m_uid;
        ComponentType m_type = ComponentType::Unknown;
        bool m_noSave = false;
        ComponentFlags m_flags = ComponentFlags::None;
        std::string m_description;
    };
}
