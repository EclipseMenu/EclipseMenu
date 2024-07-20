#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class ForcePlatformer : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Force Platformer", "level.forceplatformer")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Force Platformer"; }
    };

    REGISTER_HACK(ForcePlatformer)

    class $modify(PlayLayer) {
        bool init(GJGameLevel* gj, bool p1, bool p2) {
            if (!PlayLayer::init(gj, p1, p2)) return false;
            if (!config::get<bool>("level.forceplatformer", false)) return true;

            if (m_player1) m_player1->togglePlatformerMode(true);
            if (m_player2) m_player2->togglePlatformerMode(true);

            return true;
        }
    };
}