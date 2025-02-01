#pragma once
#ifndef ECLIPSE_COMPONENTS_HPP
#define ECLIPSE_COMPONENTS_HPP

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <Geode/loader/Event.hpp>
#include <Geode/loader/Setting.hpp>

#include "config.hpp"

namespace eclipse::components {
    enum class ComponentType {
        Label, Toggle, Button, InputFloat
    };

    template <ComponentType T>
    class Component {
    public:
        constexpr ComponentType getType() const { return T; }
        size_t getUniqueID() const { return m_uniqueID; }
        explicit Component(size_t id) : m_uniqueID(id) {}
    protected:
        void setDescriptionImpl(const std::string& description) const;
        size_t m_uniqueID = 0;
    };

    class Label final : public Component<ComponentType::Label> {
    public:
        explicit Label(size_t uid) : Component(uid) {}
        Label& setText(const std::string& text);
        Label const& setText(const std::string& text) const;
    };

    class Toggle final : public Component<ComponentType::Toggle> {
    public:
        Toggle(size_t uid, std::string id) : Component(uid), m_id(std::move(id)) {}
        const std::string& getID() const { return m_id; }
        Toggle& setDescription(const std::string& description) { setDescriptionImpl(description); return *this; }

        bool getValue() const;
    private:
        std::string m_id;
    };

    class Button final : public Component<ComponentType::Button> {
    public:
        explicit Button(size_t uid) : Component(uid) {}
        Button& setDescription(const std::string& description) { setDescriptionImpl(description); return *this; }
    };

    class InputFloat final : public Component<ComponentType::InputFloat> {
    public:
        InputFloat(size_t uid, std::string id) : Component(uid), m_id(std::move(id)) {}
        const std::string& getID() const { return m_id; }
        InputFloat& setDescription(const std::string& description) { setDescriptionImpl(description); return *this; }

        float getValue() const;

        InputFloat& setMinValue(float value);
        InputFloat& setMaxValue(float value);
        InputFloat& setFormat(const std::string& format);
    private:
        std::string m_id;
    };
}

namespace eclipse {
    class MenuTab final {
    public:
        static MenuTab find(const std::string& name);

        components::Label addLabel(const std::string& title) const;
        components::Toggle addToggle(const std::string& id, const std::string& title, const std::function<void(bool)> &callback) const;
        components::Toggle addModSettingToggle(std::shared_ptr<geode::Setting> const& setting) const;
        components::Button addButton(const std::string& title, const std::function<void()>& callback) const;
        components::InputFloat addInputFloat(const std::string& id, const std::string& title, const std::function<void(float)>& callback) const;

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
    using AddInputFloatEvent = AddComponentEvent<std::function<void(float)>>;

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

    class SetLabelTextEvent : public geode::Event {
    public:
        SetLabelTextEvent(size_t id, std::string text)
            : m_id(id), m_text(std::move(text)) {}
        size_t getID() const { return m_id; }
        const std::string& getText() const { return m_text; }
    private:
        size_t m_id;
        std::string m_text;
    };

    class SetInputFloatParamsEvent : public geode::Event {
    public:
        SetInputFloatParamsEvent(size_t id, std::optional<float> min, std::optional<float> max, std::optional<std::string> format)
            : m_id(id), m_min(min), m_max(max), m_format(std::move(format)) {}
        size_t getID() const { return m_id; }
        std::optional<float> getMin() const { return m_min; }
        std::optional<float> getMax() const { return m_max; }
        const std::optional<std::string>& getFormat() const { return m_format; }
    private:
        size_t m_id;
        std::optional<float> m_min;
        std::optional<float> m_max;
        std::optional<std::string> m_format;
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

    /// @brief A shorthand for adding a mod setting toggle to the tab.
    /// @param setting The setting to add.
    inline components::Toggle MenuTab::addModSettingToggle(std::shared_ptr<geode::Setting> const& setting) const {
        auto mod = setting->getMod();
        auto settingId = setting->getKey();
        auto title = setting->getDisplayName();
        auto description = setting->getDescription().value_or("");
        auto value = mod->getSettingValue<bool>(settingId);

        // Add the toggle
        auto toggle = addToggle(mod->expandSpriteName(settingId).data(), title, [mod, settingId](bool v) {
            mod->setSettingValue(settingId, v);
        }).setDescription(description);

        // Set the initial value
        config::set(toggle.getID(), value);

        // Listen for changes
        geode::listenForSettingChanges<bool>(settingId, [toggle](bool v) {
            config::set(toggle.getID(), v);
        }, mod);

        return toggle;
    }

    /// @brief Add a button to the tab.
    /// @param title The title of the button.
    /// @param callback The callback function to call when the button is pressed.
    inline components::Button MenuTab::addButton(const std::string& title, const std::function<void()>& callback) const {
        events::AddButtonEvent event(this, "", title, callback);
        event.post();
        return components::Button(event.getUniqueID());
    }

    /// @brief Add an input float to the tab.
    /// @param id The ID of the input float.
    /// @param title The title of the input float.
    /// @param callback The callback function to call when the input float is changed.
    inline components::InputFloat MenuTab::addInputFloat(const std::string& id, const std::string& title, const std::function<void(float)>& callback) const {
        events::AddInputFloatEvent event(this, id, title, callback);
        event.post();
        return components::InputFloat(event.getUniqueID(), id);
    }

    namespace components {
        /// @brief Set the description of the component.
        /// @param description The description to set.
        template <ComponentType T>
        void Component<T>::setDescriptionImpl(const std::string& description) const {
            events::SetComponentDescriptionEvent(getUniqueID(), description).post();
        }

        /// @brief Set the text of the label.
        /// @param text The text to set.
        inline Label& Label::setText(const std::string& text) {
            events::SetLabelTextEvent(getUniqueID(), text).post();
            return *this;
        }

        /// @brief Set the text of the label.
        /// @param text The text to set.
        inline Label const& Label::setText(const std::string& text) const {
            events::SetLabelTextEvent(getUniqueID(), text).post();
            return *this;
        }

        /// @brief Get the value of the toggle.
        inline bool Toggle::getValue() const {
            return config::get<bool>(m_id, false);
        }

        /// @brief Get the value of the input float.
        inline float InputFloat::getValue() const {
            return config::get<float>(m_id, 0.f);
        }

        /// @brief Set the minimum allowed value of the input float.
        /// @param value The value to set.
        inline InputFloat& InputFloat::setMinValue(float value) {
            events::SetInputFloatParamsEvent(getUniqueID(), value, std::nullopt, std::nullopt).post();
            return *this;
        }

        /// @brief Set the maximum allowed value of the input float.
        /// @param value The value to set.
        inline InputFloat& InputFloat::setMaxValue(float value) {
            events::SetInputFloatParamsEvent(getUniqueID(), std::nullopt, value, std::nullopt).post();
            return *this;
        }

        /// @brief Set the format string of the input float.
        /// @param format The format string to set. (e.g. "%.2f")
        inline InputFloat& InputFloat::setFormat(const std::string& format) {
            events::SetInputFloatParamsEvent(getUniqueID(), std::nullopt, std::nullopt, format).post();
            return *this;
        }
    }
}

#endif // ECLIPSE_COMPONENTS_HPP