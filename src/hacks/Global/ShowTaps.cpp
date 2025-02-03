#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/labels/variables.hpp>
#include <vector>

#ifndef GEODE_IS_DESKTOP

#include <Geode/modify/CCTouchDispatcher.hpp>

namespace eclipse::hacks::Global {
    static std::vector<cocos2d::CCSprite*> touchNodes;
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
                auto touch = touches->anyObject();
                if (touch) {
                    auto pos = static_cast<cocos2d::CCTouch*>(touch)->getLocation();
                    switch (touchType) {
                        case cocos2d::CCTOUCHBEGAN: {
                            auto circle = cocos2d::CCSprite::createWithSpriteFrameName("circle.png"_spr);
                            circle->setScale(0.3F);
                            circle->setOpacity(175);
                            circle->setPosition(pos);
                            circle->setZOrder(scene->getHighestChildZ() + 1);
                            touchNodes.push_back(circle);
                            scene->addChild(circle);
                        } break;

                        case cocos2d::CCTOUCHENDED: { // case cocos2d::CCTOUCHCANCELLED:
                            if (!touchNodes.empty()) {
                                auto circle = touchNodes.at(0);
                                if (circle->getPosition() == pos || touchNodes.size() == 1) {
                                    circle->removeFromParentAndCleanup(true);
                                    touchNodes.erase(touchNodes.begin());
                                }
                            }
                        } break;
                        /*case cocos2d::CCTOUCHMOVED: {

                        } break;*/
                        default: break;
                    }
                }
            }
            return cocos2d::CCTouchDispatcher::touches(touches, event, touchType);
        }
    };
}

#endif
