#include <modules/api/mods.hpp>
#include <modules/config/config.hpp>
#include <modules/gui/color.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/cocos/nodes/CCMenuItemExt.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/binding/GameStatsManager.hpp>

#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/RetryLevelLayer.hpp>

#include <modules.hpp>
#include <ranges>

namespace eclipse::hacks::Global {
    enum class SafeModeState {
        Normal,   // no cheats detected
        Cheating, // cheats detected
        Tripped   // cheated in current attempt
    };

    // Contains the state of activated hacks in an attempt
    std::map<std::string_view, bool> s_attemptCheats;
    std::map<std::string_view, bool> const& getAttemptCheats() { return s_attemptCheats; }

    // Whether the last attempt had tripped any cheats
    bool s_trippedLastAttempt = false;

    class $hack(AutoSafeMode) {
        static bool hasCheats() {
            for (auto& callback : api::getCheats() | std::views::values) {
                if (callback()) return true;
            }

            auto const& hacks = hack::getCheatingHacks();
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
            for (auto const& hack : hack::getCheatingHacks()) {
                if (hack->isCheating()) {
                    s_attemptCheats[hack->getId()] = true;
                } else if (s_attemptCheats.contains(hack->getId())) {
                    s_attemptCheats[hack->getId()] = false;
                }
            }
            for (auto& [id, active] : api::getCheats()) {
                if (active()) {
                    s_attemptCheats[id] = true;
                } else if (s_attemptCheats.contains(id)) {
                    s_attemptCheats[id] = false;
                }
            }
        }

        static std::string constructMessage() {
            std::string message = "";
            message.reserve(s_attemptCheats.size() * 20);
            for (auto const& [id, active] : s_attemptCheats) {
                fmt::format_to(std::back_inserter(message), "- {}{}</c>\n", active ? "<cr>" : "<co>", id);
            }

            // Remove the last newline
            if (!message.empty())
                message.pop_back();

            return message;
        }

        static void showPopup(std::string const& message) {
            if (!s_trippedLastAttempt && !hasCheats())
                return;

            FLAlertLayer::create(
                nullptr,
                "Cheats Detected", message, "OK",
                nullptr, 400, true, 0, 1
            )->show();
        }

        void init() override {
            auto tab = gui::MenuTab::find("tab.global");

            config::setIfEmpty("global.autosafemode", true);
            config::setIfEmpty("global.autosafemode.warn-popup", true);

            tab->addToggle("global.autosafemode")->handleKeybinds()->setDescription()->addOptions([](auto options) {
                options->addToggle("global.autosafemode.warn-popup")->setDescription();
            });
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

    class $hack(SafeMode) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");

            config::setIfEmpty("global.safemode", false);
            config::setIfEmpty("global.safemode.freeze_attempts", true);
            config::setIfEmpty("global.safemode.freeze_jumps", true);
            config::setIfEmpty("global.safemode.freeze_best_run", false);
            config::setIfEmpty("global.safemode.warn-popup", true);

            tab->addToggle("global.safemode")->handleKeybinds()->setDescription()
               ->addOptions([](auto options) {
                   options->addToggle("global.safemode.warn-popup")->setDescription();
                   options->addToggle("global.safemode.freeze_attempts");
                   options->addToggle("global.safemode.freeze_jumps");
                   options->addToggle("global.safemode.freeze_best_run");
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
            auto* GSM = utils::get<GameStatsManager>();

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

                auto* GSM = utils::get<GameStatsManager>();

                if (config::get<bool>("global.safemode.freeze_jumps", true))
                    GSM->setStat("1", m_fields->totalJumps);
                if (config::get<bool>("global.safemode.freeze_attempts", true))
                    GSM->setStat("2", m_fields->totalAttempts);
            }

            PlayLayer::destroyPlayer(player, object);

            this->m_isTestMode = original;
        }

        void resetLevel() override {
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
            if (config::get<bool>("global.safemode", false)
                || AutoSafeMode::shouldEnable()
                && config::get<bool>("global.safemode.freeze_jumps", true))
                return;

            PlayerObject::incrementJumps();
        }
    };

    #define NormalColor gui::Colors::GREEN
    #define CheatingColor gui::Colors::RED
    #define TrippedColor gui::Color { 0.72f, 0.37f, 0.f }

