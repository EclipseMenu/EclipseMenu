#pragma once
#include "Keybind.hpp"
#include "../Prelude.hpp"

namespace eclipse {
    struct SavedKeybind { Keybind keybind; KeybindMode mode; };

    /// @brief Manages all keybindings and propagates input throughout the menu
    class ECLIPSE_DLL InputManager {
    public:
        /// @brief Gets the global InputManager instance
        /// @return The global InputManager instance
        static InputManager* get()
            ECLIPSE_EVENT_METHOD(InputManager::get);

        InputManager(InputManager const&) = delete;
        InputManager& operator=(InputManager const&) = delete;
        InputManager(InputManager&&) = delete;
        InputManager& operator=(InputManager&&) = delete;

        using GlobalListener = Function<bool(cocos2d::enumKeyCodes key, KeyboardModifier mods, bool isDown)>;

        /// @brief Registers a keybind with the InputManager
        /// @param id A unique string identifier for the keybind, used for saving/loading and displaying to the user
        /// @param callback The function to call when the keybind is triggered
        /// @param ephemeral Whether the keybind will be visible in the keybinds tab. Used for "internal" keybinds like StartPos Switcher, which have its own settings
        void registerListener(std::string id, Binding::Action callback, bool ephemeral = false)
            ECLIPSE_EVENT_METHOD(InputManager::registerListener, this, std::move(id), std::move(callback), ephemeral);

        /// @brief Should be invoked from keyboard listeners, handles the event and propagates it to registered keybinds
        /// @param key The key that was pressed/released (see cocos2d::enumKeyCodes)
        /// @param mods The keyboard modifiers active during the event
        /// @param isDown Whether the key was pressed (true) or released (false)
        /// @param timestamp The timestamp of the event (geode::utils::getInputTimestamp())
        /// @return true if the event was "swallowed" (shouldn't be passed further), false otherwise
        bool onKeyEvent(cocos2d::enumKeyCodes key, KeyboardModifier mods, bool isDown, double timestamp)
            ECLIPSE_EVENT_METHOD(InputManager::onKeyEvent, this, key, mods, isDown, timestamp);

        /// @brief Sets a global listener that will be called for every key event, regardless of registered keybinds. Used for rebind UI.
        /// @param listener The function to call for every key event
        void setGlobalListener(GlobalListener listener)
            ECLIPSE_EVENT_METHOD(InputManager::setGlobalListener, this, std::move(listener));

        void setDefaultKeybind(std::string id, Keybind keybind, KeybindMode mode = KeybindMode::Toggle)
            ECLIPSE_EVENT_METHOD(InputManager::setDefaultKeybind, this, std::move(id), keybind, mode);

    private:
        InputManager();

        StringMap<std::unique_ptr<Binding>> m_bindings;
        StringMap<SavedKeybind> m_loadedKeybinds;
        GlobalListener m_globalListener;
    };
}

template <>
struct matjson::Serialize<eclipse::SavedKeybind> {
    static Value toJson(eclipse::SavedKeybind const& saved) {
        return makeObject({
            {"keybind", saved.keybind},
            {"mode", static_cast<int>(saved.mode)}
        });
    }

    static geode::Result<eclipse::SavedKeybind> fromJson(Value const& json) {
        GEODE_UNWRAP_INTO(eclipse::Keybind keybind, json["keybind"].as<eclipse::Keybind>());
        GEODE_UNWRAP_INTO(int modeInt, json["mode"].asInt());

        return geode::Ok(eclipse::SavedKeybind{
            .keybind = keybind,
            .mode = static_cast<eclipse::KeybindMode>(modeInt)
        });
    }
};