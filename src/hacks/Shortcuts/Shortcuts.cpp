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
                            gsm->setStat("8", gsm->getStat("8") - level->m_coinsVerified);
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

    #ifdef GEODE_IS_ANDROID
        static void openDevtools() {
            // simple hack that will call onMoreGames, which should open devtools
            // calling it on CCScene just to make sure hook will not actually crash on nullptr in case someone else hooked it
            reinterpret_cast<MenuLayer*>(utils::get<cocos2d::CCScene>())->onMoreGames(nullptr);
        }
    #endif

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
