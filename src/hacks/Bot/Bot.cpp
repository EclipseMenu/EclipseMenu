#include <modules.hpp>
#include <modules/bot/bot.hpp>
#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/popup.hpp>
#include <modules/gui/cocos/cocos.hpp>
#include <modules/gui/components/button.hpp>
#include <modules/gui/components/combo.hpp>
#include <modules/gui/components/filesystem-combo.hpp>
#include <modules/gui/components/radio.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/i18n/translations.hpp>

#include <Geode/modify/CheckpointObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

namespace eclipse::hacks::Bot {
    static bot::Bot s_bot;
    // static bool s_respawning = false;
    static bool s_dontPlaceAuto = false;
    bot::Bot& getBot() { return s_bot; }

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
                    config::set<std::string_view>("bot.selectedreplay", "");

                    // refresh cocos ui page
                    if (auto cocos = gui::cocos::CocosRenderer::get())
                        cocos->refreshPage();
                }
            }
        );
    }
    void openReplaysFolder() {
        auto replaysDir = Mod::get()->getSaveDir() / "replays";
        std::error_code ec;
        
        // create the folder if it doesn't exist
        if (!std::filesystem::exists(replaysDir, ec)) {
            std::filesystem::create_directory(replaysDir, ec);
            if (ec) {
                return Popup::create(
                    i18n::get_("common.error"),
                    ec.message()
                );
            }
        }
        
        geode::utils::file::openFolder(replaysDir);
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

                // disable Click Between Steps
                GameManager::get()->setGameVariable(GameVar::ClickBetweenSteps, false);
                if (auto gl = utils::get<GJBaseGameLayer>()) {
                    gl->m_clickBetweenSteps = false;
                }
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

#ifdef GEODE_IS_WINDOWS
            config::set("bot.practice-fix-mode", 0); // TODO: change to 1 when memory practice fix is fixed
#else
            config::set("bot.practice-fix-mode", 0);
#endif
            config::set("bot.state", 0);
            restorePreBotSettings();
            updateBotState(0);

            auto tab = gui::MenuTab::find("tab.bot");

            tab->addRadioButton("bot.disabled", "bot.state", 0)->callback(updateBotState)->handleKeybinds();
            tab->addRadioButton("bot.record", "bot.state", 1)->callback(updateBotState)->handleKeybinds();
            tab->addRadioButton("bot.playback", "bot.state", 2)->callback(updateBotState)->handleKeybinds();

            tab->addFilesystemCombo("bot.replays", "bot.selectedreplay", Mod::get()->getSaveDir() / "replays");
#ifdef GEODE_IS_WINDOWS
            tab->addCombo("bot.practice-fix-mode", {"Checkpoint", "Memory"}, 0)->setDescription();
#endif

            tab->addToggle("bot.ignore-inputs")->handleKeybinds()->setDescription();

            tab->addButton("common.new")->handleKeybinds()->callback(newReplay);
            tab->addButton("common.save")->handleKeybinds()->callback(saveReplay);
            tab->addButton("common.load")->handleKeybinds()->callback(loadReplay);
            tab->addButton("common.delete")->handleKeybinds()->callback(deleteReplay);
            tab->addButton("bot.open-replays-folder")->handleKeybinds()->callback(openReplaysFolder);
        }

        [[nodiscard]] bool isCheating() const override {
            auto state = config::get<"bot.state", int>(0);
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
            s_bot.setPlatformer(gj->isPlatformer());
            // s_respawning = false;
            return result;
        }

        void resetLevel() {
            // s_respawning = true;
            PlayLayer::resetLevel();
            Bot::applySettings();
            
            if (m_checkpointArray->count() == 0) {
                if(s_bot.getState() != bot::State::PLAYBACK)
                    s_bot.clearInputs();
                s_bot.restart();
            }
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

            s_bot.removeInputsAfter(checkpoint->m_gameState.m_currentProgress / 2);

            PlayLayer::loadFromCheckpoint(checkpoint);
        }

        void pauseGame(bool unfocused) {
            PlayLayer::pauseGame(unfocused);
            if (s_bot.getState() != bot::State::RECORD)
                return;

            // here robtop calls releaseAllButtons on player1 twice. i'm assuming this is a bug and will be fixed in the next versions.
            // if that's the case, then add this for player2 as well.
            bool swapControls = GameManager::get()->getGameVariable(GameVar::Flip2PlayerControls);
            for(int button = 1; button <= 3; button++) {
                s_bot.recordInput(
                    // + 1 because while the input happens on this frame, it will only get processed on the next frame
                    // because of pausing. since we don't pause on playback simulate this by adding one frame
                    (m_gameState.m_currentProgress / 2) + 1,
                    (PlayerButton)button, swapControls, false
                );
            }
        }
    };

    class $modify(BotPlayerHook, PlayerObject) {
        struct Fields {
            bool m_triedPlacingCheckpoint = false;
        };

        static void onModify(auto& self) {
            int value = config::get("bot.state", 0);
            geode::Hook* hookPtr = nullptr;
            auto it = self.m_hooks.find("PlayerObject::tryPlaceCheckpoint");
            if (it != self.m_hooks.end()) {
                it->second->setAutoEnable(value == (int)bot::State::RECORD);
                it->second->setPriority(SAFE_HOOK_PRIORITY);
                hookPtr = it->second.get();
            } else {
                geode::log::warn("Hook 'tryPlaceCheckpoint' not found in class 'PlayerObject'");
            }
            config::addDelegate("bot.state", [hookPtr] {
                int value = config::get("bot.state", 0);
                (void) hookPtr->toggle(value == (int)bot::State::RECORD);
            });
        }

        void tryPlaceCheckpoint() {
            if(s_dontPlaceAuto) {
                m_fields->m_triedPlacingCheckpoint = true;
                return;
            }
            PlayerObject::tryPlaceCheckpoint();
        }
    };

    class $modify(BotBGLHook, GJBaseGameLayer) {

        static void onModify(auto& self) {
            SAFE_HOOKS(GJBaseGameLayer, "processQueuedButtons");
            
            int value = config::get("bot.state", 0);
            geode::Hook* hookPtr = nullptr;
            auto it = self.m_hooks.find("GJBaseGameLayer::update");
            if (it != self.m_hooks.end()) {
                it->second->setAutoEnable(value == (int)bot::State::RECORD);
                it->second->setPriority(SAFE_HOOK_PRIORITY);
                hookPtr = it->second.get();
            } else {
                geode::log::warn("Hook 'update' not found in class 'GJBaseGameLayer'");
            }
            config::addDelegate("bot.state", [hookPtr] {
                int value = config::get("bot.state", 0);
                (void) hookPtr->toggle(value == (int)bot::State::RECORD);
            });
        }

        void update(float dt) {
            auto player1Fields = reinterpret_cast<BotPlayerHook*>(m_player1)->m_fields.self();
            auto player2Fields = reinterpret_cast<BotPlayerHook*>(m_player2)->m_fields.self();

            player1Fields->m_triedPlacingCheckpoint = false;
            player2Fields->m_triedPlacingCheckpoint = false;
            s_dontPlaceAuto = true;
            GJBaseGameLayer::update(dt);
            s_dontPlaceAuto = false;

            if(player1Fields->m_triedPlacingCheckpoint) {
                m_player1->m_shouldTryPlacingCheckpoint = true;
                m_player1->tryPlaceCheckpoint();
            }
            if(player2Fields->m_triedPlacingCheckpoint) {
                m_player2->m_shouldTryPlacingCheckpoint = true;
                m_player2->tryPlaceCheckpoint();
            }
        }

        void simulateClick(PlayerButton button, bool down, bool player2) {
            auto performButton = down ? &PlayerObject::pushButton : &PlayerObject::releaseButton;
            bool swapControls = GameManager::get()->getGameVariable(GameVar::Flip2PlayerControls);
            player2 = swapControls ? !player2 : player2;

            // in two player mode, only one player should be controlled
            if (m_levelSettings->m_twoPlayerMode) {
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

        void processBot() {
            // if(s_respawning) s_respawning = false;

            if (s_bot.getState() != bot::State::PLAYBACK)
                return;

            std::optional<gdr::Input<>> input = std::nullopt;

            // TODO: 2.208 made m_currentProgress count twice as fast, for now we just divide it by 2
            // to avoid breaking existing replays. Find a better solution later
            auto progress = m_gameState.m_currentProgress / 2;

            while ((input = s_bot.poll(progress)) != std::nullopt) {
                #ifdef GEODE_IS_MOBILE
                m_allowedButtons.clear();
                #endif
                this->simulateClick((PlayerButton) input->button, input->down, input->player2);
            }
        }

        void processQueuedButtons(float dt, bool clearInputQueue) {
            GJBaseGameLayer::processQueuedButtons(dt, clearInputQueue);
            this->processBot();
        }

        void handleButton(bool down, int button, bool player1) {
            if (s_bot.getState() == bot::State::PLAYBACK && s_bot.getInputCount() && config::get<bool>("bot.ignore-inputs", false))
                return;

            // does not seem to be happening anymore in 2.208. leaving this here in case it pops up again
            // if (s_bot.getState() == bot::State::RECORD && s_respawning) { // somehow this avoid a weird bug where an orb buffered if you press down while respawning doesn't register
            //     bool swapControls = GameManager::get()->getGameVariable("0010");
            //     bool player2 = swapControls ? player1 : !player1;
            //     PlayerObject* checkPlayer = player2 ? m_player2 : m_player1;
            //     if(checkPlayer->m_touchedRings.size() > 0) return;
            // }

            GJBaseGameLayer::handleButton(down, button, player1);

            if (s_bot.getState() != bot::State::RECORD)
                return;

            s_bot.recordInput(
                m_gameState.m_currentProgress / 2,
                (PlayerButton) button, !player1, down
            );
        }
    };

    // this fixes bot playback in level editor (robtop doesn't reset the counter)
    class $modify(BotEUIHook, EditorUI) {
        void onPlaytest(CCObject* sender) {
            if (auto* editorLayer = utils::get<LevelEditorLayer>()) {
                if (editorLayer->m_playbackMode == PlaybackMode::Not) {
                    s_bot.restart();
                    editorLayer->m_gameState.m_currentProgress = 0;
                }
            }
            EditorUI::onPlaytest(sender);
        }
    };
}