#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/button.hpp>
#include <modules/gui/components/keybind.hpp>
#include <modules/hack/hack.hpp>
#include <modules/i18n/translations.hpp>

namespace eclipse::hacks::Shortcuts {
    class $hack(Shortcuts) {
        using FileEvent = geode::Task<geode::Result<std::filesystem::path>>;

        static void openSettings() {
            if (auto* options = OptionsLayer::create()) {
                auto scene = utils::get<cocos2d::CCScene>();
                if (!scene) return;
                auto zOrder = scene->getHighestChildZ();
                scene->addChild(options, zOrder + 1);
                options->showLayer(false);
            }
        }

        static void uncompleteLevel() {
            auto scene = utils::get<cocos2d::CCScene>();
            if (!scene) return; // sometimes people frget CCScene can sometimes be nullptr for no reason
            GJGameLevel* level = nullptr;

            // try to find it from either PlayLayer or LevelInfoLayer
            if (auto* pl = utils::get<PlayLayer>()) {
                level = pl->m_level;
            } else if (auto* lil = scene->getChildByType<LevelInfoLayer>(0)) {
                level = lil->m_level;
            }

            if (!level)
                return Popup::create(
                    i18n::get_("common.error"),
                    i18n::get_("shortcuts.uncomplete-level.error")
                );

            Popup::create(
                i18n::get_("shortcuts.uncomplete-level.title"),
                i18n::format("shortcuts.uncomplete-level.msg", level->m_levelName),
                i18n::get_("common.yes"), i18n::get_("common.no"),
                [level](bool yes) {
                    if (!yes) return;
                    auto gsm = utils::get<GameStatsManager>();
                    auto glm = utils::get<GameLevelManager>();
                    // if level had been completed, ensure that their stars are also removed if the said level was rated
                    if (level->m_normalPercent >= 100 && gsm->hasCompletedLevel(level)) {
                        int levelid = level->m_levelID.value();
                        // Delete completion
                        gsm->setStat("4", gsm->getStat("4") - 1); // completed levels
                        gsm->m_completedLevels->removeObjectForKey(fmt::format("c_{}", levelid));
                        if (level->m_stars > 0) {
                            //GSM->m_completedLevels->removeObjectForKey(fmt::format("unique_{}", levelid));
                            gsm->m_completedLevels->removeObjectForKey(fmt::format("star_{}", levelid));
                            gsm->m_completedLevels->removeObjectForKey(fmt::format("demon_{}", levelid));
                            if (level->isPlatformer()) {
                                gsm->setStat("28", gsm->getStat("28") - level->m_stars); // moons
                            } else {
                                gsm->setStat("6", gsm->getStat("6") - level->m_stars); // stars
                            }
                            if (level->m_demon > 0) {
                                gsm->setStat("5", gsm->getStat("5") - 1); // demons
                            }
                            auto coinDict = gsm->m_verifiedUserCoins;
                            for (auto i = 0; i < level->m_coins; i++) {
                                auto key = level->getCoinKey(i + 1);
                                if (gsm->hasUserCoin(key) || gsm->hasPendingUserCoin(key)) {
                                    gsm->setStat("12", gsm->getStat("12") - 1);
                                }
                            }
                        }
                    }

                    // Clear progress
                    level->m_practicePercent = 0;
                    level->m_normalPercent = 0;
                    level->m_newNormalPercent2 = 0;
                    level->m_orbCompletion = 0;
                    level->m_54 = 0;
                    level->m_k111 = 0;
                    level->m_bestPoints = 0;
                    level->m_bestTime = 0;

                    /* // unsure if this is needed
                    level->m_isChkValid = 0;
                    level->m_chk = 0;
                    level->m_coinsVerified = 0;
                    */

                    // Remove coins
                    auto coinDict = gsm->m_verifiedUserCoins;
                    for (auto i = 0; i < level->m_coins; i++) {
                        auto key = level->getCoinKey(i + 1);
                        coinDict->removeObjectForKey(key);
                    }

                    // Save the level
                    glm->saveLevel(level);
                }
            );
        }

        static void restartLevel() {
            if (auto* pl = utils::get<PlayLayer>())
                pl->resetLevel();
        }

        static void togglePracticeMode() {
            if (auto* pl = utils::get<PlayLayer>())
                pl->togglePracticeMode(!pl->m_isPracticeMode);
        }

        static void placeCheckpoint() {
            if (auto* pl = utils::get<PlayLayer>()) {
                if (pl->m_isPracticeMode)
                    pl->markCheckpoint();
            }
        }

