#include <Geode/Geode.hpp>

#ifdef GEODE_IS_ANDROID
#include <modules/keybinds/manager.hpp>

#include <Geode/modify/CCTouchDispatcher.hpp>

namespace eclipse::keybinds {

    class $modify(cocos2d::CCTouchDispatcher) {
        void touches(CCSet* touches, CCEvent* event, unsigned int type) {
            if (!touches) return cocos2d::CCTouchDispatcher::touches(touches, event, type);

            auto* manager = Manager::get();
            if (type == CCTOUCHBEGAN) {
                manager->registerKeyPress(Keys::MouseLeft);
            } else if (type == CCTOUCHENDED) {
                manager->registerKeyRelease(Keys::MouseLeft);
            }

            cocos2d::CCTouchDispatcher::touches(touches, event, type);
        }
    };

}

#endif