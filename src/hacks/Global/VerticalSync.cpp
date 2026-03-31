#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#ifdef GEODE_IS_DESKTOP

namespace eclipse::hacks::Global {

    class $hack(VerticalSync) {
        #ifdef GEODE_IS_MACOS
        static void toggleSDLVSync(bool enabled) {
            static auto sdl = geode::Loader::get()->getInstalledMod("zmx.sdl");
            if (!sdl || !sdl->isLoaded()) return;

            sdl->setSettingValue("disable-vsync", !enabled);
        }

        void lateInit() override {
            auto sdl = geode::Loader::get()->getInstalledMod("zmx.sdl");
            if (!sdl || !sdl->isLoaded()) return;

            config::setTemp("global.vsync", !sdl->getSettingValue<bool>("disable-vsync"));
            geode::listenForSettingChanges<bool>("disable-vsync", [](bool enabled) {
                config::setTemp("global.vsync", !enabled);
            }, sdl);
        }
        #endif

        void init() override {
            #ifdef GEODE_IS_MACOS
            auto sdl = geode::Loader::get()->getInstalledMod("zmx.sdl");
            if (!sdl || !sdl->shouldLoad()) return;
            #endif

            auto tab = gui::MenuTab::find("tab.global");

            tab->addToggle("global.vsync")
               ->handleKeybinds()
               ->setDescription()
               ->callback([](bool v) {
                #ifdef GEODE_IS_WINDOWS
                   utils::get<GameManager>()->setGameVariable(GameVar::VerticalSync, v);
                   utils::get<AppDelegate>()->toggleVerticalSync(v);
                #else
                   toggleSDLVSync(v);
                #endif
               })
               ->disableSaving();
        }

        [[nodiscard]] const char* getId() const override { return "Vertical Sync"; }
        [[nodiscard]] int32_t getPriority() const override { return -5; }
    };
    REGISTER_HACK(VerticalSync)
}
#endif