        static void removeCheckpoint() {
            if (auto* pl = utils::get<PlayLayer>()) {
                if (pl->m_isPracticeMode)
                    pl->removeCheckpoint(false);
            }
        }

        #ifdef GEODE_IS_WINDOWS
        static void injectDll() {
            static geode::EventListener<FileEvent> m_listener;
            geode::utils::file::FilePickOptions::Filter filter;
            filter.description = "Dynamic Link Library (*.dll)";
            filter.files.insert("*.dll");

            m_listener.bind([](FileEvent::Event* event) {
                if (auto value = event->getValue()) {
                    auto path = value->unwrapOr("");
                    if (path.empty() || !std::filesystem::exists(path))
                        return;

                    geode::log::warn("Injecting DLL: {}", path);
                    HMODULE module = LoadLibraryA(path.string().c_str());
                    if (!module) return geode::log::error("Failed to inject DLL: {}", path);

                    // Call DLLMain with DLL_PROCESS_ATTACH
                    bool success = module > (HMODULE) HINSTANCE_ERROR;
                    if (success) {
                        using DllMain = BOOL(WINAPI*)(HINSTANCE, DWORD, LPVOID);
                        auto dllMain = reinterpret_cast<DllMain>(GetProcAddress(module, "DllMain"));
                        if (dllMain)
                            dllMain(static_cast<HINSTANCE>(module), DLL_PROCESS_ATTACH, nullptr);
                    } else {
                        FreeLibrary(module);
                        geode::log::error("Failed to inject DLL: {}", path);
                    }
                }
            });
            m_listener.setFilter(geode::utils::file::pick(
                geode::utils::file::PickMode::OpenFile,
                {geode::dirs::getGameDir(), {filter}}
            ));
        }
        #endif

        static void openSaveFolder() {
            auto path = geode::Mod::get()->getSaveDir();
            geode::utils::file::openFolder(path);
        }
        static void resetBGVolume() {
            auto fmod = FMODAudioEngine::sharedEngine();
            fmod->setBackgroundMusicVolume(1.F);
            auto backgroundVolume = fmod->getBackgroundMusicVolume();
            fmod->setBackgroundMusicVolume(1.F);
            if (backgroundVolume <= 0.0) {
                if (auto GM = GameManager::sharedState()) {
                    GM->playMenuMusic();
                }
            }
        }
        static void resetSFXVolume() {
            FMODAudioEngine::sharedEngine()->setEffectsVolume(1.F);
        }

    #ifdef GEODE_IS_ANDROID
        static void openDevtools() {
            // simple hack that will call onMoreGames, which should open devtools
            // calling it on CCScene just to make sure hook will not actually crash on nullptr in case someone else hooked it
            reinterpret_cast<MenuLayer*>(utils::get<cocos2d::CCScene>())->onMoreGames(nullptr);
        }
    #endif


        static int getSecretCoinsRange(int min, int max) {
            auto glm = utils::get<GameLevelManager>();
            auto gsm = utils::get<GameStatsManager>();
            int secretCoins = 0;
            for (size_t i = min; i < max; i++) {
                auto level = glm->getMainLevel(i, true);
                if (gsm->hasCompletedMainLevel(i)) {
                    for (auto ix = 0; ix < 3; ix++) {
                        if (gsm->hasSecretCoin(level->getCoinKey(ix + 1))) {
                            secretCoins++;
                        }
                    }
                }
            }
            return secretCoins;
        }
        static void recountSecretCoins() {

            auto am = AchievementManager::sharedState();
            auto glm = utils::get<GameLevelManager>();
            auto gsm = utils::get<GameStatsManager>();
            int newSecretCoins = 0;
            //gsm->hasCompletedMainLevel(1);
            //https://wyliemaster.github.io/gddocs/#/resources/client/gamesave/GS_Value
            auto mapPacks = gsm->getCompletedMapPacks();
            //std::unordered_set<int> demonPacks = {19,20,21,22,26,27,28,29,30,31,46,47,48,49,50,64,65,66};
            // official levels
            newSecretCoins += getSecretCoinsRange(1, 23);
            newSecretCoins += getSecretCoinsRange(5001, 5005);
            // map packs
            for (size_t i = 0; i < mapPacks->count(); i++) {
                // so mr robert doesnt have a dict for coins, but for stars!? this is rigged!
                // looking at GameManager::verifyCoinUnlocks, robert simply has a huge CCString CCArray of map packs to parse from to get the secret coins, that sounds very inefficient!
                int key = mapPacks->stringAtIndex(i)->intValue();
                //if (demonPacks.find(key) != demonPacks.end()) {
                //    newSecretCoins += 2;
                //} else {
                //    newSecretCoins += 1;
                //}

                // anyway, most (actually all) demon map packs offer 2 secret coins, sooo...
                if (gsm->m_completedMappacks->valueForKey(fmt::format("pack_{}", key))->intValue() == 10) {
                    newSecretCoins += 2;
                } else {
                    newSecretCoins += 1;
                }
            }
            if (gsm->hasSecretCoin("secret04")) {
                newSecretCoins++;
            }
            if (gsm->hasSecretCoin("secret06")) {
                newSecretCoins++;
            }
            if (gsm->hasSecretCoin("secretB03")) {
                newSecretCoins++;
            }
            if (gsm->getStat("8") == newSecretCoins) return Popup::create(i18n::get_("common.info"), i18n::get_("shortcuts.recount-secret-coins.nochange"));
            if (gsm->getStat("8") > newSecretCoins) return Popup::create(i18n::get_("common.error"), i18n::format("shortcuts.recount-secret-coins.error", gsm->getStat("8"), newSecretCoins));
            Popup::create(
                i18n::get_("shortcuts.recount-secret-coins.title"),
                i18n::format("shortcuts.recount-secret-coins.msg", gsm->getStat("8"), newSecretCoins),
                i18n::get_("common.yes"), i18n::get_("common.no"),
                [newSecretCoins, gsm](bool yes) {
                    if (!yes) return;
                    if (gsm->getStat("8") > newSecretCoins) return;
                    gsm->setStat("8", newSecretCoins);
                }
            );
        }

