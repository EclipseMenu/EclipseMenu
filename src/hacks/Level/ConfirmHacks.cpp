#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PauseLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(ConfirmRestart) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            config::setIfEmpty("level.confirmrestart", false);

            tab->addToggle("level.confirmrestart")
               ->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Confirm Restart"; }
    };

    class $hack(ConfirmFullReset) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            config::setIfEmpty("level.confirmfullreset", false);

            tab->addToggle("level.confirmfullreset")
               ->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Confirm Full Reset"; }
    };

    class $hack(ConfirmPractice) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            config::setIfEmpty("level.confirmpractice", false);
            config::setIfEmpty("level.confirmpractice.confirmexitpractice", true);

            tab->addToggle("level.confirmpractice")
                ->handleKeybinds()->setDescription()
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                options->addToggle("level.confirmpractice.confirmexitpractice")->setDescription();
                    });
        }

        [[nodiscard]] const char* getId() const override { return "Confirm Practice"; }
    };

    REGISTER_HACK(ConfirmRestart)
    REGISTER_HACK(ConfirmFullReset)
    REGISTER_HACK(ConfirmPractice)

    class $modify(ConfirmRestartPauseLayerHook, PauseLayer) {
        ADD_HOOKS_DELEGATE("level.confirmrestart")

	    struct Fields {
                bool m_isPopupVisible = false;
	    };

	    void onRestart(cocos2d::CCObject* sender) {
            if (m_fields->m_isPopupVisible) {
                PauseLayer::onRestart(sender);
                return;
		    }

            geode::createQuickPopup(
                "Restart Level",                                   // title
                "Are you sure you want to <cr>restart</c>?",			// content
                "Cancel", "Restart",                                  // buttons
                [this, sender](auto, bool btn2) {
                    if (btn2) {
                        m_fields->m_isPopupVisible = true;
                        PauseLayer::onRestart(sender);
                        m_fields->m_isPopupVisible = false;
                    }
                }
            );
        }
    };

    class $modify(ConfirmPracticePauseLayerHook, PauseLayer) {
        ADD_HOOKS_DELEGATE("level.confirmpractice")

            struct Fields {
            bool m_isEnterPopupVisible = false;
            bool m_isExitPopupVisible = false;
        };

        void onPracticeMode(cocos2d::CCObject * sender) {
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

        void onNormalMode(cocos2d::CCObject * sender) {
            if (m_fields->m_isExitPopupVisible ||
                !config::get<bool>("level.confirmpractice.confirmexitpractice", false)) {
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

    class $modify(ConfirmFullResetPauseLayerHook, PauseLayer) {
        ADD_HOOKS_DELEGATE("level.confirmfullreset")

        struct Fields {
            bool m_isPopupVisible = false;
        };

        void onRestartFull(cocos2d::CCObject * sender) {
            if (m_fields->m_isPopupVisible) {
                PauseLayer::onRestartFull(sender);
                return;
            }

            geode::createQuickPopup(
                "Reset Progress",                                   // title
                "Are you sure you want to <cr>fully reset current progress</c>?",			// content
                "Cancel", "Reset",                                  // buttons
                [this, sender](auto, bool btn2) {
                    if (btn2) {
                        m_fields->m_isPopupVisible = true;
                        PauseLayer::onRestartFull(sender);
                        m_fields->m_isPopupVisible = false;
                    }
                }
            );
        }
    };
}
