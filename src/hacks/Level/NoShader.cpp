#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/ShaderLayer.hpp>

namespace eclipse::hacks::Level {
    class NoShader : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.noshader")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("level.noshader", false); }
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
