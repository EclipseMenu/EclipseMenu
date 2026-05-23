#include <ranges>
#include <Core/InputManager.hpp>
#include <Core/Keybind.hpp>

using namespace eclipse::prelude;

namespace eclipse {
    struct Names { std::string_view CTRL, ALT, SHIFT, SUPER; };
    static constexpr auto n = Names {
        GEODE_WINDOWS("Ctrl+", "Alt+", "Shift+", "Win+")
        GEODE_ANDROID("Ctrl+", "Alt+", "Shift+", "Super+")
        GEODE_MACOS("Control+", "Option+", "Shift+", "Command+")
        GEODE_IOS("Control+", "Option+", "Shift+", "Command+")
    };

    static void modsToString(utils::StringBuffer<64>& buffer, KeyboardModifier mods, enumKeyCodes key) {
        auto test = [&](KeyboardModifier mod, std::array<enumKeyCodes, 3> keys) {
            if (!(mods & mod)) return false;
            for (auto k : keys) {
                if (key == k) return false;
            }
            return true;
        };

        if (test(KeyboardModifier::Control, {KEY_LeftControl, KEY_RightControl, KEY_Control})) buffer.append(n.CTRL);
        if (test(KeyboardModifier::Alt, {KEY_LeftMenu, KEY_RightMenu, KEY_Alt})) buffer.append(n.ALT);
        if (test(KeyboardModifier::Shift, {KEY_LeftShift, KEY_RightShift, KEY_Shift})) buffer.append(n.SHIFT);
        if (test(KeyboardModifier::Super, {KEY_LeftWindowsKey, KEY_RightWindowsKey, KEY_None})) buffer.append(n.SUPER);
    }

