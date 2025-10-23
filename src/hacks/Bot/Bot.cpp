#include <Geode/Result.hpp>

#include <modules.hpp>
#include <modules/bot/bot.hpp>
#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/popup.hpp>
#include <modules/gui/cocos/cocos.hpp>
#include <modules/gui/components/button.hpp>
#include <modules/gui/components/filesystem-combo.hpp>
#include <modules/gui/components/radio.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/i18n/translations.hpp>

#include <Geode/modify/CheckpointObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

namespace eclipse::hacks::Bot {
    static bot::Bot s_bot;

    void newReplay() {
        Popup::prompt(
            i18n::get_("bot.new-replay"),
            i18n::get_("bot.new-replay.msg"),
            [&](bool result, std::string name) {
                if (!result)
                    return;

                auto replaysDir = Mod::get()->getSaveDir() / "replays";
                auto replayPath = replaysDir / (name + ".gdr2");

                std::error_code ec;
                if (!std::filesystem::exists(replaysDir, ec)) {
                    std::filesystem::create_directory(replaysDir, ec);
                    if (ec) {
                        return Popup::create(
                            i18n::get_("common.error"),
                            ec.message()
                        );
                    }
                }

                auto res = s_bot.save(replayPath);

                if(res.isErr()) {
                    Popup::create(i18n::get_("common.error"), res.unwrapErr());
                    return;
                }

                config::set("bot.selectedreplay", replayPath);

                // refresh cocos ui page
                if (auto cocos = gui::cocos::CocosRenderer::get())
                    cocos->refreshPage();
            },
            i18n::get_("common.create"),
            i18n::get_("common.cancel"),
            ""
        );
    }

    void saveReplay() {
        std::filesystem::path replayDirectory = Mod::get()->getSaveDir() / "replays";

        std::error_code ec;
        if (!std::filesystem::exists(replayDirectory, ec)) {
            std::filesystem::create_directory(replayDirectory, ec);
            if (ec) {
                return Popup::create(
                    i18n::get_("common.error"),
                    ec.message()
                );
            }
        }

        auto replayPath = config::get<std::filesystem::path>("bot.selectedreplay", "temp");

        if (std::filesystem::exists(replayPath, ec)) {
            return Popup::create(
                i18n::get_("common.warning"),
                i18n::format("bot.overwrite", string::pathToString(replayPath.filename().stem())),
                i18n::get_("common.yes"),
                i18n::get_("common.no"),
                [&](bool result) {
                    if (!result)
                        return;

                    auto confirmReplayDirectory = config::get<std::filesystem::path>("bot.selectedreplay", "temp");

                    auto res = s_bot.save(confirmReplayDirectory);
                    if(res.isErr()) {
                        Popup::create(i18n::get_("common.error"), std::move(res).unwrapErr());
                        return;
                    }

                    Popup::create(
                        i18n::get_("bot.saved"),
                        i18n::format(
                            "bot.saved.msg", confirmReplayDirectory.filename().stem(), s_bot.getInputCount()
                        )
                    );

                    config::set("bot.selectedreplay", confirmReplayDirectory);
                }
            );
        }

        auto res = s_bot.save(replayPath);
        if(res.isErr()) {
            Popup::create(i18n::get_("common.error"), res.unwrapErr());
            return;
        }
        Popup::create(
            i18n::get_("bot.saved"),
            i18n::format("bot.saved.msg", replayPath.filename().stem(), s_bot.getInputCount())
        );
        config::set("bot.selectedreplay", replayPath);
    }

    void confirmLoad(std::filesystem::path const& replayPath) {
        auto res = s_bot.load(replayPath);
        if (res.isErr()) {
            return Popup::create(
                i18n::get_("common.error"),
                i18n::format("bot.load-fail", res.unwrapErr())
            );
        }
        Popup::create(
            i18n::get_("bot.loaded"),
            i18n::format("bot.loaded.msg", replayPath.filename().stem(), s_bot.getInputCount())
        );
    }

    void loadReplay() {
        std::filesystem::path replayPath = config::get<std::string>("bot.selectedreplay", "");

        if (s_bot.getInputCount() > 0) {
            return Popup::create(
                i18n::get_("common.warning"),
                i18n::get_("bot.overwrite-current"),
                i18n::get_("common.yes"),
                i18n::get_("common.no"),
                [&](bool result) {
                    if (!result)
                        return;

                    std::filesystem::path confirmReplayPath = config::get<std::string>("bot.selectedreplay", "");

                    confirmLoad(confirmReplayPath);
                }
            );
        }

        confirmLoad(replayPath);
    }

