#include "manager.hpp"

#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

using namespace geode::prelude;

namespace eclipse::keybinds {

#ifdef GEODE_IS_WINDOWS
    Keys convertGlfwKey(int key) {
        switch (key) {
            case GLFW_KEY_A: return Keys::A;
            case GLFW_KEY_B: return Keys::B;
            case GLFW_KEY_C: return Keys::C;
            case GLFW_KEY_D: return Keys::D;
            case GLFW_KEY_E: return Keys::E;
            case GLFW_KEY_F: return Keys::F;
            case GLFW_KEY_G: return Keys::G;
            case GLFW_KEY_H: return Keys::H;
            case GLFW_KEY_I: return Keys::I;
            case GLFW_KEY_J: return Keys::J;
            case GLFW_KEY_K: return Keys::K;
            case GLFW_KEY_L: return Keys::L;
            case GLFW_KEY_M: return Keys::M;
            case GLFW_KEY_N: return Keys::N;
            case GLFW_KEY_O: return Keys::O;
            case GLFW_KEY_P: return Keys::P;
            case GLFW_KEY_Q: return Keys::Q;
            case GLFW_KEY_R: return Keys::R;
            case GLFW_KEY_S: return Keys::S;
            case GLFW_KEY_T: return Keys::T;
            case GLFW_KEY_U: return Keys::U;
            case GLFW_KEY_V: return Keys::V;
            case GLFW_KEY_W: return Keys::W;
            case GLFW_KEY_X: return Keys::X;
            case GLFW_KEY_Y: return Keys::Y;
            case GLFW_KEY_Z: return Keys::Z;
            case GLFW_KEY_SPACE: return Keys::Space;
            case GLFW_KEY_APOSTROPHE: return Keys::Apostrophe;
            case GLFW_KEY_COMMA: return Keys::Comma;
            case GLFW_KEY_MINUS: return Keys::Minus;
            case GLFW_KEY_PERIOD: return Keys::Period;
            case GLFW_KEY_SLASH: return Keys::Slash;
            case GLFW_KEY_SEMICOLON: return Keys::Semicolon;
            case GLFW_KEY_EQUAL: return Keys::Equal;
            case GLFW_KEY_LEFT_BRACKET: return Keys::LeftBracket;
            case GLFW_KEY_BACKSLASH: return Keys::Backslash;
            case GLFW_KEY_RIGHT_BRACKET: return Keys::RightBracket;
            case GLFW_KEY_GRAVE_ACCENT: return Keys::GraveAccent;
            case GLFW_KEY_WORLD_1: return Keys::World1;
            case GLFW_KEY_WORLD_2: return Keys::World2;
            case GLFW_KEY_0: return Keys::Num0;
            case GLFW_KEY_1: return Keys::Num1;
            case GLFW_KEY_2: return Keys::Num2;
            case GLFW_KEY_3: return Keys::Num3;
            case GLFW_KEY_4: return Keys::Num4;
            case GLFW_KEY_5: return Keys::Num5;
            case GLFW_KEY_6: return Keys::Num6;
            case GLFW_KEY_7: return Keys::Num7;
            case GLFW_KEY_8: return Keys::Num8;
            case GLFW_KEY_9: return Keys::Num9;
            case GLFW_KEY_F1: return Keys::F1;
            case GLFW_KEY_F2: return Keys::F2;
            case GLFW_KEY_F3: return Keys::F3;
            case GLFW_KEY_F4: return Keys::F4;
            case GLFW_KEY_F5: return Keys::F5;
            case GLFW_KEY_F6: return Keys::F6;
            case GLFW_KEY_F7: return Keys::F7;
            case GLFW_KEY_F8: return Keys::F8;
            case GLFW_KEY_F9: return Keys::F9;
            case GLFW_KEY_F10: return Keys::F10;
            case GLFW_KEY_F11: return Keys::F11;
            case GLFW_KEY_F12: return Keys::F12;
            case GLFW_KEY_F13: return Keys::F13;
            case GLFW_KEY_F14: return Keys::F14;
            case GLFW_KEY_F15: return Keys::F15;
            case GLFW_KEY_F16: return Keys::F16;
            case GLFW_KEY_F17: return Keys::F17;
            case GLFW_KEY_F18: return Keys::F18;
            case GLFW_KEY_F19: return Keys::F19;
            case GLFW_KEY_F20: return Keys::F20;
            case GLFW_KEY_F21: return Keys::F21;
            case GLFW_KEY_F22: return Keys::F22;
            case GLFW_KEY_F23: return Keys::F23;
            case GLFW_KEY_F24: return Keys::F24;
            case GLFW_KEY_F25: return Keys::F25;
            case GLFW_KEY_KP_0: return Keys::NumPad0;
            case GLFW_KEY_KP_1: return Keys::NumPad1;
            case GLFW_KEY_KP_2: return Keys::NumPad2;
            case GLFW_KEY_KP_3: return Keys::NumPad3;
            case GLFW_KEY_KP_4: return Keys::NumPad4;
            case GLFW_KEY_KP_5: return Keys::NumPad5;
            case GLFW_KEY_KP_6: return Keys::NumPad6;
            case GLFW_KEY_KP_7: return Keys::NumPad7;
            case GLFW_KEY_KP_8: return Keys::NumPad8;
            case GLFW_KEY_KP_9: return Keys::NumPad9;
            case GLFW_KEY_KP_DECIMAL: return Keys::NumPadDecimal;
            case GLFW_KEY_KP_DIVIDE: return Keys::NumPadDivide;
            case GLFW_KEY_KP_MULTIPLY: return Keys::NumPadMultiply;
            case GLFW_KEY_KP_SUBTRACT: return Keys::NumPadSubtract;
            case GLFW_KEY_KP_ADD: return Keys::NumPadAdd;
            case GLFW_KEY_KP_ENTER: return Keys::NumPadEnter;
            case GLFW_KEY_KP_EQUAL: return Keys::NumPadEqual;
            case GLFW_KEY_MENU: return Keys::Menu;
            case GLFW_KEY_ESCAPE: return Keys::Escape;
            case GLFW_KEY_ENTER: return Keys::Enter;
            case GLFW_KEY_TAB: return Keys::Tab;
            case GLFW_KEY_BACKSPACE: return Keys::Backspace;
            case GLFW_KEY_INSERT: return Keys::Insert;
            case GLFW_KEY_DELETE: return Keys::Delete;
            case GLFW_KEY_HOME: return Keys::Home;
            case GLFW_KEY_END: return Keys::End;
            case GLFW_KEY_PAGE_UP: return Keys::PageUp;
            case GLFW_KEY_PAGE_DOWN: return Keys::PageDown;
            case GLFW_KEY_CAPS_LOCK: return Keys::CapsLock;
            case GLFW_KEY_SCROLL_LOCK: return Keys::ScrollLock;
            case GLFW_KEY_NUM_LOCK: return Keys::NumLock;
            case GLFW_KEY_PRINT_SCREEN: return Keys::PrintScreen;
            case GLFW_KEY_PAUSE: return Keys::Pause;
            case GLFW_KEY_UP: return Keys::Up;
            case GLFW_KEY_DOWN: return Keys::Down;
            case GLFW_KEY_LEFT: return Keys::Left;
            case GLFW_KEY_RIGHT: return Keys::Right;
            case GLFW_KEY_LEFT_SHIFT: return Keys::LeftShift;
            case GLFW_KEY_LEFT_CONTROL: return Keys::LeftControl;
            case GLFW_KEY_LEFT_ALT: return Keys::LeftAlt;
            case GLFW_KEY_LEFT_SUPER: return Keys::LeftSuper;
            case GLFW_KEY_RIGHT_SHIFT: return Keys::RightShift;
            case GLFW_KEY_RIGHT_CONTROL: return Keys::RightControl;
            case GLFW_KEY_RIGHT_ALT: return Keys::RightAlt;
            case GLFW_KEY_RIGHT_SUPER: return Keys::RightSuper;
            default: return Keys::None;
        }
    }

