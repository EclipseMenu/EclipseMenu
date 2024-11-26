#pragma once
#ifndef ECLIPSE_COMPONENTS_HPP
#define ECLIPSE_COMPONENTS_HPP

#include <functional>
#include <string>
#include <utility>
#include <Geode/loader/Event.hpp>

namespace eclipse::components {
    enum class ComponentType {
        Label, Toggle
    };

    template <ComponentType T>
    class Component {
    public:
        constexpr ComponentType getType() const { return T; }
        size_t getUniqueID() const { return m_uniqueID; }
        explicit Component(size_t id) : m_uniqueID(id) {}
    private:
        size_t m_uniqueID = 0;
    };

    using Label = Component<ComponentType::Label>;
    class Toggle final : public Component<ComponentType::Toggle> {
    public:
        Toggle(size_t uid, std::string id) : Component(uid), m_id(std::move(id)) {}
        const std::string& getID() const { return m_id; }
        Toggle& setDescription(const std::string& description);
        // Toggle& addOptions(const std::function<void()>& options);
    private:
        std::string m_id;
    };
    class Button final : public Component<ComponentType::Label> {
    public:
        explicit Button(size_t uid) : Component(uid) {}
        Button& setDescription(const std::string& description);
        // Button& setText(const std::string& text);
    };

}

namespace eclipse {
    class MenuTab final {
    public:
        static MenuTab find(const std::string& name);

        components::Label addLabel(const std::string& title) const;
        components::Toggle addToggle(const std::string& id, const std::string& title, const std::function<void(bool)> &callback) const;
        components::Button addButton(const std::string& title, const std::function<void()>& callback) const;

        const std::string& getName() const { return m_name; }
    private:
        explicit MenuTab(std::string name) : m_name(std::move(name)) {}
        std::string m_name;
    };
}

namespace eclipse::events {
    class CreateMenuTabEvent final : public geode::Event {
    public:
        explicit CreateMenuTabEvent(std::string name) : m_name(std::move(name)) {}
        const std::string& getName() const { return m_name; }
    private:
        std::string m_name;
    };

    template <typename... Callback>
    class AddComponentEvent : public geode::Event {
    public:
        AddComponentEvent(const MenuTab* tab, std::string id, std::string title, Callback... callbacks)
            : m_tabName(tab->getName()), m_id(std::move(id)), m_title(std::move(title)), m_callbacks(std::move(callbacks)...) {}

        const std::string& getTabName() const { return m_tabName; }
        const std::string& getID() const { return m_id; }
        const std::string& getTitle() const { return m_title; }
        const auto& getCallbacks() const { return m_callbacks; }

        size_t getUniqueID() const { return m_uniqueID; }
        void setUniqueID(size_t id) { m_uniqueID = id; }

    private:
        std::string m_tabName;
        std::string m_id;
        std::string m_title;
        std::tuple<Callback...> m_callbacks;
        size_t m_uniqueID = 0;
    };

    using AddLabelEvent = AddComponentEvent<>;
    using AddToggleEvent = AddComponentEvent<std::function<void(bool)>>;
    using AddButtonEvent = AddComponentEvent<std::function<void()>>;

    class SetComponentDescriptionEvent : public geode::Event {
    public:
        SetComponentDescriptionEvent(size_t id, std::string description)
            : m_id(id), m_description(std::move(description)) {}
        size_t getID() const { return m_id; }
        const std::string& getDescription() const { return m_description; }
    private:
        size_t m_id;
        std::string m_description;
    };
}

namespace eclipse {
    /// @brief Get a menu tab handle by name. Creates the tab if it doesn't exist.
    /// @param name The name of the tab.
    inline MenuTab MenuTab::find(const std::string& name) {
        events::CreateMenuTabEvent(name).post();
        return MenuTab(name);
    }

    /// @brief Add a label to the tab.
    /// @param title The title of the label.
    inline components::Label MenuTab::addLabel(const std::string& title) const {
        events::AddLabelEvent event(this, "", title);
        event.post();
        return components::Label(event.getUniqueID());
    }

    /// @brief Add a toggle to the tab.
    /// @param id The ID of the toggle.
    /// @param title The title of the toggle.
    /// @param callback The callback function to call when the toggle is toggled.
    inline components::Toggle MenuTab::addToggle(const std::string& id, const std::string& title, const std::function<void(bool)>& callback) const {
        events::AddToggleEvent event(this, id, title, callback);
        event.post();
        return components::Toggle(event.getUniqueID(), id);
    }

    /// @brief Add a button to the tab.
    /// @param title The title of the button.
    /// @param callback The callback function to call when the button is pressed.
    inline components::Button MenuTab::addButton(const std::string& title, const std::function<void()>& callback) const {
        events::AddButtonEvent event(this, "", title, callback);
        event.post();
        return components::Button(event.getUniqueID());
    }

    namespace components {
        /// @brief Set the description of the toggle.
        /// @param description The description to set.
        inline Toggle& Toggle::setDescription(const std::string& description) {
            events::SetComponentDescriptionEvent(getUniqueID(), description).post();
            return *this;
        }

        /// @brief Set the description of the button.
        /// @param description The description to set.
        inline Button& Button::setDescription(const std::string& description) {
            events::SetComponentDescriptionEvent(getUniqueID(), description).post();
            return *this;
        }

    }
}

#endif // ECLIPSE_COMPONENTS_HPP