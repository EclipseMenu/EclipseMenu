#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GameManager.hpp>

#ifdef GEODE_IS_WINDOWS
namespace eclipse::hacks::Global {

    class $hack(VerticalSync) {
    public:
        static void disableVSync() {
            config::setTemp("global.vsync", false);
            utils::get<GameManager>()->setGameVariable("0030", false);
            utils::get<AppDelegate>()->toggleVerticalSync(false);
        }

        void init() override {
            auto tab = gui::MenuTab::find("tab.global");

            tab->addToggle("global.vsync")
               ->handleKeybinds()
               ->setDescription()
               ->callback([](bool v) {
                   if (v) {
                       config::set("global.fpsbypass.toggle", false);
                       utils::get<GameManager>()->setGameVariable("0116", false);
                   }
                   utils::get<GameManager>()->setGameVariable("0030", v);
                   utils::get<AppDelegate>()->toggleVerticalSync(v);
               })
               ->disableSaving();
        }

        void lateInit() override {
            bool vsyncEnabled = utils::get<GameManager>()->getGameVariable("0030");
            config::setTemp("global.vsync", vsyncEnabled);
        }

        [[nodiscard]] const char* getId() const override { return "Vertical Sync"; }
        [[nodiscard]] int32_t getPriority() const override { return -5; }
    };
    REGISTER_HACK(VerticalSync)

    class $modify(VerticalSyncGMHook, GameManager) {
        void setGameVariable(char const* key, bool value) {
            GameManager::setGameVariable(key, value);
            if (strcmp(key, "0030") == 0) {
                config::setTemp("global.vsync", value);
                utils::get<AppDelegate>()->toggleVerticalSync(value);
            }
        }
    };
}
#endif