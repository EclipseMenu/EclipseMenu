#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/binding/GameStatsManager.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/RetryLevelLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

namespace eclipse::hacks::Global {

    enum class SafeModeState {
        Normal,    // no cheats detected
        Cheating,  // cheats detected
        Tripped    // cheated in current attempt
    };

    // Contains the state of activated hacks in an attempt
    std::map<std::string_view, bool> s_attemptCheats;

    // Whether the last attempt had tripped any cheats
    bool s_trippedLastAttempt = false;

    class AutoSafeMode : public hack::Hack {
    public:
        static bool hasCheats() {
            const auto& hacks = hack::Hack::getHacks();
            return std::ranges::any_of(hacks, [](auto& hack) {
                return hack->isCheating();
            });
        }

        static bool shouldEnable() {
            if (!config::get<bool>("global.autosafemode", false))
                return false;

            return s_trippedLastAttempt || hasCheats();
        }

        static void updateCheatStates() {
            for (const auto& hack : hack::Hack::getHacks()) {
                if (hack->isCheating()) {
                    s_attemptCheats[hack->getId()] = true;
                } else if (s_attemptCheats.contains(hack->getId())) {
                    s_attemptCheats[hack->getId()] = false;
                }
            }
        }

        static std::string constructMessage() {
            std::string message = "";
            message.reserve(s_attemptCheats.size() * 20);
            for (const auto& [id, active] : s_attemptCheats) {
                message += fmt::format("- {}{}</c>\n", active ? "<cr>" : "<co>", id);
            }

            // Remove the last newline
            if (!message.empty())
                message.pop_back();

            return message;
        }

        static void showPopup(const std::string& message) {
            if (!s_trippedLastAttempt && !hasCheats())
                return;

            FLAlertLayer::create(nullptr,
                "Cheats Detected", message, "OK",
                nullptr, 400, true, 0, 1
            )->show();
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
            this->updateCheatStates();
            auto hasCheats = this->hasCheats();
            s_trippedLastAttempt |= hasCheats;
            config::setTemp("hasCheats", hasCheats);
            config::setTemp("trippedSafeMode", s_trippedLastAttempt);
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

        void destroyPlayer(PlayerObject* player, GameObject* object) override {
            bool original = this->m_isTestMode;
            bool safeMode = config::get<bool>("global.safemode", false);

            if (safeMode || AutoSafeMode::shouldEnable()) {
                this->m_isTestMode = true;

                auto* GSM = GameStatsManager::sharedState();

                if (config::get<bool>("global.safemode.freeze_jumps", true))
                    GSM->setStat("1", m_fields->totalJumps);
                if (config::get<bool>("global.safemode.freeze_attempts", true))
                    GSM->setStat("2", m_fields->totalAttempts);
            }

            PlayLayer::destroyPlayer(player, object);

            this->m_isTestMode = original;
        }

        void resetLevel() {
            bool safeMode = config::get<bool>("global.safemode", false);

            if ((safeMode || AutoSafeMode::shouldEnable()) && config::get<bool>("global.safemode.freeze_attempts", true))
                this->m_level->m_attempts = this->m_level->m_attempts - 1;

            s_trippedLastAttempt = false;
            s_attemptCheats.clear();
            PlayLayer::resetLevel();
        }

        void levelComplete() {
            bool original = this->m_isTestMode;
            bool safeMode = config::get<bool>("global.safemode", false);

            if (safeMode || AutoSafeMode::shouldEnable())
                this->m_isTestMode = true;

            PlayLayer::levelComplete();

            this->m_isTestMode = original;
        }
    };

    class $modify(SafeModePOHook, PlayerObject) {
        ENABLE_SAFE_HOOKS_ALL()

        void incrementJumps() {
            if (config::get<bool>("global.safemode", false) || AutoSafeMode::shouldEnable() && config::get<bool>("global.safemode.freeze_jumps", true))
                return;

            PlayerObject::incrementJumps();
        }
    };

    #define NormalColor gui::Color::GREEN
    #define CheatingColor gui::Color::RED
    #define TrippedColor gui::Color { 0.72f, 0.37f, 0.f }

    static CCMenuItemSpriteExtra* createCI() {
        auto ci = cocos2d::CCLabelBMFont::create(".", "bigFont.fnt");
        auto color = AutoSafeMode::hasCheats() ? CheatingColor : s_trippedLastAttempt ? TrippedColor : NormalColor;
        ci->setColor(color.toCCColor3B());
        auto msg = AutoSafeMode::constructMessage();
        auto btn = geode::cocos::CCMenuItemExt::createSpriteExtra(ci, [msg](auto) {
            AutoSafeMode::showPopup(msg);
        });
        btn->setAnchorPoint({ 0.45f, 0.2f });
        btn->setPosition({ -165, 100 });
        btn->setID("cheat-indicator"_spr);
        return btn;
    }

    class $modify(SafeModeRLLHook, RetryLevelLayer) {
        void customSetup() override {
            RetryLevelLayer::customSetup();
            if (!config::get<bool>("labels.cheat-indicator.endscreen", true))
                return;

            m_mainMenu->addChild(createCI());
        }
    };

    class $modify(SafeModeELLHook, EndLevelLayer) {
        void customSetup() override {
            EndLevelLayer::customSetup();
            if (!config::get<bool>("labels.cheat-indicator.endscreen", true))
                return;

            auto btn = createCI();
            if (this->getChildByIDRecursive("absolllute.megahack/cheat-indicator")) {
                btn->setPosition({ -168, 90 });
            }

            auto menu = m_mainLayer->getChildByID("button-menu");
            if (!menu) menu = m_mainLayer->getChildByType<cocos2d::CCMenu>(0);
            if (menu) menu->addChild(btn);
        }
    };

}