    static void keyToString(utils::StringBuffer<64>& buffer, enumKeyCodes key) {
        if (key >= KEY_A && key <= KEY_Z) {
            buffer.append(static_cast<char>('A' + (key - KEY_A)));
        } else if (key >= KEY_Zero && key <= KEY_Nine) {
            buffer.append(static_cast<char>('0' + (key - KEY_Zero)));
        } else if (key >= KEY_NumPad0 && key <= KEY_NumPad9) {
            buffer.append("Num {}", key - KEY_NumPad0);
        } else if (key >= KEY_F1 && key <= KEY_F24) {
            buffer.append("F{}", key - KEY_F1);
        } else switch (key) {
            case KEY_None: buffer.append("None"); break;
            case KEY_Backspace: buffer.append("Backspace"); break;
            case KEY_Tab: buffer.append("Tab"); break;
            case KEY_Clear: buffer.append("Clear"); break;
            case KEY_Enter: buffer.append("Enter"); break;
            case KEY_Shift: buffer.append(n.SHIFT); break;
            case KEY_Control: buffer.append(n.CTRL); break;
            case KEY_Alt: buffer.append(n.ALT); break;
            case KEY_Pause: buffer.append("Pause"); break;
            case KEY_CapsLock: buffer.append("Caps Lock"); break;
            case KEY_Escape: buffer.append("Escape"); break;
            case KEY_Space: buffer.append("Space"); break;
            case KEY_PageUp: buffer.append("Page Up"); break;
            case KEY_PageDown: buffer.append("Page Down"); break;
            case KEY_End: buffer.append("End"); break;
            case KEY_Home: buffer.append("Home"); break;
            case KEY_Left: buffer.append("Left"); break;
            case KEY_Up: buffer.append("Up"); break;
            case KEY_Right: buffer.append("Right"); break;
            case KEY_Down: buffer.append("Down"); break;
            case KEY_Select: buffer.append("Select"); break;
            case KEY_Print: buffer.append("Print"); break;
            case KEY_Execute: buffer.append("Execute"); break;
            case KEY_PrintScreen: buffer.append("Print Screen"); break;
            case KEY_Insert: buffer.append("Insert"); break;
            case KEY_Delete: buffer.append("Delete"); break;
            case KEY_Help: buffer.append("Help"); break;
            case KEY_LeftWindowsKey: buffer.append(n.SUPER); break;
            case KEY_RightWindowsKey: buffer.append("R{}", n.SUPER); break;
            case KEY_ApplicationsKey: buffer.append("Menu"); break;
            case KEY_Sleep: buffer.append("Sleep"); break;
            case KEY_Multiply: buffer.append("Num *"); break;
            case KEY_Add: buffer.append("Num +"); break;
            case KEY_Seperator: buffer.append("Num ,"); break;
            case KEY_Subtract: buffer.append("Num -"); break;
            case KEY_Decimal: buffer.append("Num ."); break;
            case KEY_Divide: buffer.append("Num /"); break;
            case KEY_Numlock: buffer.append("Num Lock"); break;
            case KEY_ScrollLock: buffer.append("Scroll Lock"); break;
            case KEY_LeftShift: buffer.append(n.SHIFT); break;
            case KEY_RightShift: buffer.append("R{}", n.SHIFT); break;
            case KEY_LeftControl: buffer.append(n.CTRL); break;
            case KEY_RightControl: buffer.append("R{}", n.CTRL); break;
            case KEY_LeftMenu: buffer.append(n.ALT); break;
            case KEY_RightMenu: buffer.append("R{}", n.ALT); break;
            case KEY_BrowserBack: buffer.append("Browser Back"); break;
            case KEY_BrowserForward: buffer.append("Browser Forward"); break;
            case KEY_BrowserRefresh: buffer.append("Browser Refresh"); break;
            case KEY_BrowserStop: buffer.append("Browser Stop"); break;
            case KEY_BrowserSearch: buffer.append("Browser Search"); break;
            case KEY_BrowserFavorites: buffer.append("Browser Favorites"); break;
            case KEY_BrowserHome: buffer.append("Browser Home"); break;
            case KEY_VolumeMute: buffer.append("Volume Mute"); break;
            case KEY_VolumeDown: buffer.append("Volume Down"); break;
            case KEY_VolumeUp: buffer.append("Volume Up"); break;
            case KEY_NextTrack: buffer.append("Next Track"); break;
            case KEY_PreviousTrack: buffer.append("Previous Track"); break;
            case KEY_StopMedia: buffer.append("Stop Media"); break;
            case KEY_PlayPause: buffer.append("Play/Pause"); break;
            case KEY_LaunchMail: buffer.append("Launch Mail"); break;
            case KEY_SelectMedia: buffer.append("Select Media"); break;
            case KEY_LaunchApp1: buffer.append("Launch App 1"); break;
            case KEY_LaunchApp2: buffer.append("Launch App 2"); break;
            case KEY_OEM1: buffer.append("OEM 1"); break;
            case KEY_OEMPlus: buffer.append("OEM Plus"); break;
            case KEY_OEMComma: buffer.append("OEM Comma"); break;
            case KEY_OEMMinus: buffer.append("OEM Minus"); break;
            case KEY_OEMPeriod: buffer.append("OEM Period"); break;
            case KEY_OEM2: buffer.append("OEM 2"); break;
            case KEY_OEM3: buffer.append("OEM 3"); break;
            case KEY_OEM4: buffer.append("OEM 4"); break;
            case KEY_OEM5: buffer.append("OEM 5"); break;
            case KEY_OEM6: buffer.append("OEM 6"); break;
            case KEY_OEM7: buffer.append("OEM 7"); break;
            case KEY_OEM8: buffer.append("OEM 8"); break;
            case KEY_OEM102: buffer.append("OEM 102"); break;
            case KEY_Process: buffer.append("Process"); break;
            case KEY_Packet: buffer.append("Packet"); break;
            case KEY_Attn: buffer.append("Attn"); break;
            case KEY_CrSel: buffer.append("CrSel"); break;
            case KEY_ExSel: buffer.append("ExSel"); break;
            case KEY_EraseEOF: buffer.append("Erase EOF"); break;
            case KEY_Play: buffer.append("Play"); break;
            case KEY_Zoom: buffer.append("Zoom"); break;
            case KEY_PA1: buffer.append("PA1"); break;
            case KEY_OEMClear: buffer.append("OEM Clear"); break;
            case KEY_ArrowUp: buffer.append("Up"); break;
            case KEY_ArrowDown: buffer.append("Down"); break;
            case KEY_ArrowLeft: buffer.append("Left"); break;
            case KEY_ArrowRight: buffer.append("Right"); break;
            case CONTROLLER_A: buffer.append("Gamepad A"); break;
            case CONTROLLER_B: buffer.append("Gamepad B"); break;
            case CONTROLLER_Y: buffer.append("Gamepad Y"); break;
            case CONTROLLER_X: buffer.append("Gamepad X"); break;
            case CONTROLLER_Start: buffer.append("Gamepad Start"); break;
            case CONTROLLER_Back: buffer.append("Gamepad Back"); break;
            case CONTROLLER_RB: buffer.append("Gamepad RB"); break;
            case CONTROLLER_LB: buffer.append("Gamepad LB"); break;
            case CONTROLLER_RT: buffer.append("Gamepad RT"); break;
            case CONTROLLER_LT: buffer.append("Gamepad LT"); break;
            case CONTROLLER_Up: buffer.append("Gamepad Up"); break;
            case CONTROLLER_Down: buffer.append("Gamepad Down"); break;
            case CONTROLLER_Left: buffer.append("Gamepad Left"); break;
            case CONTROLLER_Right: buffer.append("Gamepad Right"); break;
            case CONTROLLER_LTHUMBSTICK_UP: buffer.append("Gamepad LS Up"); break;
            case CONTROLLER_LTHUMBSTICK_DOWN: buffer.append("Gamepad LS Down"); break;
            case CONTROLLER_LTHUMBSTICK_LEFT: buffer.append("Gamepad LS Left"); break;
            case CONTROLLER_LTHUMBSTICK_RIGHT: buffer.append("Gamepad LS Right"); break;
            case CONTROLLER_RTHUMBSTICK_UP: buffer.append("Gamepad RS Up"); break;
            case CONTROLLER_RTHUMBSTICK_DOWN: buffer.append("Gamepad RS Down"); break;
            case CONTROLLER_RTHUMBSTICK_LEFT: buffer.append("Gamepad RS Left"); break;
            case CONTROLLER_RTHUMBSTICK_RIGHT: buffer.append("Gamepad RS Right"); break;
            case KEY_GraveAccent: buffer.append("`"); break;
            case KEY_OEMEqual: buffer.append("="); break;
            case KEY_LeftBracket: buffer.append("["); break;
            case KEY_RightBracket: buffer.append("]"); break;
            case KEY_Backslash: buffer.append("\\"); break;
            case KEY_Semicolon: buffer.append(";"); break;
            case KEY_Apostrophe: buffer.append("'"); break;
            case KEY_Slash: buffer.append("/"); break;
            case KEY_Equal: buffer.append("="); break;
            case KEY_NumEnter: buffer.append("Num Enter"); break;
            case KEY_World1: buffer.append("World 1"); break;
            case KEY_World2: buffer.append("World 2"); break;
            case MOUSE_4: buffer.append("Mouse 4"); break;
            case MOUSE_5: buffer.append("Mouse 5"); break;
            case MOUSE_6: buffer.append("Mouse 6"); break;
            case MOUSE_7: buffer.append("Mouse 7"); break;
            case MOUSE_8: buffer.append("Mouse 8"); break;

            default: buffer.append("Unknown({})", static_cast<int32_t>(key)); break;
        }
    }