    class $modify(cocos2d::CCEGLView) {
        void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            CCEGLView::onGLFWKeyCallback(window, key, scancode, action, mods);

            if (action == GLFW_PRESS) {
                Manager::get()->registerKeyPress(convertGlfwKey(key));
            } else if (action == GLFW_RELEASE) {
                Manager::get()->registerKeyRelease(convertGlfwKey(key));
            }
        }
    };
#else
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
            case KEY_Up: return Keys::Up;
            case KEY_Down: return Keys::Down;
            case KEY_Left: return Keys::Left;
            case KEY_Right: return Keys::Right;
            case KEY_LeftShift: return Keys::LeftShift;
            case KEY_LeftControl: return Keys::LeftControl;
            //case KEY_LeftAlt: return Keys::LeftAlt;
            //case KEY_LeftSuper: return Keys::LeftSuper;
            case KEY_RightShift: return Keys::RightShift;
            case KEY_RightContol: return Keys::RightControl;
            //case KEY_RightAlt: return Keys::RightAlt;
            //case KEY_RightSuper: return Keys::RightSuper;
            default: return Keys::None;
        }
    }

    class $modify(cocos2d::CCKeyboardDispatcher) {
        bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool down, bool idk) {
            if (down) {
                Manager::get()->registerKeyPress(convertCocosKey(key));
            } else if (!down) {
                Manager::get()->registerKeyRelease(convertCocosKey(key));
            }

            return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, idk);
        }
    };
#endif

    Manager* Manager::get() {
        static Manager s_manager;
        return &s_manager;
    }

    Keybind& Manager::registerKeybind(const std::string& id, const std::string& title, const std::function<void()>& callback) {
        // check if this id already exists
        for (auto& keybind : m_keybinds) {
            if (keybind.getId() == id) {
                return keybind;
            }
        }

        m_keybinds.emplace_back(Keys::None, id, title, callback);
        return m_keybinds.back();
    }

    void Manager::update() {
        for (auto& key: m_keyStates) {
            m_lastKeyStates.insert_or_assign(key.first, key.second);
        }
    }

    void Manager::setKeybindState(const std::string& id, bool state) {
        for (auto& keybind : m_keybinds) {
            if (keybind.getId() == id) {
                keybind.setInitialized(state);
            }
        }
    }

    void Manager::registerKeyPress(Keys key) {
        m_keyStates[key] = true;

        for (auto& keybind : m_keybinds) {
            if (keybind.getKey() == key && keybind.isInitialized()) {
                keybind.execute();
            }
        }
    }

    void Manager::registerKeyRelease(Keys key) {
        m_keyStates[key] = false;
    }

    bool isKeyDown(Keys key) {
        auto manager = Manager::get();
        return manager->m_keyStates[key];
    }

    bool isKeyPressed(Keys key) {
        auto manager = Manager::get();
        if (manager->m_keyStates[key] ^ manager->m_lastKeyStates[key]) {
            return manager->m_keyStates[key];
        }
        return false;
    }

    bool isKeyReleased(Keys key) {
        auto manager = Manager::get();
        if (manager->m_keyStates[key] ^ manager->m_lastKeyStates[key]) {
            return !manager->m_keyStates[key];
        }
        return false;
    }

}