#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(AutoDisableShake) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("Auto Disable Shake")->handleKeybinds()->setDescription();
        }
        [[nodiscard]] const char* getId() const override { return "Auto Disable Shake"; }
    };
    REGISTER_HACK(AutoDisableShake)

    class $modify(AutoDisableShakeHook, GJBaseGameLayer) {
        ADD_HOOKS_DELEGATE("Auto Disable Shake")
        void shakeCamera(float strength, float duration, float interval) {
            // do nothing
        }
    };
}
