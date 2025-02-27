#include <Geode/platform/platform.hpp>
#ifdef GEODE_IS_ANDROID
#include <modules/keybinds/manager.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/CCKeyboardDispatcher.hpp>

namespace eclipse::keybinds {
    Keys convertCocosKey(cocos2d::enumKeyCodes key) {
        switch (key) {
            case KEY_A: return Keys::A;
            case KEY_B: return Keys::B;
            case KEY_C: return Keys::C;
            case KEY_D: return Keys::D;
            case KEY_E: return Keys::E;
            case KEY_F: return Keys::F;
            case KEY_G: return Keys::G;
            case KEY_H: return Keys::H;
            case KEY_I: return Keys::I;
            case KEY_J: return Keys::J;
            case KEY_K: return Keys::K;
            case KEY_L: return Keys::L;
            case KEY_M: return Keys::M;
            case KEY_N: return Keys::N;
            case KEY_O: return Keys::O;
            case KEY_P: return Keys::P;
            case KEY_Q: return Keys::Q;
            case KEY_R: return Keys::R;
            case KEY_S: return Keys::S;
            case KEY_T: return Keys::T;
            case KEY_U: return Keys::U;
            case KEY_V: return Keys::V;
            case KEY_W: return Keys::W;
            case KEY_X: return Keys::X;
            case KEY_Y: return Keys::Y;
            case KEY_Z: return Keys::Z;
            case KEY_Space: return Keys::Space;
            //case KEY_Apostrophe: return Keys::Apostrophe;
            case KEY_OEMComma: return Keys::Comma;
            case KEY_OEMMinus: return Keys::Minus;
            case KEY_OEMPeriod: return Keys::Period;
            //case KEY_Slash: return Keys::Slash;
            //case KEY_Semicolon: return Keys::Semicolon;
            //case KEY_Equal: return Keys::Equal;
            //case KEY_LeftBracket: return Keys::LeftBracket;
            //case KEY_BACKSLASH: return Keys::Backslash;
            //case KEY_RIGHT_BRACKET: return Keys::RightBracket;
            //case KEY_GRAVE_ACCENT: return Keys::GraveAccent;
            //case KEY_WORLD_1: return Keys::World1;
            //case KEY_WORLD_2: return Keys::World2;
            case KEY_Zero: return Keys::Num0;
            case KEY_One: return Keys::Num1;
            case KEY_Two: return Keys::Num2;
            case KEY_Three: return Keys::Num3;
            case KEY_Four: return Keys::Num4;
            case KEY_Five: return Keys::Num5;
            case KEY_Six: return Keys::Num6;
            case KEY_Seven: return Keys::Num7;
            case KEY_Eight: return Keys::Num8;
            case KEY_Nine: return Keys::Num9;
            case KEY_F1: return Keys::F1;
            case KEY_F2: return Keys::F2;
            case KEY_F3: return Keys::F3;
            case KEY_F4: return Keys::F4;
            case KEY_F5: return Keys::F5;
            case KEY_F6: return Keys::F6;
            case KEY_F7: return Keys::F7;
            case KEY_F8: return Keys::F8;
            case KEY_F9: return Keys::F9;
            case KEY_F10: return Keys::F10;
            case KEY_F11: return Keys::F11;
            case KEY_F12: return Keys::F12;
            case KEY_F13: return Keys::F13;
            case KEY_F14: return Keys::F14;
            case KEY_F15: return Keys::F15;
            case KEY_F16: return Keys::F16;
            case KEY_F17: return Keys::F17;
            case KEY_F18: return Keys::F18;
            case KEY_F19: return Keys::F19;
            case KEY_F20: return Keys::F20;
            case KEY_F21: return Keys::F21;
            case KEY_F22: return Keys::F22;
            case KEY_F23: return Keys::F23;
            case KEY_F24: return Keys::F24;
            //case KEY_F25: return Keys::F25;
            case KEY_NumPad0: return Keys::NumPad0;
            case KEY_NumPad1: return Keys::NumPad1;
            case KEY_NumPad2: return Keys::NumPad2;
            case KEY_NumPad3: return Keys::NumPad3;
            case KEY_NumPad4: return Keys::NumPad4;
            case KEY_NumPad5: return Keys::NumPad5;
            case KEY_NumPad6: return Keys::NumPad6;
            case KEY_NumPad7: return Keys::NumPad7;
            case KEY_NumPad8: return Keys::NumPad8;
            case KEY_NumPad9: return Keys::NumPad9;
            case KEY_Decimal: return Keys::NumPadDecimal;
            case KEY_Divide: return Keys::NumPadDivide;
            case KEY_Multiply: return Keys::NumPadMultiply;
            case KEY_Subtract: return Keys::NumPadSubtract;
            case KEY_Add: return Keys::NumPadAdd;
            //case KEY_KP_ENTER: return Keys::NumPadEnter;
            //case KEY_Equal: return Keys::NumPadEqual;
            //case KEY_Menu: return Keys::Menu;
            case KEY_Escape: return Keys::Escape;
            case KEY_Enter: return Keys::Enter;
            case KEY_Tab: return Keys::Tab;
            case KEY_Backspace: return Keys::Backspace;
            case KEY_Insert: return Keys::Insert;
            case KEY_Delete: return Keys::Delete;
            case KEY_Home: return Keys::Home;
            case KEY_End: return Keys::End;
            case KEY_PageUp: return Keys::PageUp;
            case KEY_PageDown: return Keys::PageDown;
            case KEY_CapsLock: return Keys::CapsLock;
            case KEY_ScrollLock: return Keys::ScrollLock;
            case KEY_Numlock: return Keys::NumLock;
            case KEY_PrintScreen: return Keys::PrintScreen;
            case KEY_Pause: return Keys::Pause;
            case KEY_ArrowUp: return Keys::Up;
            case KEY_ArrowDown: return Keys::Down;
            case KEY_ArrowLeft: return Keys::Left;
            case KEY_ArrowRight: return Keys::Right;
            case KEY_LeftShift: return Keys::LeftShift;
            case KEY_LeftControl: return Keys::LeftControl;
            //case KEY_LeftAlt: return Keys::LeftAlt;
            //case KEY_LeftSuper: return Keys::LeftSuper;
            case KEY_RightShift: return Keys::RightShift;
            case KEY_RightContol: return Keys::RightControl;
            //case KEY_RightAlt: return Keys::RightAlt;
            //case KEY_RightSuper: return Keys::RightSuper;
            default: return Keys::Unknown;
        }
    }

    class $modify(KeybindingsManagerCCKDHook, cocos2d::CCKeyboardDispatcher) {
        ENABLE_FIRST_HOOKS_ALL()

        bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool down, bool repeat) {
            if (!repeat) {
                if (down) Manager::get()->registerKeyPress(convertCocosKey(key));
                else if (!down) Manager::get()->registerKeyRelease(convertCocosKey(key));
            }

            return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat);
        }
    };
}
#endif