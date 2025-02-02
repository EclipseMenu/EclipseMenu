#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PauseLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(ConfirmPractice) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            config::setIfEmpty("level.confirmpractice", false);
            config::setIfEmpty("level.confirmpractice.confirmexitpractice", true);

            tab->addToggle("level.confirmpractice")->handleKeybinds()->setDescription()
               ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                   options->addToggle("level.confirmpractice.confirmexitpractice")->setDescription();
               });
        }

        [[nodiscard]] const char* getId() const override { return "Confirm Practice"; }
    };

    REGISTER_HACK(ConfirmPractice)

    class $modify(ConfirmPracticePauseLayerHook, PauseLayer) {	
        // ADD_HOOKS_DELEGATE("level.confirmpractice")

	    struct Fields {
                bool m_isEnterPopupVisible = false;
                bool m_isExitPopupVisible = false;
	    };

	    void onPracticeMode(cocos2d::CCObject* sender) {
            if (m_fields->m_isEnterPopupVisible) {
                PauseLayer::onPracticeMode(sender);
                return;
		    }

            geode::createQuickPopup(
                "Did I cook?",          // title
                "ahh commit",			// content
                "Nuh uh", "Yuh huh",    // buttons
                [this, sender](auto, bool btn2) {
                    if (btn2) {
                        m_fields->m_isEnterPopupVisible = true;
                        PauseLayer::onPracticeMode(sender);
                        m_fields->m_isEnterPopupVisible = false;
                    }
                }
            );
        }

        void onNormalMode(cocos2d::CCObject* sender) {
            if (m_fields->m_isExitPopupVisible || !config::get<bool>("level.confirmpractice.confirmexitpractice", false)) {
                PauseLayer::onNormalMode(sender);
                return;
            }

            geode::createQuickPopup(
                "Did I cook again?",    // title
                "ahh commit",			// content
                "Nuh uh", "Yuh huh",    // buttons
                [this, sender](auto, bool btn2) {
                    if (btn2) {
                        m_fields->m_isExitPopupVisible = true;
                        PauseLayer::onNormalMode(sender);
                        m_fields->m_isExitPopupVisible = false;
                    }
                }
            );
        }
    };
}