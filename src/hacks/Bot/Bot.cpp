#include <modules/bot/bot.hpp>
#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/popup.hpp>
#include <modules/gui/components/button.hpp>
#include <modules/gui/components/filesystem-combo.hpp>
#include <modules/gui/components/radio.hpp>
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

                s_bot.save(Mod::get()->getSaveDir() / "replays" / (name + ".gdr"));
                config::set("bot.selectedreplay", Mod::get()->getSaveDir() / "replays" / (name + ".gdr"));
            },
            i18n::get_("common.create"),
            i18n::get_("common.cancel"),
            ""
        );
    }

    void saveReplay() {
        std::filesystem::path replayDirectory = Mod::get()->getSaveDir() / "replays";

        if (!std::filesystem::exists(replayDirectory))
            std::filesystem::create_directory(replayDirectory);

        std::filesystem::path replayPath = config::get<std::filesystem::path>("bot.selectedreplay", "temp");

        if (std::filesystem::exists(replayPath)) {
            return Popup::create(
                i18n::get_("common.warning"),
                i18n::format("bot.overwrite", replayPath.filename().stem().string()),
                i18n::get_("common.yes"),
                i18n::get_("common.no"),
                [&](bool result) {
                    if (!result)
                        return;

                    auto confirmReplayDirectory = config::get<std::filesystem::path>("bot.selectedreplay", "temp");

                    s_bot.save(confirmReplayDirectory);
                    Popup::create(
                        i18n::get_("bot.saved"),
                        i18n::format(
                            "bot.saved.msg", confirmReplayDirectory.filename().stem().string(), s_bot.getInputCount()
                        )
                    );

                    config::set("bot.selectedreplay", confirmReplayDirectory);
                }
            );
        }

        s_bot.save(replayPath);
        Popup::create(
            i18n::get_("bot.saved"),
            i18n::format("bot.saved.msg", replayPath.filename().stem().string(), s_bot.getInputCount())
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
        if (!std::filesystem::exists(replayPath)) {
            return Popup::create(
                i18n::get_("bot.delete-invalid"),
                i18n::get_("bot.delete-invalid.msg")
            );
        }
        std::string replayName = replayPath.filename().stem().string();
        Popup::create(
            i18n::get_("common.warning"),
            i18n::format("bot.confirm-delete", replayName),
            i18n::get_("common.yes"),
            i18n::get_("common.no"),
            [replayPath, replayName](bool result) {
                if (!result) return;
                if (std::filesystem::exists(replayPath)) {
                    Popup::create(
                        i18n::get_("bot.deleted"),
                        i18n::format("bot.deleted.msg", replayName)
                    );
                    std::filesystem::remove(replayPath);
                    // apparently i cannot put the Popup below here otherwise some memory corruption happens, WHY? its not even a pointer!!
                    config::set("bot.selectedreplay", "");
                }
            }
        );
    }

    class $hack(Bot) {
        void init() override {
            const auto updateBotState = [](int state) {
                static bool wasTps = eclipse::config::get<bool>("global.tpsbypass.toggle", true);
                if(s_bot.getState() == bot::State::DISABLED)
                    wasTps = eclipse::config::get<bool>("global.tpsbypass.toggle", true);

                s_bot.setState(static_cast<bot::State>(state));

                if(state == 0)
                    eclipse::config::set("global.tpsbypass.toggle", wasTps);
            };

            config::setIfEmpty("bot.state", 0);
            updateBotState(config::get<int>("bot.state", 0));

            auto tab = gui::MenuTab::find("tab.bot");

            tab->addRadioButton("bot.disabled", "bot.state", 0)->callback(updateBotState)->handleKeybinds();
            tab->addRadioButton("bot.record", "bot.state", 1)->callback(updateBotState)->handleKeybinds();
            tab->addRadioButton("bot.playback", "bot.state", 2)->callback(updateBotState)->handleKeybinds();

            tab->addFilesystemCombo("bot.replays", "bot.selectedreplay", Mod::get()->getSaveDir() / "replays");
            tab->addButton("common.new")->callback(newReplay);
            tab->addButton("common.save")->callback(saveReplay);
            tab->addButton("common.load")->callback(loadReplay);
            tab->addButton("common.delete")->callback(deleteReplay);
        }

        [[nodiscard]] bool isCheating() const override {
            CACHE_CONFIG(int, state, "bot.state", 0);
            // only check if we are in playback mode and there are inputs
            return state == 2 && s_bot.getInputCount() != 0;
        }

        [[nodiscard]] const char* getId() const override { return "Bot"; }
    };

    REGISTER_HACK(Bot)

    class $modify(BotPLHook, PlayLayer) {
        bool init(GJGameLevel* gj, bool p1, bool p2) {
            bool result = PlayLayer::init(gj, p1, p2);
            s_bot.setLevelInfo(gdr::Level(gj->m_levelName, gj->m_levelID.value()));
            return result;
        }

        void resetLevel() {
            PlayLayer::resetLevel();

            static Mod* cbfMod = geode::Loader::get()->getLoadedMod("syzzi.click_between_frames");
            if (s_bot.getState() != bot::State::DISABLED) {
                if(cbfMod)
                    cbfMod->setSettingValue<bool>("soft-toggle", true);

                config::set<bool>("level.checkpointdelay", true);
                config::set<bool>("global.tpsbypass.toggle", true);
                config::set<float>("global.tpsbypass", s_bot.getFramerate());
            }

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
        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            if (s_bot.getState() != bot::State::PLAYBACK)
                return;

            std::optional<gdr::Input> input = std::nullopt;

            while ((input = s_bot.poll(m_gameState.m_currentProgress)) != std::nullopt) {
                auto performButton = input->down ? &PlayerObject::pushButton : &PlayerObject::releaseButton;
                if(m_levelSettings->m_twoPlayerMode) {
                    PlayerObject* player = input->player2 ? m_player2 : m_player1;
                    (player->*performButton)((PlayerButton)input->button);
                    continue;
                }

                (m_player1->*performButton)((PlayerButton)input->button);
                if(m_gameState.m_isDualMode)
                    (m_player2->*performButton)((PlayerButton)input->button);
            }
        }

        void handleButton(bool down, int button, bool player1) {
            GJBaseGameLayer::handleButton(down, button, player1);

            if (s_bot.getState() != bot::State::RECORD)
                return;

            bool realPlayer1 = m_levelSettings->m_twoPlayerMode ? player1 : player1 || !m_gameState.m_isDualMode;
            s_bot.recordInput(m_gameState.m_currentProgress, (PlayerButton) button, !realPlayer1, down);
        }
    };
}
