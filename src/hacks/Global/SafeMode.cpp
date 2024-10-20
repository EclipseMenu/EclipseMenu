#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/binding/GameStatsManager.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Global {

    class AutoSafeMode : public hack::Hack {
    public:
        static bool hasCheats() {
            const auto& hacks = hack::Hack::getHacks();
            return std::any_of(hacks.begin(), hacks.end(), [](auto& hack) {
                return hack->isCheating();
            });
        }

        static bool shouldEnable() {
            if (!config::get<bool>("global.autosafemode", false))
                return false;

            return hasCheats();
        }

    private:
        void init() override {
            auto tab = gui::MenuTab::find("Global");

            config::setIfEmpty("global.autosafemode", true);
            tab->addToggle("Auto Safe Mode", "global.autosafemode")
                ->handleKeybinds()
                ->setDescription("Automatically enables safe mode if cheats are enabled");
        }

        void update() override {
            config::setTemp("hasCheats", hasCheats());
        }

        [[nodiscard]] const char* getId() const override { return "Auto Safe Mode"; }
    };

    class SafeMode : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Global");

            config::setIfEmpty("global.safemode", false);
            config::setIfEmpty("global.safemode.freeze_attempts", true);
            config::setIfEmpty("global.safemode.freeze_jumps", true);

            tab->addToggle("Safe Mode", "global.safemode")
                ->handleKeybinds()
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                    options->addToggle("Freeze Attempt Count", "global.safemode.freeze_attempts");
                    options->addToggle("Freeze Jump Count", "global.safemode.freeze_jumps");
                });
        }

        [[nodiscard]] const char* getId() const override { return "Safe Mode"; }
    };

    REGISTER_HACK(AutoSafeMode)
    REGISTER_HACK(SafeMode)

    class $modify(SafeModePLHook, PlayLayer) {
        struct Fields {
            std::uint32_t totalJumps;
            std::uint32_t totalAttempts;
        };

        ENABLE_SAFE_HOOKS_ALL()

        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            auto* GSM = GameStatsManager::sharedState();

            m_fields->totalJumps = GSM->getStat("1");
            m_fields->totalAttempts = GSM->getStat("2");

            if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

            return true;
        }

        void destroyPlayer(PlayerObject* player, GameObject* object) {
            bool original = this->m_isTestMode;
            bool safeMode = config::get<bool>("global.safemode");

            if (safeMode || AutoSafeMode::shouldEnable()) {
                this->m_isTestMode = true;

                auto* GSM = GameStatsManager::sharedState();

                if (config::get<bool>("global.safemode.freeze_jumps"))
                    GSM->setStat("1", m_fields->totalJumps);
                if (config::get<bool>("global.safemode.freeze_attempts"))
                    GSM->setStat("2", m_fields->totalAttempts);
            }

            PlayLayer::destroyPlayer(player, object);

            this->m_isTestMode = original;
        }

        void resetLevel() {
            bool safeMode = config::get<bool>("global.safemode");

            if ((safeMode || AutoSafeMode::shouldEnable()) && config::get<bool>("global.safemode.freeze_attempts"))
                this->m_level->m_attempts = this->m_level->m_attempts - 1;

            PlayLayer::resetLevel();
        }

        void levelComplete() {
            bool original = this->m_isTestMode;
            bool safeMode = config::get<bool>("global.safemode");

            if (safeMode || AutoSafeMode::shouldEnable())
                this->m_isTestMode = true;

            PlayLayer::levelComplete();

            this->m_isTestMode = original;
        }
    };

    class $modify(SafeModePOHook, PlayerObject) {
        ENABLE_SAFE_HOOKS_ALL()

        void incrementJumps() {
            if (config::get<bool>("global.safemode") || AutoSafeMode::shouldEnable() && config::get<bool>("global.safemode.freeze_jumps")) return;

            PlayerObject::incrementJumps();
        }
    };

}
