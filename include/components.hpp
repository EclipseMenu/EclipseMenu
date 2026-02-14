#pragma once
#ifndef ECLIPSE_COMPONENTS_HPP
#define ECLIPSE_COMPONENTS_HPP

#include "events.hpp"

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
        void setDescriptionImpl(std::string description) const;
        size_t m_uniqueID = 0;
    };

    class Label final : public Component<ComponentType::Label> {
    public:
        explicit Label(size_t uid) : Component(uid) {}
        Label& setText(std::string text);
        Label const& setText(std::string text) const;
    };

    class Toggle final : public Component<ComponentType::Toggle> {
    public:
        Toggle(size_t uid, std::string id) : Component(uid), m_id(std::move(id)) {}
        std::string const& getID() const { return m_id; }
        Toggle& setDescription(std::string description) { setDescriptionImpl(std::move(description)); return *this; }

        bool getValue() const;

    private:
        std::string m_id;
    };

    class Button final : public Component<ComponentType::Button> {
    public:
        explicit Button(size_t uid) : Component(uid) {}
        Button& setDescription(std::string description) { setDescriptionImpl(std::move(description)); return *this; }
    };

    class InputFloat final : public Component<ComponentType::InputFloat> {
    public:
        InputFloat(size_t uid, std::string id) : Component(uid), m_id(std::move(id)) {}
        std::string const& getID() const { return m_id; }
        InputFloat& setDescription(std::string description) { setDescriptionImpl(std::move(description)); return *this; }

        float getValue() const;

        InputFloat& setMinValue(float value);
        InputFloat& setMaxValue(float value);
        InputFloat& setFormat(std::string format);

    private:
        std::string m_id;
    };
}

namespace eclipse {
    class MenuTab final {
    public:
        static MenuTab find(std::string name);

        components::Label addLabel(std::string title) const;
        components::Toggle addToggle(std::string id, std::string title, geode::Function<void(bool)> callback) const;
        components::Toggle addModSettingToggle(std::shared_ptr<geode::Setting> const& setting) const;
        components::Button addButton(std::string title, geode::Function<void()> callback) const;
        components::InputFloat addInputFloat(std::string id, std::string title, geode::Function<void(float)> callback) const;

        std::string const& getName() const { return m_name; }

    private:
        explicit MenuTab(std::string name) : m_name(std::move(name)) {}
        std::string m_name;
    };
}

#ifndef ECLIPSE_DONT_DEFINE_IMPLS

namespace eclipse {
    /// @brief Get a menu tab handle by name. Creates the tab if it doesn't exist.
    /// @param name The name of the tab.
    inline MenuTab MenuTab::find(std::string name) {
        auto& vtable = __internal__::getVTable();
        if (vtable.CreateMenuTab) {
            vtable.CreateMenuTab(name);
        }
        return MenuTab(std::move(name));
    }

    /// @brief Add a label to the tab.
    /// @param title The title of the label.
    inline components::Label MenuTab::addLabel(std::string title) const {
        auto& vtable = __internal__::getVTable();
        if (vtable.CreateLabel) {
            size_t id = vtable.CreateLabel(m_name, std::move(title));
            return components::Label(id);
        }
        return components::Label(0);
    }

    /// @brief Add a toggle to the tab.
    /// @param id The ID of the toggle.
    /// @param title The title of the toggle.
    /// @param callback The callback function to call when the toggle is toggled.
    inline components::Toggle MenuTab::addToggle(std::string id, std::string title, geode::Function<void(bool)> callback) const {
        auto& vtable = __internal__::getVTable();
        if (vtable.CreateToggle) {
            size_t idx = vtable.CreateToggle(m_name, id, std::move(title), std::move(callback));
            return components::Toggle(idx, std::move(id));
        }
        return components::Toggle(0, std::move(id));
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
        auto toggle = addToggle(mod->expandSpriteName(settingId), std::move(title), [mod, settingId](bool v) {
            mod->setSettingValue(settingId, v);
        }).setDescription(std::move(description));

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
    inline components::Button MenuTab::addButton(std::string title, geode::Function<void()> callback) const {
        auto& vtable = __internal__::getVTable();
        if (vtable.CreateButton) {
            size_t id = vtable.CreateButton(m_name, std::move(title), std::move(callback));
            return components::Button(id);
        }
        return components::Button(0);
    }

    /// @brief Add an input float to the tab.
    /// @param id The ID of the input float.
    /// @param title The title of the input float.
    /// @param callback The callback function to call when the input float is changed.
    inline components::InputFloat MenuTab::addInputFloat(std::string id, std::string title, geode::Function<void(float)> callback) const {
        auto& vtable = __internal__::getVTable();
        if (vtable.CreateInputFloat) {
            size_t idx = vtable.CreateInputFloat(m_name, id, std::move(title), std::move(callback));
            return components::InputFloat(idx, std::move(id));
        }
        return components::InputFloat(0, std::move(id));
    }

    namespace components {
        /// @brief Set the description of the component.
        /// @param description The description to set.
        template <ComponentType T>
        void Component<T>::setDescriptionImpl(std::string description) const {
            auto& vtable = __internal__::getVTable();
            if (vtable.SetComponentDescription) {
                vtable.SetComponentDescription(getUniqueID(), std::move(description));
            }
        }

        /// @brief Set the text of the label.
        /// @param text The text to set.
        inline Label& Label::setText(std::string text) {
            auto& vtable = __internal__::getVTable();
            if (vtable.SetLabelText) {
                vtable.SetLabelText(getUniqueID(), std::move(text));
            }
            return *this;
        }

        /// @brief Set the text of the label.
        /// @param text The text to set.
        inline Label const& Label::setText(std::string text) const {
            auto& vtable = __internal__::getVTable();
            if (vtable.SetLabelText) {
                vtable.SetLabelText(getUniqueID(), std::move(text));
            }
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
            auto& vtable = __internal__::getVTable();
            if (vtable.SetInputFloatParams) {
                vtable.SetInputFloatParams(getUniqueID(), value, std::nullopt, std::nullopt);
            }
            return *this;
        }

        /// @brief Set the maximum allowed value of the input float.
        /// @param value The value to set.
        inline InputFloat& InputFloat::setMaxValue(float value) {
            auto& vtable = __internal__::getVTable();
            if (vtable.SetInputFloatParams) {
                vtable.SetInputFloatParams(getUniqueID(), std::nullopt, value, std::nullopt);
            }
            return *this;
        }

        /// @brief Set the format string of the input float.
        /// @param format The format string to set. (e.g. "%.2f")
        inline InputFloat& InputFloat::setFormat(std::string format) {
            auto& vtable = __internal__::getVTable();
            if (vtable.SetInputFloatParams) {
                vtable.SetInputFloatParams(getUniqueID(), std::nullopt, std::nullopt, std::move(format));
            }
            return *this;
        }
    }
}

#endif // ECLIPSE_DONT_DEFINE_IMPLS

#endif // ECLIPSE_COMPONENTS_HPP