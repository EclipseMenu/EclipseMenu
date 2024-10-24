#pragma once

namespace eclipse {

    namespace components {
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
            Toggle& addOptions(const std::function<void()>& options);
        private:
            std::string m_id;
        };
    }

    class MenuTab final {
    public:
        static MenuTab find(const std::string& name);

        components::Label addLabel(const std::string& title) const;
        components::Toggle addToggle(const std::string& id, const std::string& title, const std::function<void(bool)> &callback) const;

        const std::string& getName() const { return m_name; }
    private:
        explicit MenuTab(std::string name) : m_name(std::move(name)) {}
        std::string m_name;
    };

    namespace events {
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

        template <typename T>
        class RequestConfigValueEvent : public geode::Event {
        public:
            explicit RequestConfigValueEvent(std::string key, bool internal = false)
                : m_key(std::move(key)), m_useInternal(internal) {}
            const std::string& getKey() const { return m_key; }
            T getValue() const { return m_value; }
            bool hasValue() const { return m_hasValue; }
            bool getUseInternal() const { return m_useInternal; }

            void setValue(T value) {
                m_value = value;
                m_hasValue = true;
            }

        private:
            std::string m_key;
            T m_value;
            bool m_useInternal = false;
            bool m_hasValue = false;
        };

        using RequestBoolConfigValueEvent = RequestConfigValueEvent<bool>;
        using RequestIntConfigValueEvent = RequestConfigValueEvent<int>;
        using RequestStringConfigValueEvent = RequestConfigValueEvent<std::string>;
        using RequestFloatConfigValueEvent = RequestConfigValueEvent<float>;
    }

    inline components::Toggle& components::Toggle::setDescription(const std::string& description) {
        events::SetComponentDescriptionEvent(getUniqueID(), description).post();
        return *this;
    }

    inline MenuTab MenuTab::find(const std::string& name) {
        events::CreateMenuTabEvent(name).post();
        return MenuTab(name);
    }

    inline components::Label MenuTab::addLabel(const std::string& title) const {
        events::AddLabelEvent event(this, "", title);
        event.post();
        return components::Label(event.getUniqueID());
    }

    inline components::Toggle MenuTab::addToggle(const std::string& id, const std::string& title, const std::function<void(bool)>& callback) const {
        events::AddToggleEvent event(this, id, title, callback);
        event.post();
        return components::Toggle(event.getUniqueID(), id);
    }

    namespace config {
        template <typename T>
        concept SupportedType = requires(T a) {
            std::same_as<T, bool> || std::same_as<T, int> || std::same_as<T, float> || std::same_as<T, std::string>;
        };

        template <SupportedType T>
        T get(std::string key, T defaultValue) {
            events::RequestConfigValueEvent<T> event(key);
            event.post();
            if (event.hasValue())
                return event.getValue();
            return defaultValue;
        }

        template <SupportedType T>
        T getInternal(std::string key, T defaultValue) {
            events::RequestConfigValueEvent<T> event(key, true);
            event.post();
            if (event.hasValue())
                return event.getValue();
            return defaultValue;
        }

        template <SupportedType T>
        void set(std::string key, T value) {
            /* TODO: Implement */
        }
    }
}
