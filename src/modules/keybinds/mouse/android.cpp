#include <Geode/Geode.hpp>

#ifdef GEODE_IS_ANDROID
#include <modules/keybinds/manager.hpp>

#include <Geode/modify/CCTouchDispatcher.hpp>

namespace eclipse::keybinds {

    class $modify(cocos2d::CCTouchDispatcher) {
        void touches(cocos2d::CCSet* touches, cocos2d::CCEvent* event, unsigned int type) {
            auto* manager = Manager::get();
            if (type == cocos2d::CCTOUCHBEGAN) {
                manager->registerKeyPress(Keys::MouseLeft);
            } else if (type == cocos2d::CCTOUCHENDED) {
                manager->registerKeyRelease(Keys::MouseLeft);
            }

            cocos2d::CCTouchDispatcher::touches(touches, event, type);
        }
    };

}

#endif