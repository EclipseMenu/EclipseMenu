#pragma once

#include <utility>
#include <string>
#include <vector>

namespace eclipse::gui {

    class Component {
    public:
        /// @brief Initialize the component.
        virtual void onInit() = 0;

        /// @brief Update the component.
        virtual void onUpdate() = 0;

        /// @brief Get the component's ID. (unique identifier)
        [[nodiscard]] virtual const std::string& getId() const = 0;

        /// @brief Get the component's title. (used for sorting in the menu)
        [[nodiscard]] virtual const std::string& getTitle() const { return getId(); }
    };

    /// @brief Simple label component, that displays a title.
    class LabelComponent : public Component {
    public:
        explicit LabelComponent(std::string title) : m_title(std::move(title)) {}

        void onInit() override {}
        void onUpdate() override {}

        [[nodiscard]] const std::string& getId() const override { return m_title; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

    private:
        std::string m_title;
    };

    /// @brief Simple toggle component, that displays a title and a checkbox.
    class ToggleComponent : public Component {
    public:
        explicit ToggleComponent(std::string id, std::string title)
            : m_id(std::move(id)), m_title(std::move(title)) {}

        void onInit() override {}
        void onUpdate() override {}

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

    private:
        std::string m_id;
        std::string m_title;
    };

    /// @brief Contains a list of components and a title, to be passed into render engine.
    class MenuTab {
    public:
        explicit MenuTab(std::string title) : m_title(std::move(title)) {}

        /// @brief Add a component to the tab.
        void addComponent(Component* component) {
            m_components.push_back(component);
        }

        /// @brief Add a label to the tab.
        void addLabel(const std::string& title) {
            addComponent(new LabelComponent(title));
        }

        /// @brief Add a checkbox to the tab.
        void addToggle(const std::string& title, const std::string& id) {
            addComponent(new ToggleComponent(id, title));
        }

        /// @brief Get the tab's title.
        [[nodiscard]] const std::string& getTitle() const { return m_title; }

        /// @brief Get the tab's components.
        [[nodiscard]] const std::vector<Component*>& getComponents() const { return m_components; }

        /// @brief Find a tab by name (or create a new one if it does not exist).
        static MenuTab* find(const std::string& name);

    private:
        std::string m_title;
        std::vector<Component*> m_components;
    };

    /// @brief Abstract class, that wraps all UI function calls.
    class Engine {
    public:
        /// @brief Get the UI engine instance. (ImGui for desktop, Cocos2d for mobile)
        static Engine* get();

        /// @brief Initialize the UI engine.
        virtual void init() = 0;

        /// @brief Find a tab by name.
        virtual MenuTab* findTab(const std::string& name) = 0;
    };


}