    void deleteReplay() {
        std::filesystem::path replayPath = config::get<std::string>("bot.selectedreplay", "");
        std::error_code ec;
        if (!std::filesystem::exists(replayPath, ec)) {
            return Popup::create(
                i18n::get_("bot.delete-invalid"),
                i18n::get_("bot.delete-invalid.msg")
            );
        }
        auto replayName = string::pathToString(replayPath.filename().stem());
        Popup::create(
            i18n::get_("common.warning"),
            i18n::format("bot.confirm-delete", replayName),
            i18n::get_("common.yes"),
            i18n::get_("common.no"),
            [replayPath, replayName](bool result) {
                if (!result) return;
                std::error_code ec;
                if (std::filesystem::exists(replayPath, ec)) {
                    Popup::create(
                        i18n::get_("bot.deleted"),
                        i18n::format("bot.deleted.msg", replayName)
                    );
                    std::filesystem::remove(replayPath, ec);
                    if (ec) {
                        return Popup::create(
                            i18n::get_("common.error"),
                            ec.message()
                        );
                    }
                    // apparently i cannot put the Popup below here otherwise some memory corruption happens, WHY? its not even a pointer!!
                    config::set("bot.selectedreplay", "");

                    // refresh cocos ui page
                    if (auto cocos = gui::cocos::CocosRenderer::get())
                        cocos->refreshPage();
                }
            }
        );
    }

    class $hack(Bot) {
        static void savePreBotSettings() {
            config::set("bot.original.tpsbypass", config::get<bool>("global.tpsbypass.toggle", false));
            config::set("bot.original.checkpointdelay", config::get<bool>("level.checkpointdelay", false));
        }

        static void restorePreBotSettings() {
            config::set("global.tpsbypass.toggle", config::get<bool>("bot.original.tpsbypass", false));
            config::set("level.checkpointdelay", config::get<bool>("bot.original.checkpointdelay", false));
        }

        static void applySettings() {
            if (s_bot.getState() == bot::State::DISABLED) {
                return;
            }

            config::set("level.checkpointdelay", true);
            config::set("global.tpsbypass.toggle", true);
            if (s_bot.getState() == bot::State::RECORD) {
                s_bot.setFramerate(utils::getTPS());
            } else {
                config::set<float>("global.tpsbypass", s_bot.getFramerate());
            }

            static Mod* cbfMod = Loader::get()->getLoadedMod("syzzi.click_between_frames");
            if (cbfMod) cbfMod->setSettingValue<bool>("soft-toggle", true);
        }

        void init() override {
            const auto updateBotState = [](int state) {
                if (s_bot.getState() == bot::State::DISABLED) {
                    savePreBotSettings();
                }

                s_bot.setState(static_cast<bot::State>(state));

                if (state == 0) {
                    restorePreBotSettings();
                    return;
                }
                Bot::applySettings();
            };

            config::set("bot.state", 0);
            restorePreBotSettings();
            updateBotState(0);

            auto tab = gui::MenuTab::find("tab.bot");

            tab->addRadioButton("bot.disabled", "bot.state", 0)->callback(updateBotState)->handleKeybinds();
            tab->addRadioButton("bot.record", "bot.state", 1)->callback(updateBotState)->handleKeybinds();
            tab->addRadioButton("bot.playback", "bot.state", 2)->callback(updateBotState)->handleKeybinds();

            tab->addFilesystemCombo("bot.replays", "bot.selectedreplay", Mod::get()->getSaveDir() / "replays");

            tab->addToggle("bot.ignore-inputs")->handleKeybinds()->setDescription();

            tab->addButton("common.new")->handleKeybinds()->callback(newReplay);
            tab->addButton("common.save")->handleKeybinds()->callback(saveReplay);
            tab->addButton("common.load")->handleKeybinds()->callback(loadReplay);
            tab->addButton("common.delete")->handleKeybinds()->callback(deleteReplay);
        }

        [[nodiscard]] bool isCheating() const override {
            auto state = config::get<"bot.state", int>(0);
            // only check if we are in playback mode and there are inputs
            return state == 2 && s_bot.getInputCount() != 0;
        }

