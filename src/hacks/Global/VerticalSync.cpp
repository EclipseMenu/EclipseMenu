#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#ifdef GEODE_IS_WINDOWS
namespace eclipse::hacks::Global {

    class $hack(VerticalSync) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");

            tab->addToggle("global.vsync")
               ->handleKeybinds()
               ->setDescription()
               ->callback([](bool v) {
                   utils::get<GameManager>()->setGameVariable(GameVar::VerticalSync, v);
                   utils::get<AppDelegate>()->toggleVerticalSync(v);
               })
               ->disableSaving();
        }

        [[nodiscard]] const char* getId() const override { return "Vertical Sync"; }
        [[nodiscard]] int32_t getPriority() const override { return -5; }
    };
    REGISTER_HACK(VerticalSync)
}
#endif