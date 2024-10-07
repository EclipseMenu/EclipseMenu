#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {

    class ForcePlatformer : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Force Platformer", "level.forceplatformer")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Force Platformer"; }
    };

    REGISTER_HACK(ForcePlatformer)

    class $modify(ForcePlatformerPLHook, GJBaseGameLayer) {
        ADD_HOOKS_DELEGATE("level.forceplatformer")

        void loadLevelSettings() {
            if(!PlayLayer::get() || m_levelSettings->m_platformerMode) return GJBaseGameLayer::loadLevelSettings(); 


            m_levelSettings->m_platformerMode = true;
            int originalLength = m_levelSettings->m_level->m_levelLength;
            GJBaseGameLayer::loadLevelSettings();
            m_levelSettings->m_level->m_levelLength = originalLength;
            m_levelSettings->m_platformerMode = false;
        }
    };
}