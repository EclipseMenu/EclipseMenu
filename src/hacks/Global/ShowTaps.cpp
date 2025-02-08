#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/labels/variables.hpp>
#include <vector>

#if !defined(GEODE_IS_DESKTOP) || defined(ECLIPSE_DEBUG_BUILD)

#include <Geode/modify/CCTouchDispatcher.hpp>

namespace eclipse::hacks::Global {

    struct Touch {
        cocos2d::CCSprite* node{};
        cocos2d::CCTouch* touch{};
    };

    static std::vector<Touch> touchNodes;
    class $hack(ShowTaps) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");
            tab->addToggle("global.show-taps")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Show Taps"; }
    };

    REGISTER_HACK(ShowTaps)

    class $modify(ShowTapsTouchHook, cocos2d::CCTouchDispatcher) {
        ADD_HOOKS_DELEGATE("global.show-taps")

        void touches(cocos2d::CCSet* touches, cocos2d::CCEvent* event, unsigned int touchType) {
            if (auto scene = cocos2d::CCScene::get()) {
                auto touch = static_cast<cocos2d::CCTouch*>(touches->anyObject());
                switch (touchType) {
                    case cocos2d::CCTOUCHBEGAN: {
                        auto circle = cocos2d::CCSprite::createWithSpriteFrameName("circle.png"_spr);
                        circle->setScale(0.3F);
                        circle->setOpacity(175);
                        circle->setPosition(touch->getLocation());
                        circle->setZOrder(scene->getHighestChildZ() + 1);
                        touchNodes.push_back({circle, touch});
                        scene->addChild(circle);
                    } break;

                    case cocos2d::CCTOUCHENDED: { // case cocos2d::CCTOUCHCANCELLED:
                        for (auto& circle : touchNodes) {
                            auto it = std::ranges::find_if(
                                touchNodes, [&touch](Touch circle) {
                                    return circle.touch == touch;
                                }
                            );
                            if (it == touchNodes.end()) continue;
                            (*it).node->removeFromParentAndCleanup(true);
                            touchNodes.erase(it); // TODO: will this really free memory or leave a dangling pointer? 
                        }
                    } break;
                    /*case cocos2d::CCTOUCHMOVED: {

                    } break;*/
                    default: break;
                }
            }
            return cocos2d::CCTouchDispatcher::touches(touches, event, touchType);
        }
    };
}

#endif
