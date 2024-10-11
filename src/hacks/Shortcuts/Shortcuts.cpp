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
            GJGameLevel* level = nullptr;

            // try to find it from either PlayLayer or LevelInfoLayer
            auto* pl = PlayLayer::get();
            if (pl) {
                level = pl->m_level;
            } else if (auto* lil = geode::cocos::getChildOfType<LevelInfoLayer>(cocos2d::CCScene::get(), 0)) {
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

                // Delete completion
                if (level->m_normalPercent >= 100 && gsm->hasCompletedLevel(level)) {
                    gsm->m_completedLevels->removeObjectForKey(fmt::format("c_{}", level->m_levelID.value()));
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

                // Remove coins
                auto coinDict = gsm->m_verifiedUserCoins;
                for (auto i = 0; i < level->m_coins; i++) {
                    auto key = level->getCoinKey(i + 1);
                    coinDict->removeObjectForKey(key);
                }

                // Save the level
                glm->saveLevel(level);
            });
        }

        static void restartLevel() {
            if (auto* pl = PlayLayer::get())
                pl->resetLevel();
        }

        static void togglePracticeMode() {
            if (auto* pl = PlayLayer::get())
                pl->togglePracticeMode(!pl->m_isPracticeMode);
        }

#ifdef GEODE_IS_WINDOWS
        static void injectDll() {
            static geode::EventListener<FileEvent> m_listener;
            geode::FileSetting::Filter filter;
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
            auto tab = gui::MenuTab::find("Shortcuts");
            tab->addButton("Show Options")->setDescription("Open game settings menu")->callback(openSettings)->handleKeybinds();
            tab->addButton("Uncomplete Level")->setDescription("Clear progress from a level")->callback(uncompleteLevel)->handleKeybinds();
            tab->addButton("Restart Level")->setDescription("Restart the current level")->callback(restartLevel)->handleKeybinds();
            tab->addButton("Toggle Practice Mode")->callback(togglePracticeMode)->handleKeybinds();
            GEODE_WINDOWS(
                tab->addButton("Inject DLL")->setDescription("Pick a DLL file to inject")->callback(injectDll)->handleKeybinds();
            )
            tab->addButton("Save folder")->setDescription("Open the game's save folder")->callback(openSaveFolder)->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Shortcuts"; }
    };

    REGISTER_HACK(Shortcuts)

}