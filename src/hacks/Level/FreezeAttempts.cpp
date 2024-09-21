#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class FreezeAttempts : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Freeze Attempts", "level.freeze_attempts")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Freeze Attempts"; }
    };

    REGISTER_HACK(FreezeAttempts)
    
    class $modify(FreezeAttemptsPLHook, PlayLayer) {
        struct Fields {
            std::uint32_t totalAttempts;
        };

        static void onModify(auto& self) {
            HOOKS_TOGGLE("level.freeze_attempts", PlayLayer,
                "updateAttempts", "resetLevel", "onQuit"
            );
        }

        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            auto* GSM = GameStatsManager::sharedState();

            m_fields->totalAttempts = GSM->getStat("2");

            return PlayLayer::init(level, useReplay, dontCreateObjects);
        }

        void updateAttempts() {}

        void resetLevel() {
            this->m_level->m_attempts = this->m_level->m_attempts - 1;
            PlayLayer::resetLevel();
        }

        void onQuit() {
            GameStatsManager::sharedState()->setStat("2", m_fields->totalAttempts);
            PlayLayer::onQuit();
        }
    };
}
