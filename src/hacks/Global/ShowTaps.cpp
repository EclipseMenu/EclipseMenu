#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/labels/variables.hpp>
#include <vector>
#include <modules/utils/vmthooker.hpp>

#if defined(GEODE_IS_MOBILE) || defined(ECLIPSE_DEBUG_BUILD)

#include <Geode/modify/CCTouchDispatcher.hpp>

namespace eclipse::hacks::Global {
    static std::vector<cocos2d::CCTouch*> touchNodes;

    void ShowTapsCCSceneVisitHook(auto& original, cocos2d::CCScene* self) {
        original(self);
        if (touchNodes.empty()) return;

        cocos2d::ccDrawColor4B(255, 255, 255, 100);
        cocos2d::ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (auto touch : touchNodes) {
            cocos2d::ccDrawFilledCircle(touch->getLocation(), 8, 0, 16);
        }
    }

    class $hack(ShowTaps) {
        static void activateHook() {
            utils::VMTHooker<cocos2d::CCScene, void, cocos2d::CCNode>::get(&cocos2d::CCNode::visit)
                .toggleHook(ShowTapsCCSceneVisitHook, config::get("global.show-taps", false));
        }

        void init() override {
            auto tab = gui::MenuTab::find("tab.global");
            tab->addToggle("global.show-taps")->setDescription()->handleKeybinds();

            config::addDelegate("global.show-taps", activateHook);
            activateHook();
        }

        [[nodiscard]] const char* getId() const override { return "Show Taps"; }
    };

    REGISTER_HACK(ShowTaps)

    class $modify(ShowTapsTouchHook, cocos2d::CCTouchDispatcher) {
        ADD_HOOKS_DELEGATE("global.show-taps")

        void touches(cocos2d::CCSet* touches, cocos2d::CCEvent* event, unsigned int touchType) {
            auto touch = static_cast<cocos2d::CCTouch*>(touches->anyObject());
            switch (touchType) {
                case cocos2d::CCTOUCHBEGAN: {
                    touchNodes.push_back(touch);
                } break;
                case cocos2d::CCTOUCHENDED: {
                    std::erase(touchNodes, touch);
                } break;
                default: break;
            }
            return CCTouchDispatcher::touches(touches, event, touchType);
        }
    };
}

#endif
