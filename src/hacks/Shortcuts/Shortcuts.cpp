#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

namespace eclipse::hacks::Shortcuts {

    class Shortcuts : public hack::Hack {
        using FileEvent = geode::Task<geode::Result<std::filesystem::path>>;

        static void openSettings() {
            if (auto* options = OptionsLayer::create()) {
                auto scene = cocos2d::CCScene::get();
                if (!scene) return;
                auto zOrder = scene->getHighestChildZ();
                scene->addChild(options, zOrder + 1);
                options->showLayer(false);
            }
        }

        static void uncompleteLevel() {
            auto scene = cocos2d::CCScene::get();
            if (!scene) return; // sometimes people frget CCScene can sometimes be nullptr for no reason
            GJGameLevel* level = nullptr;

            // try to find it from either PlayLayer or LevelInfoLayer
            if (auto* pl = PlayLayer::get()) {
                level = pl->m_level;
            } else if (auto* lil = scene->getChildByType<LevelInfoLayer>(0)) {
                level = lil->m_level;
            }

            if (!level) return Popup::create("Error", "You have to open the level first!");

            Popup::create(
                "Uncomplete level?",
                fmt::format("Are you sure you want to uncomplete \"{}\"?\nYou can't undo this process.", level->m_levelName),
                "Yes", "No",[level](bool yes) {
                    if (!yes) return;
                    auto gsm = GameStatsManager::sharedState();
                    auto glm = GameLevelManager::sharedState();
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
            if (auto* pl = PlayLayer::get())
                pl->resetLevel();
        }

        static void togglePracticeMode() {
            if (auto* pl = PlayLayer::get())
                pl->togglePracticeMode(!pl->m_isPracticeMode);
        }

        static void placeCheckpoint() {
            if (auto* pl = PlayLayer::get()) {
                if (pl->m_isPracticeMode)
                    pl->markCheckpoint();
            }
        }

        static void removeCheckpoint() {
            if (auto* pl = PlayLayer::get()) {
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
                { geode::dirs::getGameDir(), { filter }}
            ));
        }
#endif

        static void openSaveFolder() {
            auto path = geode::dirs::getSaveDir();
            geode::utils::file::openFolder(path);
        }

        void init() override {
            config::setIfEmpty("shortcut.p1jump", keybinds::Keys::None);
            config::setIfEmpty("shortcut.p2jump", keybinds::Keys::None);

            auto tab = gui::MenuTab::find("Shortcuts");
            tab->addKeybind("P1 Jump", "shortcut.p1jump", true)->setInternal();
            tab->addKeybind("P2 Jump", "shortcut.p2jump", true)->setInternal();
            tab->addButton("Show Options")->setDescription("Open game settings menu")->callback(openSettings)->handleKeybinds();
            tab->addButton("Uncomplete Level")->setDescription("Clear progress from a level")->callback(uncompleteLevel)->handleKeybinds();
            tab->addButton("Restart Level")->setDescription("Restart the current level")->callback(restartLevel)->handleKeybinds();
            tab->addButton("Toggle Practice Mode")->callback(togglePracticeMode)->handleKeybinds();
            tab->addButton("Place Checkpoint")->callback(placeCheckpoint)->handleKeybinds();
            tab->addButton("Remove Checkpoint")->callback(removeCheckpoint)->handleKeybinds();
            GEODE_WINDOWS(
                tab->addButton("Inject DLL")->setDescription("Pick a DLL file to inject")->callback(injectDll)->handleKeybinds();
            )
            tab->addButton("Save folder")->setDescription("Open the game's save folder")->callback(openSaveFolder)->handleKeybinds();

            auto manager = keybinds::Manager::get();
            manager->addListener("shortcut.p1jump", [](bool down) {
                auto gameLayer = GJBaseGameLayer::get();
                if (!gameLayer) return;
                gameLayer->handleButton(down, 1, true);
            });
            manager->addListener("shortcut.p2jump", [](bool down) {
                auto gameLayer = GJBaseGameLayer::get();
                if (!gameLayer) return;
                gameLayer->handleButton(down, 1, false);
            });
        }

        [[nodiscard]] const char* getId() const override { return "Shortcuts"; }
    };

    REGISTER_HACK(Shortcuts)

}
