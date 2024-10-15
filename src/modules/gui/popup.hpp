#pragma once
#include <string>
#include <functional>
#include <stdint.h>

namespace eclipse {

    /// @brief Represents a classic modal window, with a title, a message and a set of buttons.
    class Popup {
    public:
        using PopupCallback = std::function<void(bool)>;
        using PromptCallback = std::function<void(bool, const std::string&)>;


        static void create(const std::string &title, const std::string &message, const std::string &button1,
                           const std::string &button2, const PopupCallback &callback);
        static void create(const std::string &title, const std::string &message, const std::string &button1,
                           const PopupCallback &callback);
        static void create(const std::string &title, const std::string &message);


        static void prompt(const std::string &title, const std::string& message, const PromptCallback &callback,
                           const std::string& defaultValue = "");
        static void prompt(const std::string &title, const std::string& message, const PromptCallback &callback,
                           const std::string& button1, const std::string& button2, const std::string& defaultValue = "");
        static void prompt(const std::string &title, const std::string& message, const PromptCallback &callback,
                           const std::string& button1, const std::string& defaultValue = "");

        [[nodiscard]] size_t getId() const { return m_id; }
        [[nodiscard]] bool isPrompt() const { return m_prompt; }
        [[nodiscard]] const std::string& getTitle() const { return m_title; }
        [[nodiscard]] const std::string& getMessage() const { return m_message; }
        [[nodiscard]] const std::string& getButton1() const { return m_button1; }
        [[nodiscard]] const std::string& getButton2() const { return m_button2; }
        [[nodiscard]] const PopupCallback& getCallback() const { return m_callback; }

        [[nodiscard]] std::string& getPromptValue() { return m_promptValue; }
        [[nodiscard]] const PromptCallback& getPromptCallback() const { return m_promptCallback; }

    private:
        static size_t s_instanceCounter;

        size_t m_id = 0;
        bool m_prompt = false;
        std::string m_title;
        std::string m_message;
        std::string m_button1;
        std::string m_button2;
        PopupCallback m_callback = [](bool){};

        std::string m_promptValue;
        PromptCallback m_promptCallback;
    };

}