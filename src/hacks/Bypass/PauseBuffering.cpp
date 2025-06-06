#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/UILayer.hpp>

namespace eclipse::hacks::Bypass {
    class $hack(PauseBuffering) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");
            tab->addToggle("bypass.pausebuffering")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Pause Buffering"; }
    };

    REGISTER_HACK(PauseBuffering)

    class $modify(PauseBufferingUILHook, UILayer) {
        ADD_HOOKS_DELEGATE("bypass.pausebuffering")

        void onPause(cocos2d::CCObject* sender) {
            auto* playLayer = utils::get<PlayLayer>();
            if (!playLayer) return UILayer::onPause(sender);

            uint32_t original = playLayer->m_gameState.m_pauseCounter;
            playLayer->m_gameState.m_pauseCounter = 0;
            UILayer::onPause(sender);
            playLayer->m_gameState.m_pauseCounter = original;
        }
    };
}
