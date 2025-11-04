#pragma once
#include <string>
#include <std23/move_only_function.h>
#include <functional.hpp>

namespace eclipse {
    /// @brief Represents a classic modal window, with a title, a message and a set of buttons.
    class Popup {
    public:
        using PopupCallback = Function<void(bool)>;
        using PromptCallback = Function<void(bool, std::string const&)>;

        Popup() = default;
        ~Popup() = default;
        Popup(Popup const&) = delete;
        Popup(Popup&&) noexcept = default;
        Popup& operator=(Popup const&) = delete;
        Popup& operator=(Popup&&) noexcept = default;

        static void create(
            std::string title, std::string message, std::string button1,
            std::string button2, PopupCallback&& callback
        );
        static void create(
            std::string title, std::string message, std::string button1,
            PopupCallback&& callback
        );
        static void create(std::string title, std::string message);


        static void prompt(
            std::string title, std::string message, PromptCallback&& callback,
            std::string defaultValue = ""
        );
        static void prompt(
            std::string title, std::string message, PromptCallback&& callback,
            std::string button1, std::string button2, std::string defaultValue = ""
        );
        static void prompt(
            std::string title, std::string message, PromptCallback&& callback,
            std::string button1, std::string defaultValue = ""
        );

        [[nodiscard]] size_t getId() const { return m_id; }
        [[nodiscard]] bool isPrompt() const { return m_prompt; }
        [[nodiscard]] std::string const& getTitle() const { return m_title; }
        [[nodiscard]] std::string const& getMessage() const { return m_message; }
        [[nodiscard]] std::string const& getButton1() const { return m_button1; }
        [[nodiscard]] std::string const& getButton2() const { return m_button2; }
        [[nodiscard]] PopupCallback& getCallback() { return m_callback; }

        [[nodiscard]] std::string& getPromptValue() { return m_promptValue; }
        [[nodiscard]] std::string const& getPromptValue() const { return m_promptValue; }
        [[nodiscard]] PromptCallback& getPromptCallback() { return m_promptCallback; }

    private:
        static size_t s_instanceCounter;

        size_t m_id = 0;
        bool m_prompt = false;
        std::string m_title;
        std::string m_message;
        std::string m_button1;
        std::string m_button2;
        PopupCallback m_callback;

        std::string m_promptValue;
        PromptCallback m_promptCallback;
    };
}
