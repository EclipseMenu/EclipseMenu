#pragma once

#include <functional.hpp>
#include <Geode/Geode.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>
#include <Geode/utils/Keyboard.hpp>

namespace eclipse::keybinds {
    enum class Keys : uint8_t {
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

    struct KeybindProps {
        Keys key;
        geode::KeyboardModifier mods;

        constexpr KeybindProps(Keys key = Keys::None, geode::KeyboardModifier mods = {}) noexcept
            : key(key), mods(mods) {}

        constexpr bool operator==(KeybindProps const& other) const noexcept {
            return key == other.key && mods == other.mods;
        }

        constexpr bool operator==(Keys other) const noexcept {
            return key == other;
        }
    };

    struct KeyEvent {
        double timestamp;
        KeybindProps props;
        bool down;
    };

    /// @brief Convert a key to a string.
    /// @param key Key to convert.
    /// @return String representation of the key.
    [[nodiscard]] std::string keyToString(KeybindProps key);

    /// @brief Convert a string to a key.
    /// @param key String to convert.
    /// @return Key representation of the string.
    [[nodiscard]] Keys stringToKey(std::string_view key);

    /// @brief Check if a key is currently being pressed.
    bool isKeyDown(Keys key);

    /// @brief Check if a keybind is currently being pressed.
    bool isKeyDown(KeybindProps key);

    /// @brief Check if a key was pressed this frame.
    bool isKeyPressed(Keys key);

    /// @brief Check if a keybind was pressed this frame.
    bool isKeyPressed(KeybindProps key);

    /// @brief Check if a key was released this frame.
    bool isKeyReleased(Keys key);

    /// @brief Check if a keybind was released this frame.
    bool isKeyReleased(KeybindProps key);

    /// @brief Get the current modifiers state.
    /// @return The current modifiers state.
    geode::KeyboardModifier getCurrentModifiers();

    /// @brief A keybind that can be used to execute a callback when a key is pressed.
    class Keybind {
    public:
        /// @brief Construct a keybind.
        /// @param key The key of the keybind.
        /// @param id The ID of the keybind.
        /// @param title The title of the keybind.
        /// @param callback The callback to execute when the keybind is pressed.
        /// @param internal Whether the keybind is internal or not.
        Keybind(KeybindProps key, std::string id, std::string title, Function<void(KeyEvent)>&& callback, bool internal = false)
            : m_id(std::move(id)), m_title(std::move(title)), m_callback(std::move(callback)), m_key(key),
              m_internal(internal) {}

        Keybind(Keybind&&) = default;
        Keybind& operator=(Keybind&&) = default;
        Keybind(Keybind const&) = delete;
        Keybind& operator=(Keybind const&) = delete;

        /// @brief Get the key of the keybind.
        [[nodiscard]] KeybindProps getKey() const { return m_key; }

        /// @brief Execute the keybind's callback with the given state.
        void execute(KeyEvent down) { m_callback(down); }

        /// @brief Get the ID of the keybind.
        [[nodiscard]] std::string const& getId() const { return m_id; }

        /// @brief Get the title of the keybind.
        [[nodiscard]] std::string const& getTitle() const { return m_title; }

        /// @brief Check if the keybind is initialized.
        [[nodiscard]] bool isInitialized() const { return m_initialized; }

        /// @brief Check if the keybind is initialized.
        [[nodiscard]] bool isInternal() const { return m_internal; }

        /// @brief Set the keybind as initialized.
        void setInitialized(bool initialized) { m_initialized = initialized; }

        /// @brief Set the key of the keybind.
        void setKey(KeybindProps key) { m_key = key; }

        /// @brief Set the title of the keybind.
        void setTitle(std::string title) { m_title = std::move(title); }

    private:
        std::string m_id;
        std::string m_title;
        Function<void(KeyEvent)> m_callback;
        KeybindProps m_key;
        bool m_initialized = false;
        bool m_internal = false;
    };

    /// @brief A manager for keybinds.
    class Manager {
    public:
        /// @brief Get the keybind manager.
        static Manager* get();

        /// @brief Register a keybind to the manager (in case the keybind will be later used)
        /// @param id The ID of the keybind.
        /// @param title The title of the keybind.
        /// @param callback The callback to execute when the keybind is pressed.
        Keybind& registerKeybind(std::string id, std::string title, Function<void(KeyEvent)>&& callback);

        /// @brief Register a keybind without adding it to the keybinds UI tab. Useful for internal keybinds.
        /// @param id The ID of the keybind.
        /// @param callback The callback to execute when the keybind is pressed.
        Keybind& addListener(std::string id, Function<void(KeyEvent)>&& callback);

        /// @brief Register a global listener that will be called for every key event.
        void registerGlobalListener(Function<bool(KeyEvent)>&& callback);

        /// @brief Unregister a keybind from the manager. This will completely remove it from the configuration.
        /// @param id The ID of the keybind.
        /// @return Whether the keybind was successfully unregistered.
        bool unregisterKeybind(std::string const& id);

        /// @brief Load keybinds from config.
        void init();

        /// @brief Sets up the keybind manager UI tab.
        void setupTab();

        /// @brief Update the keybinds. This should be called every frame.
        void update();

        /// @brief Get all keybinds.
        /// @return All keybinds.
        [[nodiscard]] std::vector<Keybind> const& getKeybinds() const { return m_keybinds; }

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
        void registerKeyPress(KeyEvent key);

        /// @brief Register a key release.
        /// @note This function is called from the key callback hook.
        void registerKeyRelease(KeyEvent key);

        /// @brief Get the unique ID for the menu keybind component.
        [[nodiscard]] size_t getMenuKeybindUID() const { return m_menuKeybindUID; }

    private:
        std::vector<Keybind> m_keybinds;
        std::vector<Function<bool(KeyEvent)>> m_globalListeners;
        std::unordered_map<Keys, bool> m_keyStates;
        std::unordered_map<Keys, bool> m_lastKeyStates;
        size_t m_menuKeybindUID = 0;
        bool m_initialized = false;

        Keybind& registerKeybindInternal(std::string id, std::string title, Function<void(KeyEvent)>&& callback, bool internal);

        friend bool isKeyDown(Keys key);
        friend bool isKeyPressed(Keys key);
        friend bool isKeyReleased(Keys key);
        friend bool isKeyDown(KeybindProps key);
        friend bool isKeyPressed(KeybindProps key);
        friend bool isKeyReleased(KeybindProps key);
    };
}

template <>
struct matjson::Serialize<eclipse::keybinds::KeybindProps> {
    static Value toJson(eclipse::keybinds::KeybindProps const& key) {
        return static_cast<int>(key.key) | (static_cast<int>(key.mods) << 8);
    }

    static geode::Result<eclipse::keybinds::KeybindProps> fromJson(Value const& value) {
        GEODE_UNWRAP_INTO(int keyInt, value.as<int>());
        eclipse::keybinds::KeybindProps props;
        props.key = static_cast<eclipse::keybinds::Keys>(keyInt & 0xFF);
        props.mods = (keyInt >> 8) & 0xFF;
        return geode::Ok(props);
    }
};