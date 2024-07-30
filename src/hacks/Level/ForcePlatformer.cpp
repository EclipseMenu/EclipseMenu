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

    class $modify(ForcePlatformerPLHook, PlayLayer) {
        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
            if (!config::get<bool>("level.forceplatformer", false)) return true;

            if (m_player1) m_player1->togglePlatformerMode(true);
            if (m_player2) m_player2->togglePlatformerMode(true);

            return true;
        }
    };
}