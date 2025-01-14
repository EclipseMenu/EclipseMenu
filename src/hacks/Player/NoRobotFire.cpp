#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {
    class $hack(NoRobotFire) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.norobotfire")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Robot Fire"; }
    };

    REGISTER_HACK(NoRobotFire)

    class $modify(NoRobotFirePOHook, PlayerObject) {
        ADD_HOOKS_DELEGATE("player.norobotfire")

        void update(float dt) override {
            PlayerObject::update(dt);
            m_robotFire->setVisible(false);
            m_robotBurstParticles->setVisible(false);
        }
    };
}
