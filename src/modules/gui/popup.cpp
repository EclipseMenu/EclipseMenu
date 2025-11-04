#include "popup.hpp"

#include <modules/i18n/translations.hpp>

#include "gui.hpp"

namespace eclipse {
    size_t Popup::s_instanceCounter = 0;

    void Popup::create(
        std::string title, std::string message, std::string button1,
        std::string button2, PopupCallback&& callback
    ) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_title = std::move(title);
        popup.m_message = std::move(message);
        popup.m_button1 = std::move(button1);
        popup.m_button2 = std::move(button2);
        popup.m_callback = std::move(callback);
        s_instanceCounter++;
        gui::Engine::get()->showPopup(std::move(popup));
    }

    void Popup::create(
        std::string title, std::string message, std::string button1,
        PopupCallback&& callback
    ) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_title = std::move(title);
        popup.m_message = std::move(message);
        popup.m_button1 = std::move(button1);
        popup.m_callback = std::move(callback);
        s_instanceCounter++;
        gui::Engine::get()->showPopup(std::move(popup));
    }

    void Popup::create(std::string title, std::string message) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_title = std::move(title);
        popup.m_message = std::move(message);
        popup.m_button1 = i18n::get_("common.ok");
        gui::Engine::get()->showPopup(std::move(popup));
    }

    void Popup::prompt(
        std::string title, std::string message,
        PromptCallback&& callback, std::string defaultValue
    ) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_prompt = true;
        popup.m_title = std::move(title);
        popup.m_message = std::move(message);
        popup.m_button1 = i18n::get_("common.ok");
        popup.m_promptValue = std::move(defaultValue);
        popup.m_promptCallback = std::move(callback);
        gui::Engine::get()->showPopup(std::move(popup));
    }

    void Popup::prompt(
        std::string title, std::string message, PromptCallback&& callback,
        std::string button1, std::string button2, std::string defaultValue
    ) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_prompt = true;
        popup.m_title = std::move(title);
        popup.m_message = std::move(message);
        popup.m_button1 = std::move(button1);
        popup.m_button2 = std::move(button2);
        popup.m_promptValue = std::move(defaultValue);
        popup.m_promptCallback = std::move(callback);
        gui::Engine::get()->showPopup(std::move(popup));
    }

    void Popup::prompt(
        std::string title, std::string message, PromptCallback&& callback,
        std::string button1, std::string defaultValue
    ) {
        Popup popup;
        popup.m_id = s_instanceCounter++;
        popup.m_prompt = true;
        popup.m_title = std::move(title);
        popup.m_message = std::move(message);
        popup.m_button1 = std::move(button1);
        popup.m_promptValue = std::move(defaultValue);
        popup.m_promptCallback = std::move(callback);
        gui::Engine::get()->showPopup(std::move(popup));
    }
}