    std::string Keybind::toString() const {
        if (!this->isBound()) return "<unbound>";

        utils::StringBuffer<64> buffer;

        modsToString(buffer, mods, key);
        keyToString(buffer, key);

        return buffer.str();
    }

    InputManager* InputManager::get() {
        static InputManager instance;
        return &instance;
    }

    void InputManager::registerListener(std::string id, Binding::Action callback, bool ephemeral) {
        std::string idCopy = id;
        auto [it, _] = m_bindings.insert_or_assign(
            std::move(idCopy),
            std::make_unique<Binding>(std::move(id), std::move(callback))
        );

        it->second->setEphemeral(ephemeral);

        auto loadedIt = m_loadedKeybinds.find(it->first);
        if (loadedIt != m_loadedKeybinds.end()) {
            it->second->setKeybind(loadedIt->second.keybind);
            it->second->setMode(loadedIt->second.mode);
        }
    }

    bool InputManager::onKeyEvent(enumKeyCodes key, KeyboardModifier mods, bool isDown, double timestamp) {
        if (m_globalListener && m_globalListener(key, mods, isDown)) {
            return true;
        }

        for (auto& binding : m_bindings | std::views::values) {
            if (binding->invoke(isDown, key, mods, timestamp)) {
                return true;
            }
        }

        return false;
    }

    void InputManager::setGlobalListener(GlobalListener listener) {
        m_globalListener = std::move(listener);
    }

    void InputManager::setDefaultKeybind(std::string id, Keybind keybind, KeybindMode mode) {
        if (!m_loadedKeybinds.contains(id)) {
            m_loadedKeybinds.insert_or_assign(std::move(id), SavedKeybind{ keybind, mode });
        }
    }

    InputManager::InputManager() {
        geode::KeyboardInputEvent()
            .listen([this](geode::KeyboardInputData& data) {
                if (data.action == geode::KeyboardInputData::Action::Repeat) return false;
                return this->onKeyEvent(
                    data.key,
                    data.modifiers,
                    data.action == geode::KeyboardInputData::Action::Press,
                    data.timestamp
                );
            })
            .leak();

        geode::ModStateEvent(geode::ModEventType::DataSaved, Mod::get())
            .listen([this] {
                matjson::Value toSave;
                for (auto& [id, binding] : m_bindings) {
                    if (!binding->keybind().isBound()) continue;
                    toSave.set(id, SavedKeybind{ binding->keybind(), binding->mode() });
                }

                auto str = toSave.dump();
                auto res = utils::file::writeStringSafe(
                    Mod::get()->getSaveDir() / "keybinds.json",
                    str
                );

                if (res.isErr()) {
                    log::error("Failed to save keybinds: {}", res.unwrapErr());
                }
            })
            .leak();

        auto res = utils::file::readFromJson<StringMap<SavedKeybind>>(
            Mod::get()->getSaveDir() / "keybinds.json"
        );

        if (res.isErr()) {
            log::error("Failed to load keybinds: {}", res.unwrapErr());
            return;
        }

        m_loadedKeybinds = std::move(res).unwrap();
    }
}
