#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/labels/variables.hpp>
#include <vector>

#if defined(GEODE_IS_MOBILE) || defined(ECLIPSE_DEBUG_BUILD)

#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>

namespace eclipse::hacks::Global {
    static std::vector<geode::WeakRef<cocos2d::CCTouch>> touchNodes;

    void renderTaps() {
        if (touchNodes.empty()) return;

        cocos2d::ccDrawColor4B(255, 255, 255, 100);
        cocos2d::ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (size_t i = 0; i < touchNodes.size();) {
            if (auto touch = touchNodes[i].lock()) {
                cocos2d::ccDrawFilledCircle(touch->getLocation(), 8, 0, 16);
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
            tab->addToggle("global.show-taps")->setDescription()->handleKeybinds();

            config::addDelegate("global.show-taps", []() {
                touchNodes.clear();
            });
        }

        [[nodiscard]] const char* getId() const override { return "Show Taps"; }
    };

    REGISTER_HACK(ShowTaps)

    #ifdef GEODE_IS_WINDOWS
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

#endif
