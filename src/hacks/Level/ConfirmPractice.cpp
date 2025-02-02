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

            tab->addToggle("Confirm Practice", "level.confirmpractice")
               ->handleKeybinds()->setDescription("Adds an extra confirmation window when entering Practice Mode. (Implemented by Cynthebnuy)")
               ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                   options->addToggle("Confirm Exit Practice", "level.confirmpractice.confirmexitpractice")->setDescription("Toggle the confirmation window for when you exit Practice Mode.");
               });
        }

        [[nodiscard]] const char* getId() const override { return "Confirm Practice"; }
    };

    REGISTER_HACK(ConfirmPractice)

    class $modify(ConfirmPracticePauseLayerHook, PauseLayer) {
        ADD_HOOKS_DELEGATE("level.confirmpractice")

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
                "Enter Practice",                                   // title
                "Are you sure you want to\n<cr>enter</c> <cg>Practice Mode</c>?",			// content
                "Cancel", "Enter",                                  // buttons
                [this, sender](auto, bool btn2) {
                    if (btn2) {
                        m_fields->m_isEnterPopupVisible = true;
                        PauseLayer::onPracticeMode(sender);
                        m_fields->m_isEnterPopupVisible = false;
                    }
                }
            );
        }

        ADD_HOOKS_DELEGATE("level.confirmpractice.confirmexitpractice")

        void onNormalMode(cocos2d::CCObject* sender) {
            if (m_fields->m_isExitPopupVisible) {
                PauseLayer::onNormalMode(sender);
                return;
            }

            geode::createQuickPopup(
                "Exit Practice",                                     // title
                "Are you sure you want to\n<cr>exit</c> <cg>Practice Mode</c>?",			 // content
                "Cancel", "Exit",                                    // buttons
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