#ifdef GEODE_IS_ANDROID
#include <modules/keybinds/manager.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/CCTouchDispatcher.hpp>

namespace eclipse::keybinds {

    class $modify(cocos2d::CCTouchDispatcher) {
        static void onModify(auto& self) {
            FIRST_PRIORITY("cocos2d::CCTouchDispatcher::touches");
        }

        void touches(cocos2d::CCSet* touches, cocos2d::CCEvent* event, unsigned int type) {
            if (!touches) return CCTouchDispatcher::touches(touches, event, type);
            auto* touch = static_cast<cocos2d::CCTouch*>(touches->anyObject());
            if (!touch) return CCTouchDispatcher::touches(touches, event, type);

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