        void init() override {
            config::setIfEmpty("shortcut.p1jump", keybinds::Keys::None);
            config::setIfEmpty("shortcut.p2jump", keybinds::Keys::None);

            auto tab = gui::MenuTab::find("tab.shortcuts");
            #ifdef GEODE_IS_DESKTOP
            tab->addKeybind("shortcuts.p1jump", "shortcut.p1jump", true)->setInternal();
            tab->addKeybind("shortcuts.p2jump", "shortcut.p2jump", true)->setInternal();
            #endif
            tab->addButton("shortcuts.options")->setDescription()->callback(openSettings)->handleKeybinds();
            tab->addButton("shortcuts.uncomplete-level")->setDescription()->callback(uncompleteLevel)->handleKeybinds();
            tab->addButton("shortcuts.restart-level")->setDescription()->callback(restartLevel)->handleKeybinds();
            tab->addButton("shortcuts.toggle-practice")->setDescription()->callback(togglePracticeMode)->handleKeybinds();
            tab->addButton("shortcuts.place-checkpoint")->setDescription()->callback(placeCheckpoint)->handleKeybinds();
            tab->addButton("shortcuts.remove-checkpoint")->setDescription()->callback(removeCheckpoint)->handleKeybinds();
            GEODE_WINDOWS(
                tab->addButton("shortcuts.inject-dll")->setDescription()->callback(injectDll)->handleKeybinds();
            )
            tab->addButton("shortcuts.save-folder")->setDescription()->callback(openSaveFolder)->handleKeybinds();
            tab->addButton("shortcuts.reset-bg-volume")->setDescription()->callback(resetBGVolume)->handleKeybinds();
            tab->addButton("shortcuts.reset-sfx-volume")->setDescription()->callback(resetSFXVolume)->handleKeybinds();
            tab->addButton("shortcuts.recount-secret-coins")->setDescription()->callback(recountSecretCoins)->handleKeybinds();

            auto manager = keybinds::Manager::get();
            manager->addListener("shortcut.p1jump", [](bool down) {
                auto gameLayer = utils::get<GJBaseGameLayer>();
                if (!gameLayer) return;
                gameLayer->queueButton(1, down, false);
            });
            manager->addListener("shortcut.p2jump", [](bool down) {
                auto gameLayer = utils::get<GJBaseGameLayer>();
                if (!gameLayer) return;
                gameLayer->queueButton(1, down, true);
            });
        }

        GEODE_ANDROID(void lateInit() override {
            auto devtools = geode::Loader::get()->getLoadedMod("geode.devtools");
            if (devtools) {
                gui::MenuTab::find("tab.shortcuts")
                    ->addButton("shortcuts.devtools")
                    ->setDescription()
                    ->callback(openDevtools)
                    ->handleKeybinds();
            }
        })

        [[nodiscard]] const char* getId() const override { return "Shortcuts"; }
    };

    REGISTER_HACK(Shortcuts)
}
