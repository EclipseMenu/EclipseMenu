#ifdef GEODE_IS_WINDOWS
#include <modules/keybinds/manager.hpp>

#include <Geode/modify/CCEGLView.hpp>

namespace eclipse::keybinds {

    Keys convertMouseKey(int button) {
        switch(button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                return Keys::MouseLeft;
            case GLFW_MOUSE_BUTTON_RIGHT:
                return Keys::MouseRight;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                return Keys::MouseMiddle;
            case GLFW_MOUSE_BUTTON_4:
                return Keys::MouseButton4;
            case GLFW_MOUSE_BUTTON_5:
                return Keys::MouseButton5;
            default:
                return Keys::None;
        }
    }

    class $modify(MouseKeybindingsManagerCCEGLVHook, cocos2d::CCEGLView) {
        void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int mods) {
            cocos2d::CCEGLView::onGLFWMouseCallBack(window, button, action, mods);

            auto manager = Manager::get();
            auto key = convertMouseKey(button);

            if (action == GLFW_PRESS)
                manager->registerKeyPress(key);
            else if (action == GLFW_RELEASE)
                manager->registerKeyRelease(key);
        }
    };

}

#endif