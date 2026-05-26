#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(AutoDisableShake) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.autodisableshake")->handleKeybinds()->setDescription();
        }
        [[nodiscard]] const char* getId() const override { return "Auto Disable Shake"; }
    };
    REGISTER_HACK(AutoDisableShake)

    class $modify(AutoDisableShakeHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("level.autodisableshake")
        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            if (!PlayLayer::init(level, useReplay, dontCreateObjects))
                return false;
            m_gameState.m_disableShake = true;
            return true;
        }
    };
}
