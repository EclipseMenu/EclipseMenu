#ifdef __APPLE__
#define CommentType CommentTypeDummy
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#undef CommentType
#endif

#include <Geode/cocos/platform/mac/CCEventDispatcher.h>
#import <Geode/cocos/platform/mac/EAGLView.h>
#import <objc/runtime.h>
#include <modules/keybinds/manager.hpp>

using namespace geode::prelude;

using EventType = void(*)(EAGLView*, SEL, NSEvent*);

namespace eclipse::keybinds {
    // i swear, person who assigned these keycodes was drunk
    Keys convertMacKey(int keycode) {
        switch (keycode) {
            case 0: return Keys::A;
            case 1: return Keys::S;
            case 2: return Keys::D;
            case 3: return Keys::F;
            case 4: return Keys::H;
            case 5: return Keys::G;
            case 6: return Keys::Z;
            case 7: return Keys::X;
            case 8: return Keys::C;
            case 9: return Keys::V;
            // case 10: return Keys::Unknown; // § key
            case 11: return Keys::B;
            case 12: return Keys::Q;
            case 13: return Keys::W;
            case 14: return Keys::E;
            case 15: return Keys::R;
            case 16: return Keys::Y;
            case 17: return Keys::T;
            case 18: return Keys::Num1;
            case 19: return Keys::Num2;
            case 20: return Keys::Num3;
            case 21: return Keys::Num4;
            case 22: return Keys::Num6;
            case 23: return Keys::Num5;
            case 24: return Keys::Equal;
            case 25: return Keys::Num9;
            case 26: return Keys::Num7;
            case 27: return Keys::Minus;
            case 28: return Keys::Num8;
            case 29: return Keys::Num0;
            case 30: return Keys::LeftBracket;
            case 31: return Keys::O;
            case 32: return Keys::U;
            case 33: return Keys::RightBracket;
            case 34: return Keys::I;
            case 35: return Keys::P;
            case 36: return Keys::Enter;
            case 37: return Keys::L;
            case 38: return Keys::J;
            case 39: return Keys::Apostrophe;
            case 40: return Keys::K;
            case 41: return Keys::Semicolon;
            case 42: return Keys::Backslash;
            case 43: return Keys::Comma;
            case 44: return Keys::Slash;
            case 45: return Keys::N;
            case 46: return Keys::M;
            case 47: return Keys::Period;
            case 48: return Keys::Tab;
            case 49: return Keys::Space;
            case 50: return Keys::GraveAccent;
            case 51: return Keys::Backspace;
            // case 52: return Keys::Unknown; // line feed key
            case 53: return Keys::Escape;
            case 54: return Keys::RightSuper;
            case 55: return Keys::LeftSuper;
            case 56: return Keys::LeftShift;
            case 57: return Keys::CapsLock;
            case 58: return Keys::LeftAlt;
            case 59: return Keys::LeftControl;
            case 60: return Keys::RightShift;
            case 61: return Keys::RightAlt;
            case 62: return Keys::RightControl;
            // case 63: return Keys::Unknown; // fn key
            case 64: return Keys::F17;
            case 65: return Keys::NumPadDecimal;
            // case 66: return Keys::Unknown; // unasigned
            case 67: return Keys::NumPadMultiply;
            // case 68: return Keys::Unknown; // unasigned
            case 69: return Keys::NumPadAdd;
            // case 70: return Keys::Unknown; // unasigned
            // case 71: return Keys::Unknown; // keypad clear key (?)
            // case 72: return Keys::Unknown; // volume up key
            // case 73: return Keys::Unknown; // volume down key
            // case 74: return Keys::Unknown; // mute key
            case 75: return Keys::NumPadDivide;
            case 76: return Keys::NumPadEnter;
            // case 77: return Keys::Unknown; // unasigned
            case 78: return Keys::NumPadSubtract;
            case 79: return Keys::F18;
            case 80: return Keys::F19;
            case 81: return Keys::NumPadEqual;
            case 82: return Keys::NumPad0;
            case 83: return Keys::NumPad1;
            case 84: return Keys::NumPad2;
            case 85: return Keys::NumPad3;
            case 86: return Keys::NumPad4;
            case 87: return Keys::NumPad5;
            case 88: return Keys::NumPad6;
            case 89: return Keys::NumPad7;
            case 90: return Keys::F20;
            case 91: return Keys::NumPad8;
            case 92: return Keys::NumPad9;
            // case 93: return Keys::Unknown; // unasigned
            // case 94: return Keys::Unknown; // unasigned
            // case 95: return Keys::Unknown; // unasigned
            case 96: return Keys::F5;
            case 97: return Keys::F6;
            case 98: return Keys::F7;
            case 99: return Keys::F3;
            case 100: return Keys::F8;
            case 101: return Keys::F9;
            // case 102: return Keys::Unknown; // unasigned
            case 103: return Keys::F11;
            // case 104: return Keys::Unknown; // unasigned
            case 105: return Keys::F13;
            case 106: return Keys::F16;
            case 107: return Keys::F14;
            // case 108: return Keys::Unknown; // unasigned
            case 109: return Keys::F10;
            // case 110: return Keys::Unknown; // unasigned
            case 111: return Keys::F12;
            // case 112: return Keys::Unknown; // unasigned
            case 113: return Keys::F15;
            case 114: return Keys::Insert;
            case 115: return Keys::Home;
            case 116: return Keys::PageUp;
            case 117: return Keys::Delete;
            case 118: return Keys::F4;
            case 119: return Keys::End;
            case 120: return Keys::F2;
            case 121: return Keys::PageDown;
            case 122: return Keys::F1;
            case 123: return Keys::Left;
            case 124: return Keys::Right;
            case 125: return Keys::Down;
            case 126: return Keys::Up;
            default: return Keys::Unknown;
        }
    };

    static EventType originalKeyDown;
    static EventType originalKeyUp;

    void keyDownHook(EAGLView* self, SEL _cmd, NSEvent* event) {
        originalKeyDown(self, _cmd, event);

        if (event.isARepeat) return;
        Manager::get()->registerKeyPress(convertMacKey(event.keyCode));
    }

    void keyUpHook(EAGLView* self, SEL _cmd, NSEvent* event) {
        originalKeyUp(self, _cmd, event);

        if (event.isARepeat) return;
        Manager::get()->registerKeyRelease(convertMacKey(event.keyCode));
    }

    #define OBJC_SWIZZLE(method, swizzle, original) do { \
        auto method##Method = class_getInstanceMethod(class_, @selector(method:)); \
        original = reinterpret_cast<decltype(original)>(method_getImplementation(method##Method)); \
        method_setImplementation(method##Method, reinterpret_cast<IMP>(swizzle)); \
    } while(0)

    $execute {
        Class class_ = NSClassFromString(@"EAGLView");
        OBJC_SWIZZLE(keyDown, keyDownHook, originalKeyDown);
        OBJC_SWIZZLE(keyUp, keyUpHook, originalKeyUp);
    }
}

