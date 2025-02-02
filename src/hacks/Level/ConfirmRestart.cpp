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

            tab->addToggle("Confirm Restart", "level.confirmrestart")
               ->handleKeybinds()->setDescription("Adds an extra confirmation window when restarting. (Implemented by Cynthebnuy)")
        }

        [[nodiscard]] const char* getId() const override { return "Confirm Restart"; }
    };

    REGISTER_HACK(ConfirmRestart)

    class $modify(ConfirmRestartPauseLayerHook, PauseLayer) {	
	    struct Fields {
                bool m_isPopupVisible = false;
	    };

	    void onRestart(cocos2d::CCObject* sender) {
            if (m_fields->m_isPopupVisible || 
            !config::get<bool>("level.confirmrestart", false)) {
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
}