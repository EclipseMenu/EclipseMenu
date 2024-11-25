#pragma once

#include <Geode/Geode.hpp>

#include <memory>
#include <utility>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>

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

        // Mouse buttons
        MouseLeft, MouseRight, MouseMiddle, MouseButton4, MouseButton5,

        // System keys
        MenuKey, LastKey,

        // Unknown key
        Unknown
    };

    Keys& operator++(Keys& key);
    Keys operator++(Keys& key, int);

    static constexpr int KEY_COUNT = static_cast<int>(Keys::LastKey);

    /// @brief Convert a key to a string.
    /// @param key Key to convert.
    /// @return String representation of the key.
    [[nodiscard]] std::string keyToString(Keys key);

    /// @brief Convert a string to a key.
    /// @param key String to convert.
    /// @return Key representation of the string.
    [[nodiscard]] Keys stringToKey(const std::string& key);

    /// @brief Check if a key is currently being pressed.
    bool isKeyDown(Keys key);

    /// @brief Check if a key was pressed this frame.
    bool isKeyPressed(Keys key);

    /// @brief Check if a key was released this frame.
    bool isKeyReleased(Keys key);

    /// @brief A keybind that can be used to execute a callback when a key is pressed.
    class Keybind {
    public:
        /// @brief Construct a keybind.
        /// @param key The key of the keybind.
        /// @param id The ID of the keybind.
        /// @param title The title of the keybind.
        /// @param callback The callback to execute when the keybind is pressed.
        /// @param internal Whether the keybind is internal or not.
        Keybind(Keys key, std::string id, std::string title, std::function<void(bool)> callback, bool internal = false)
            : m_key(key), m_id(std::move(id)), m_title(std::move(title)), m_callback(std::move(callback)), m_internal(internal) {}

        /// @brief Get the key of the keybind.
        [[nodiscard]] Keys getKey() const { return m_key; }

        /// @brief Execute the keybind's callback with the given state.
        void execute(bool down) const { m_callback(down); }

        /// @brief Execute the pressed callback.
        void push() const { execute(true); }

        /// @brief Execute the released callback.
        void release() const { execute(false); }

        /// @brief Get the ID of the keybind.
        [[nodiscard]] const std::string& getId() const { return m_id; }

        /// @brief Get the title of the keybind.
        [[nodiscard]] const std::string& getTitle() const { return m_title; }

        /// @brief Check if the keybind is initialized.
        [[nodiscard]] bool isInitialized() const { return m_initialized; }

        /// @brief Check if the keybind is initialized.
        [[nodiscard]] bool isInternal() const { return m_internal; }

        /// @brief Set the keybind as initialized.
        void setInitialized(bool initialized) { m_initialized = initialized; }

        /// @brief Set the key of the keybind.
        void setKey(Keys key) { m_key = key; }

    private:
        Keys m_key;
        std::string m_id;
        std::string m_title;
        std::function<void(bool)> m_callback;
        bool m_initialized = false;
        bool m_internal = false;
    };

    /// @brief A manager for keybinds.
    class Manager {
    public:
        /// @brief Get the keybind manager.
        static std::shared_ptr<Manager> get();

        /// @brief Register a keybind to the manager (in case the keybind will be later used)
        /// @param id The ID of the keybind.
        /// @param title The title of the keybind.
        /// @param callback The callback to execute when the keybind is pressed.
        Keybind& registerKeybind(const std::string& id, const std::string& title, const std::function<void(bool)>& callback);

        /// @brief Register a keybind without adding it to the keybinds UI tab. Useful for internal keybinds.
        /// @param id The ID of the keybind.
        /// @param callback The callback to execute when the keybind is pressed.
        Keybind& addListener(const std::string& id, const std::function<void(bool)>& callback);

        /// @brief Load keybinds from config.
        void init();

        /// @brief Sets up the keybind manager UI tab.
        void setupTab();

        /// @brief Update the keybinds. This should be called every frame.
        void update();

        /// @brief Get all keybinds.
        /// @return All keybinds.
        [[nodiscard]] const std::vector<Keybind>& getKeybinds() const { return m_keybinds; }

        /// @brief Get a keybind by its ID.
        /// @param id The ID of the keybind.
        /// @return The keybind with the given ID.
        [[nodiscard]] std::optional<std::reference_wrapper<Keybind>> getKeybind(std::string_view id);

        /// @brief Set whether a keybind is enabled or not.
        /// @param id The ID of the keybind.
        /// @param state The state of the keybind.
        void setKeybindState(std::string_view id, bool state);

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
        bool m_initialized = false;

        Keybind& registerKeybindInternal(const std::string& id, const std::string& title, const std::function<void(bool)>& callback, bool internal);

        friend bool isKeyDown(Keys key);
        friend bool isKeyPressed(Keys key);
        friend bool isKeyReleased(Keys key);
    };


}