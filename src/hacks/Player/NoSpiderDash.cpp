#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {

    class NoSpiderDash : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("No Spider Dash", "player.nospiderdash")
                ->setDescription("Disables the spider dash trail.")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Spider Dash"; }
    };

    REGISTER_HACK(NoSpiderDash)

    class $modify(NoSpiderDashPOHook, PlayerObject) {
        void playSpiderDashEffect(cocos2d::CCPoint from, cocos2d::CCPoint to) {
            bool playEffects = m_playEffects;

            if (config::get<bool>("player.nospiderdash", false))
                m_playEffects = false;

            PlayerObject::playSpiderDashEffect(from, to);
            m_playEffects = playEffects;
        }
    };

}
