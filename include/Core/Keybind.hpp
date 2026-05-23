#pragma once
#include <Geode/utils/Keyboard.hpp>
#include "../Prelude.hpp"

namespace eclipse {
    using geode::KeyboardModifier;

    struct Keybind {
        static constexpr cocos2d::enumKeyCodes UNBOUND = cocos2d::KEY_Unknown;

        cocos2d::enumKeyCodes key = UNBOUND;
        KeyboardModifier mods = KeyboardModifier::None;

        constexpr Keybind() noexcept = default;
        explicit(false) Keybind(cocos2d::enumKeyCodes key) noexcept : key(key) {}
        constexpr Keybind(cocos2d::enumKeyCodes key, KeyboardModifier mods) noexcept : key(key), mods(mods) {}

        static constexpr Keybind unbound() noexcept {
            return Keybind{ UNBOUND, KeyboardModifier::None };
        }

        [[nodiscard]] constexpr bool isBound() const noexcept {
            return key != UNBOUND;
        }

        [[nodiscard]] constexpr bool matches(cocos2d::enumKeyCodes k, KeyboardModifier m) const noexcept {
            return key == k && (mods & m) == mods;
        }

        [[nodiscard]] constexpr bool operator==(Keybind const&) const noexcept = default;

        [[nodiscard]] std::string toString() const;
    };

    enum class KeybindMode : uint8_t {
        Toggle,
        Enable,
        Disable,
        HoldOn,
        HoldOff,
    };

    class Binding {
    public:
        using Action = Function<bool(KeybindMode mode, bool isDown, double timestamp)>;

        Binding(std::string id, Action callback)
            : m_id(std::move(id)), m_callback(std::move(callback)) {}

        Binding(Binding const&) = delete;
        Binding& operator=(Binding const&) = delete;

        [[nodiscard]] std::string const& id() const noexcept { return m_id; }

        [[nodiscard]] Keybind const& keybind() const noexcept { return m_keybind; }
        void setKeybind(Keybind keybind) noexcept { m_keybind = keybind; }

        [[nodiscard]] KeybindMode mode() const noexcept { return m_mode; }
        void setMode(KeybindMode mode) noexcept { m_mode = mode; }

        [[nodiscard]] bool isEphemeral() const noexcept { return m_ephemeral; }
        void setEphemeral(bool ephemeral) noexcept { m_ephemeral = ephemeral; }

        [[nodiscard]] bool invoke(bool isDown, cocos2d::enumKeyCodes key, KeyboardModifier mods, double timestamp) {
            if (!m_callback) return false;
            if (!m_keybind.isBound() || !m_keybind.matches(key, mods)) return false;
            return m_callback(m_mode, isDown, timestamp);
        }

    private:
        std::string m_id;
        Action m_callback;
        Keybind m_keybind;
        KeybindMode m_mode = KeybindMode::Toggle;
        bool m_ephemeral = false; // doesn't show up in keybinds list
    };
}

template <>
struct matjson::Serialize<eclipse::Keybind> {
    static Value toJson(eclipse::Keybind const& keybind) {
        if (!keybind.isBound()) return Value(nullptr);

        return makeObject({
            {"key", static_cast<int32_t>(keybind.key)},
            {"mods", keybind.mods.value}
        });
    }

    static geode::Result<eclipse::Keybind> fromJson(Value const& value) {
        if (value.isNull()) {
            return geode::Ok(eclipse::Keybind::unbound());
        }

        GEODE_UNWRAP_INTO(int32_t key, value["key"].asInt());
        GEODE_UNWRAP_INTO(geode::KeyboardModifier mods, value["mods"].asInt());

        return geode::Ok(eclipse::Keybind{
            static_cast<cocos2d::enumKeyCodes>(key),
            mods
        });
    }
};