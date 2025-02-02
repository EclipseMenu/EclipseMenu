#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PauseLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(ConfirmPractice) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.confirmpractice")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Confirm Practice"; }
    };

    REGISTER_HACK(ConfirmPractice)

    class $modify(ConfirmPracticePauseLayerHook, PauseLayer) {	
        // ADD_HOOKS_DELEGATE("level.confirmpractice")

	struct Fields {
            bool m_isPopupVisible = false;
	};

	void onPracticeMode(cocos2d::CCObject* sender) {
            if (m_fields->m_isPopupVisible) {
                PauseLayer::onPracticeMode(sender);
                return;
		}

            geode::createQuickPopup(
                "Did I cook?",          // title
                "ahh commit",			// content
                "Nuh uh", "Yuh huh",    // buttons
                [this, sender](auto, bool btn2) {
                    if (btn2) {
                        m_fields->m_isPopupVisible = true;
                        PauseLayer::onPracticeMode(sender);
                        m_fields->m_isPopupVisible = false;
                    }
                }
            );
        }
    };
}