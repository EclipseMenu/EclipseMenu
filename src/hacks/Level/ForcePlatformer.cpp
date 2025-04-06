#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(ForcePlatformer) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.forceplatformer")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] bool isCheating() const override { return config::get<"level.forceplatformer", bool>(); }
        [[nodiscard]] const char* getId() const override { return "Force Platformer"; }
    };

    REGISTER_HACK(ForcePlatformer)

    class $modify(ForcePlatformerGJBLHook, GJBaseGameLayer) {
        ADD_HOOKS_DELEGATE("level.forceplatformer")

        void loadLevelSettings() {
            if (!utils::get<PlayLayer>() || m_levelSettings->m_platformerMode)
                return GJBaseGameLayer::loadLevelSettings();

            //length is changed by loadLevelSettings
            m_levelSettings->m_platformerMode = true;
            int originalLength = m_levelSettings->m_level->m_levelLength;
            GJBaseGameLayer::loadLevelSettings();
            m_levelSettings->m_level->m_levelLength = originalLength;
            m_levelSettings->m_platformerMode = false;
        }
    };

    class $modify(ForcePlatformerPLHook, PlayLayer) {
        void resetLevel() {
            auto val = config::get("level.forceplatformer", false);
            if (val != m_isPlatformer) {
                this->loadLevelSettings();
            }
            PlayLayer::resetLevel();
        }
    };
};
