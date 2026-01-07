#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GameManager.hpp>

#ifdef GEODE_IS_WINDOWS
namespace eclipse::hacks::Global {

    class $hack(VerticalSync) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");

            tab->addToggle("global.vsync")
               ->handleKeybinds()
               ->setDescription()
               ->callback([](bool v) {
                   if (v) {
                       config::set("global.fpsbypass.toggle", false);
                       utils::get<GameManager>()->setGameVariable(GameVar::UnlockFPS, false);
                   }
                   utils::get<GameManager>()->setGameVariable(GameVar::VerticalSync, v);
                   utils::get<AppDelegate>()->toggleVerticalSync(v);
               })
               ->disableSaving();
        }

        void lateInit() override {
            bool vsyncEnabled = utils::get<GameManager>()->getGameVariable(GameVar::VerticalSync);
            config::setTemp("global.vsync", vsyncEnabled);
        }

        [[nodiscard]] const char* getId() const override { return "Vertical Sync"; }
        [[nodiscard]] int32_t getPriority() const override { return -5; }
    };
    REGISTER_HACK(VerticalSync)

    class $modify(VerticalSyncGMHook, GameManager) {
        void setGameVariable(char const* key, bool value) {
            GameManager::setGameVariable(key, value);
            if (strcmp(key, GameVar::VerticalSync) == 0) {
                config::setTemp("global.vsync", value);
                utils::get<AppDelegate>()->toggleVerticalSync(value);
            }
        }
    };
}
#endif