#include <modules/config/config.hpp>
#include <modules/gui/color.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/labels/variables.hpp>
#include <vector>

#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>

namespace eclipse::hacks::Global {
    static std::vector<geode::WeakRef<cocos2d::CCTouch>> touchNodes;

    void renderTaps() {
        if (touchNodes.empty()) return;

        bool fill = config::get<bool>("global.show-taps.fill", true);
        int radius = (config::get<float>("global.show-taps.scale", 0.5F) * 16.F);
        int segments = (config::get<float>("global.show-taps.scale", 0.5F) * 32.F);
        auto color = config::get<gui::Color>("global.show-taps.color", gui::Color::WHITE).toCCColor3B();

        int stroke = config::get<int>("global.show-taps.stroke", 0);

        /*if (!fill) { // this definitely is a good idea
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        }*/
        cocos2d::ccDrawColor4B(color.r, color.g, color.b, 255.F * ((float)config::get<int>("global.show-taps.opacity", 50) / 100.F));
        cocos2d::ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (size_t i = 0; i < touchNodes.size();) {
            if (auto touch = touchNodes[i].lock()) {
                if (fill) {
                    cocos2d::ccDrawFilledCircle(touch->getLocation(), radius, 0, segments);
                } else {
                    cocos2d::ccDrawCircle(touch->getLocation(), radius, 360, segments, false);
                    if (stroke > 0) {
                        for (size_t ix = 0; ix < (stroke * 5); ix++) {
                            cocos2d::ccDrawCircle(touch->getLocation(), radius + (ix / 10.F), 360, segments, false);
                        }
                    }
                }
                i++;
            } else {
                std::swap(touchNodes[i], touchNodes.back());
                touchNodes.pop_back();
            }
        }
    }

    class $hack(ShowTaps) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");
            config::setIfEmpty("global.show-taps.fill", true);
            config::setIfEmpty("global.show-taps.scale", 0.5F);
            config::setIfEmpty("global.show-taps.opacity", 50);
            config::setIfEmpty("global.show-taps.stroke", 0);
            config::setIfEmpty("global.show-taps.color", gui::Color::WHITE);

            tab->addToggle("global.show-taps")->setDescription()->handleKeybinds()->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                options->addToggle("global.show-taps.fill")->setDescription();
                options->addInputFloat("global.show-taps.scale", 0.01f, 5.f, "%.2f");
                options->addInputInt("global.show-taps.stroke", 0, 10);
                options->addInputInt("global.show-taps.opacity", 1, 100);
                options->addColorComponent("global.show-taps.color");
            });

            config::addDelegate("global.show-taps", []() {
                touchNodes.clear();
            });
        }

        [[nodiscard]] const char* getId() const override { return "Show Taps"; }
    };

    REGISTER_HACK(ShowTaps)

    #if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_IOS)
    class $modify(ShowTapsCCEGLVHook, cocos2d::CCEGLView) {
        ADD_HOOKS_DELEGATE("global.show-taps")
        void swapBuffers() override {
            renderTaps();
            CCEGLView::swapBuffers();
        }
    };
    #else
    class $modify(ShowTapsCCDHook, cocos2d::CCDirector) {
        ADD_HOOKS_DELEGATE("global.show-taps")
        void drawScene() {
            CCDirector::drawScene();
            renderTaps();
        }
    };
    #endif

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
