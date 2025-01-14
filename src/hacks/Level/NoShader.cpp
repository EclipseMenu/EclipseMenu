#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/ShaderLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(NoShader) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.noshader")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] bool isCheating() const override { RETURN_CACHED_BOOL("level.noshader"); }
        [[nodiscard]] const char* getId() const override { return "No Shaders"; }
    };

    REGISTER_HACK(NoShader)

    class $modify(NoShaderSLHook, ShaderLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("level.noshader")

        void performCalculations() {
            m_state.m_usesShaders = false;
        }
    };
}
