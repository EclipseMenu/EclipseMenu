#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GameManager.hpp>

namespace eclipse::hacks::Global {

    class $hack(VerticalSync) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");

            tab->addToggle("global.vsync")
               ->handleKeybinds()
               ->setDescription()
               ->callback([](bool v) {
                   GameManager::sharedState()->setGameVariable("0030", v);
                   AppDelegate::get()->toggleVerticalSync(v);
               })
               ->disableSaving();
        }

        void lateInit() override {
            bool vsyncEnabled = GameManager::sharedState()->getGameVariable("0030");
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
                AppDelegate::get()->toggleVerticalSync(value);
            }
        }
    };
}
