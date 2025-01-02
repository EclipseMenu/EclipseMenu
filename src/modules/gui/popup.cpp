#include "popup.hpp"

#include <modules/i18n/translations.hpp>

#include "gui.hpp"

namespace eclipse {
    size_t Popup::s_instanceCounter = 0;

    void Popup::create(
        const std::string& title, const std::string& message, const std::string& button1,
        const std::string& button2, const std::function<void(bool)>& callback
    ) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_title = title;
        popup.m_message = message;
        popup.m_button1 = button1;
        popup.m_button2 = button2;
        popup.m_callback = callback;
        s_instanceCounter++;
        gui::Engine::get()->showPopup(popup);
    }

    void Popup::create(
        const std::string& title, const std::string& message, const std::string& button1,
        const std::function<void(bool)>& callback
    ) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_title = title;
        popup.m_message = message;
        popup.m_button1 = button1;
        popup.m_callback = callback;
        s_instanceCounter++;
        gui::Engine::get()->showPopup(popup);
    }

    void Popup::create(const std::string& title, const std::string& message) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_title = title;
        popup.m_message = message;
        popup.m_button1 = i18n::get_("common.ok");
        gui::Engine::get()->showPopup(popup);
    }

    void Popup::prompt(
        const std::string& title, const std::string& message,
        const PromptCallback& callback, const std::string& defaultValue
    ) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_prompt = true;
        popup.m_title = title;
        popup.m_message = message;
        popup.m_button1 = i18n::get_("common.ok");
        popup.m_promptValue = defaultValue;
        popup.m_promptCallback = callback;
        gui::Engine::get()->showPopup(popup);
    }

    void Popup::prompt(
        const std::string& title, const std::string& message, const PromptCallback& callback,
        const std::string& button1, const std::string& button2, const std::string& defaultValue
    ) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_prompt = true;
        popup.m_title = title;
        popup.m_message = message;
        popup.m_button1 = button1;
        popup.m_button2 = button2;
        popup.m_promptValue = defaultValue;
        popup.m_promptCallback = callback;
        gui::Engine::get()->showPopup(popup);
    }

    void Popup::prompt(
        const std::string& title, const std::string& message, const PromptCallback& callback,
        const std::string& button1, const std::string& defaultValue
    ) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_prompt = true;
        popup.m_title = title;
        popup.m_message = message;
        popup.m_button1 = button1;
        popup.m_promptValue = defaultValue;
        popup.m_promptCallback = callback;
        gui::Engine::get()->showPopup(popup);
    }
}
