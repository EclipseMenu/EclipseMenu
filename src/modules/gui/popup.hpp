#pragma once
#include <functional>
#include <string>

namespace eclipse {
    /// @brief Represents a classic modal window, with a title, a message and a set of buttons.
    class Popup {
    public:
        using PopupCallback = std::function<void(bool)>;
        using PromptCallback = std::function<void(bool, std::string const&)>;


        static void create(
            std::string title, std::string message, std::string button1,
            std::string button2, PopupCallback callback
        );
        static void create(
            std::string title, std::string message, std::string button1,
            PopupCallback callback
        );
        static void create(std::string title, std::string message);


        static void prompt(
            std::string title, std::string message, PromptCallback callback,
            std::string defaultValue = ""
        );
        static void prompt(
            std::string title, std::string message, PromptCallback callback,
            std::string button1, std::string button2, std::string defaultValue = ""
        );
        static void prompt(
            std::string title, std::string message, PromptCallback callback,
            std::string button1, std::string defaultValue = ""
        );

        [[nodiscard]] size_t getId() const { return m_id; }
        [[nodiscard]] bool isPrompt() const { return m_prompt; }
        [[nodiscard]] const std::string& getTitle() const { return m_title; }
        [[nodiscard]] const std::string& getMessage() const { return m_message; }
        [[nodiscard]] const std::string& getButton1() const { return m_button1; }
        [[nodiscard]] const std::string& getButton2() const { return m_button2; }
        [[nodiscard]] const PopupCallback& getCallback() const { return m_callback; }

        [[nodiscard]] std::string& getPromptValue() { return m_promptValue; }
        [[nodiscard]] const std::string& getPromptValue() const { return m_promptValue; }
        [[nodiscard]] const PromptCallback& getPromptCallback() const { return m_promptCallback; }

    private:
        static size_t s_instanceCounter;

        size_t m_id = 0;
        bool m_prompt = false;
        std::string m_title;
        std::string m_message;
        std::string m_button1;
        std::string m_button2;
        PopupCallback m_callback = [](bool) {};

        std::string m_promptValue;
        PromptCallback m_promptCallback;
    };
}
