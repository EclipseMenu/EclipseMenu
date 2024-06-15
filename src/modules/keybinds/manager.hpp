#pragma once

#include <Geode/Geode.hpp>

#include <utility>
#include <vector>
#include <unordered_map>
#include <functional>

namespace eclipse::keybinds {

    enum class Keys {
        None,

        // Letters
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // Symbols
        Space, Apostrophe, Comma, Minus, Period, Slash, Semicolon, Equal,
        LeftBracket, Backslash, RightBracket, GraveAccent, World1, World2,

        // Numbers
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

        // Function keys
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13,
        F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,

        // Keypad
        NumPad0, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,
        NumPadDecimal, NumPadDivide, NumPadMultiply, NumPadSubtract, NumPadAdd, NumPadEnter, NumPadEqual,

        // Special keys
        Menu, Escape, Enter, Tab, Backspace, Insert, Delete, Home, End, PageUp, PageDown,
        CapsLock, ScrollLock, NumLock, PrintScreen, Pause,

        // Arrow keys
        Up, Down, Left, Right,

        // Modifier keys
        LeftShift, LeftControl, LeftAlt, LeftSuper, RightShift, RightControl, RightAlt, RightSuper,

        // System keys
        MenuKey, LastKey
    };

    static constexpr int KEY_COUNT = static_cast<int>(Keys::LastKey);

    /// @brief Convert a key to a string.
    /// @param key Key to convert.
    /// @return String representation of the key.
    [[nodiscard]] static const std::string& keyToString(Keys key);

    /// @brief Convert a string to a key.
    /// @param key String to convert.
    /// @return Key representation of the string.
    [[nodiscard]] static Keys stringToKey(const std::string& key);

    /// @brief Check if a key is currently being pressed.
    bool isKeyDown(Keys key);

    /// @brief Check if a key was pressed this frame.
    bool isKeyPressed(Keys key);

    /// @brief Check if a key was released this frame.
    bool isKeyReleased(Keys key);

    /// @brief A keybind that can be used to execute a callback when a key is pressed.
    class Keybind {
    public:
        Keybind(Keys key, const std::function<void()>& callback) : m_key(key), m_callback(callback) {}

        /// @brief Get the key of the keybind.
        [[nodiscard]] Keys getKey() const { return m_key; }

        /// @brief Get the callback of the keybind.
        [[nodiscard]] const std::function<void()>& getCallback() const { return m_callback; }

        /// @brief Check if the keybind has been initialized.
        [[nodiscard]] bool isInitialized() const { return m_initialized; }

        /// @brief Disable the keybind.
        void disable() { m_initialized = false; }

        /// @brief Enable the keybind.
        void enable() { m_initialized = true; }

        /// @brief Set the key of the keybind.
        void setKey(Keys key) { m_key = key; }

        /// @brief Execute the keybind's callback.
        void execute() { m_callback(); }

    private:
        Keys m_key;
        std::function<void()> m_callback;
        bool m_initialized = false;
    };

    /// @brief A manager for keybinds.
    class Manager {
    public:
        /// @brief Get the keybind manager.
        static Manager* get();

        /// @brief Register a keybind to the manager (in case the keybind will be later used)
        /// @param id The ID of the keybind.
        /// @param callback The callback to execute when the keybind is pressed.
        void registerKeybind(const std::string& id, const std::function<void()>& callback);

        /// @brief Update the keybinds. This should be called every frame.
        void update();

        /// @brief Get all keybinds.
        /// @return All keybinds.
        [[nodiscard]] const std::vector<Keybind>& getKeybinds() const { return m_keybinds; }

        /// @brief Register a key press.
        /// @note This function is called from the key callback hook.
        void registerKeyPress(Keys key);

        /// @brief Register a key release.
        /// @note This function is called from the key callback hook.
        void registerKeyRelease(Keys key);

    private:
        std::vector<Keybind> m_keybinds;
        std::unordered_map<Keys, bool> m_keyStates;
        std::unordered_map<Keys, bool> m_lastKeyStates;

        friend bool isKeyDown(Keys key);
        friend bool isKeyPressed(Keys key);
        friend bool isKeyReleased(Keys key);
    };


}