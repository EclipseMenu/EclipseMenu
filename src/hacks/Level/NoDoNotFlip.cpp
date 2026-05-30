#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(NoDoNotFlip) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.nodonotflip")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Do Not Flip"; }
    };

    REGISTER_HACK(NoDoNotFlip)

    class $modify(NoDoNotFlipPLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("level.nodonotflip")

		void setupHasCompleted() {
			PlayLayer::setupHasCompleted();

            if (this->m_doNot)
			    this->m_attemptLabel->setScaleY(1.0);
		}
    };

    class $modify(NoDoNotFlipELLHook, EndLevelLayer) {
        ADD_HOOKS_DELEGATE("level.nodonotflip")

        void customSetup() {
            EndLevelLayer::customSetup();

            if (this->m_playLayer->m_doNot) {
                cocos2d::CCSprite *levelCompleteText = this->m_mainLayer->getChildByType<cocos2d::CCSprite>(2);
                levelCompleteText->setFlipX(false);
            }
        }
    };
}