    static CCMenuItemSpriteExtra* createCI() {
        auto ci = cocos2d::CCLabelBMFont::create(".", "bigFont.fnt");
        auto color = AutoSafeMode::hasCheats() ? CheatingColor : s_trippedLastAttempt ? TrippedColor : NormalColor;
        ci->setColor(color.toCCColor3B());
        auto msg = AutoSafeMode::constructMessage();
        auto btn = gui::cocos::createSpriteExtra(ci, [msg = std::move(msg)](auto) {
            AutoSafeMode::showPopup(msg);
        });
        btn->setAnchorPoint({0.45f, 0.2f});
        btn->setPosition({-165, 100});
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
        ENABLE_FIRST_HOOKS_ALL()

        void customSetup() override {
            EndLevelLayer::customSetup();

            if (s_attemptCheats.contains("Bot")) {
                CCNode* chainlt = m_mainLayer->getChildByID("chain-left");
                if (!chainlt) chainlt = m_mainLayer->getChildByType<cocos2d::CCSprite*>(0);
                if (chainlt) chainlt->setPositionX(chainlt->getPositionX() - 10.f);

                CCNode* chainrt = m_mainLayer->getChildByID("chain-right");
                if (!chainrt) chainrt = m_mainLayer->getChildByType<cocos2d::CCSprite*>(1);
                if (chainrt) chainrt->setPositionX(chainrt->getPositionX() + 10.f);
            }

            if (!config::get<bool>("labels.cheat-indicator.endscreen", true))
                return;

            auto btn = createCI();
            if (this->getChildByIDRecursive("absolllute.megahack/cheat-indicator")) {
                btn->setPosition({-168, 90});
            }

            auto menu = m_mainLayer->getChildByID("button-menu");
            if (!menu) menu = m_mainLayer->getChildByType<cocos2d::CCMenu>(0);
            if (menu) menu->addChild(btn);
        }
    };

    /// ==========
    /// "Cheats Enabled" warning popup
    /// ==========

    static bool manualSafeModePopup() {
        if (!config::get<"global.safemode", bool>(false)) {
            return false;
        }

        if (!config::get<"global.safemode.warn-popup", bool>(true)) {
            return false;
        }

        s_attemptCheats.clear();
        AutoSafeMode::updateCheatStates();

        // edge case: noclip is only considered a cheat if you die while using it
        if (config::get<"player.noclip", bool>()) {
            s_attemptCheats["Noclip"] = false;
        }

        if (s_attemptCheats.empty() ) {
            FLAlertLayer::create(
                nullptr,
                "Safe Mode (Eclipse)",
                "<cr>Progress saving is disabled.</c>\n\n"
                "<cy>Safe Mode is enabled. Progress will NOT be saved for any attempts until you disable Safe Mode.</c>",
                "OK", nullptr, 400, true, 0, 1
            )->show();
        } else {
            FLAlertLayer::create(
                nullptr,
                "Safe Mode (Eclipse)",
                fmt::format(
                    "<cr>Progress saving is disabled.</c>\n\n"
                    "<cy>Safe Mode is enabled. Progress will NOT be saved for any attempts until you disable Safe Mode.</c>"
                    "\n\n<cy>Active cheats ({}):</c>\n{}",
                    s_attemptCheats.size(),
                    AutoSafeMode::constructMessage()
                ), "OK",
                nullptr, 400, true, 0, 1
            )->show();
        }

        return true;
    }

    static bool showCheatWarn() {
        if (manualSafeModePopup()) {
            return true;
        }

        if (!config::get<"global.autosafemode.warn-popup", bool>(true)) {
            return false;
        }

        if (!config::get<"global.autosafemode", bool>(true)) {
            return false;
        }

        s_attemptCheats.clear();
        AutoSafeMode::updateCheatStates();

        // edge case: noclip is only considered a cheat if you die while using it
        if (config::get<"player.noclip", bool>()) {
            s_attemptCheats["Noclip"] = false;
        }

        if (s_attemptCheats.empty() ) {
            return false;
        }

        FLAlertLayer::create(
            nullptr,
            "Auto-Safe Mode (Eclipse)",
            fmt::format(
                "<cr>Progress saving is disabled.</c>\n\n"
                "<cy>Auto-Safe Mode is active because cheats are enabled.</c>\n"
                "While this is active, progress will <cr>NOT</c> be saved.\n\n"
                "<cy>Active cheats ({}):</c>\n{}",
                s_attemptCheats.size(),
                AutoSafeMode::constructMessage()
            ), "OK",
            nullptr, 400, true, 0, 1
        )->show();

        return true;
    }

#define DEFINE_WARN_HOOK(name, cls)                \
    class $modify(name, cls) {                     \
        ENABLE_FIRST_HOOKS_ALL()                   \
        struct Fields {                            \
            bool m_shownCheatWarn = false;         \
        };                                         \
        void onPlay(CCObject* sender) {            \
            if (!m_fields->m_shownCheatWarn) {     \
                m_fields->m_shownCheatWarn = true; \
                if (showCheatWarn()) return;       \
            }                                      \
            cls::onPlay(sender);                   \
        }                                          \
    }

    DEFINE_WARN_HOOK(CheatsEnabledWarnLPHook, LevelPage);
    DEFINE_WARN_HOOK(CheatsEnabledWarnELLHook, EditLevelLayer);
    DEFINE_WARN_HOOK(CheatsEnabledWarnLILHook, LevelInfoLayer);
}