        [[nodiscard]] const char* getId() const override { return "Bot"; }
    };

    REGISTER_HACK(Bot)

    $execute {
        new EventListener<EventFilter<events::LoadReplayEvent>>(+[](events::LoadReplayEvent* e) {
            if (auto* path = e->getPath()) {
                e->setResult(s_bot.load(*path));
            } else {
                e->setResult(s_bot.load(e->getData()));
            }
            return ListenerResult::Stop;
        });
    }

    class $modify(BotPLHook, PlayLayer) {
        bool init(GJGameLevel* gj, bool p1, bool p2) {
            bool result = PlayLayer::init(gj, p1, p2);
            s_bot.setLevelInfo(gdr::Level(gj->m_levelName, gj->m_levelID.value()));
            s_bot.setPlatformer(gj->isPlatformer());
            return result;
        }

        void resetLevel() {
            PlayLayer::resetLevel();
            Bot::applySettings();

            if (s_bot.getState() == bot::State::RECORD) {
                //gd does this automatically for holding but not releases so we do it manually
                s_bot.recordInput(m_gameState.m_currentProgress + 1, PlayerButton::Jump, false, false);
                m_player1->m_isDashing = false; // temporary, find better way to fix dash orbs
                if (m_gameState.m_isDualMode && m_levelSettings->m_twoPlayerMode) {
                    s_bot.recordInput(m_gameState.m_currentProgress + 1, PlayerButton::Jump, true, false);
                    m_player2->m_isDashing = false;
                }
            }

            if (m_checkpointArray->count() > 0) return;

            s_bot.restart();

            if (s_bot.getState() == bot::State::PLAYBACK) return;

            s_bot.clearInputs();
        }

        CheckpointObject* markCheckpoint() {
            if (s_bot.getState() == bot::State::RECORD && (m_player1->m_isDead || m_player2->m_isDead))
                return nullptr;

            return PlayLayer::markCheckpoint();
        }

        void loadFromCheckpoint(CheckpointObject* checkpoint) {
            PlayLayer* playLayer = utils::get<PlayLayer>();

            if (s_bot.getState() != bot::State::RECORD || !playLayer)
                return PlayLayer::loadFromCheckpoint(checkpoint);

            s_bot.removeInputsAfter(checkpoint->m_gameState.m_currentProgress);

            PlayLayer::loadFromCheckpoint(checkpoint);
        }
    };

    class $modify(BotBGLHook, GJBaseGameLayer) {
        void simulateClick(PlayerButton button, bool down, bool player2) {
            auto performButton = down ? &PlayerObject::pushButton : &PlayerObject::releaseButton;
            bool swapControls = GameManager::get()->getGameVariable("0010");
            player2 = swapControls ? !player2 : player2;

            // in two player mode, only one player should be controlled
            if (m_levelSettings->m_twoPlayerMode && m_gameState.m_isDualMode) {
                PlayerObject* player = player2 ? m_player2 : m_player1;
                (player->*performButton)(button);
            } else {
                // otherwise, click both players (if dual mode is enabled)
                (m_player1->*performButton)(button);
                if (m_gameState.m_isDualMode)
                    (m_player2->*performButton)(button);
            }

            // register the click for touch triggers
            m_effectManager->playerButton(down, !player2);

            // increment click count
            if (down) {
                m_clicks++;
                if (button == PlayerButton::Jump) {
                    // not sure what this does, but this is how it is in original handleButton
                    m_jumping = true;
                }
            }
        }

        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            if (s_bot.getState() != bot::State::PLAYBACK)
                return;

            std::optional<gdr::Input<>> input = std::nullopt;

            while ((input = s_bot.poll(m_gameState.m_currentProgress)) != std::nullopt) {
                this->simulateClick((PlayerButton) input->button, input->down, input->player2);
            }
        }

        void handleButton(bool down, int button, bool player1) {
            if (s_bot.getState() == bot::State::PLAYBACK && s_bot.getInputCount() && config::get<bool>("bot.ignore-inputs", false))
                return;

            GJBaseGameLayer::handleButton(down, button, player1);

            if (s_bot.getState() != bot::State::RECORD)
                return;

            s_bot.recordInput(m_gameState.m_currentProgress, (PlayerButton) button, !player1, down);
        }
    };
}
