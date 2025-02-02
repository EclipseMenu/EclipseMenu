#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PauseLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(ConfirmRestart) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            config::setIfEmpty("level.confirmfullreset", false);

            tab->addToggle("Confirm Full Reset", "level.confirmfullreset")
               ->handleKeybinds()->setDescription("Adds an extra confirmation window when fully resetting in Platformer mode. (Implemented by Cynthebnuy)");
        }

        [[nodiscard]] const char* getId() const override { return "Confirm Full Reset"; }
    };

    REGISTER_HACK(ConfirmRestart)

    class $modify(ConfirmRestartPauseLayerHook, PauseLayer) {	
	    struct Fields {
                bool m_isPopupVisible = false;
	    };

        void onRestartFull(cocos2d::CCObject* sender) {
            if (m_fields->m_isPopupVisible || 
            !config::get<bool>("level.confirmfullreset", false)) {
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