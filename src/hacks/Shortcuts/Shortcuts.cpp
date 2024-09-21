#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

namespace eclipse::hacks::Shortcuts {

    class Shortcuts : public hack::Hack {
        using FileEvent = geode::Task<geode::Result<std::filesystem::path>>;

        static void openSettings() {
            if (auto* options = OptionsLayer::create()) {
                cocos2d::CCScene::get()->addChild(options, 1000);
                options->showLayer(false);
            }
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