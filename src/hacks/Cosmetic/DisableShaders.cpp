//from prism its superseded anyways im lazy
#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/ShaderLayer.hpp>

namespace eclipse::hacks::Cosmetic {

    class DisableShaders : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Cosmetic");
            tab->addToggle("Disable Shaders", "cosmetic.disableshaders")->setDescription("Disables shaders");
        }

        void update() override {}
        [[nodiscard]] const char* getId() const override { return "Disable Shaders"; }
    };

    REGISTER_HACK(DisableShaders)

    class $modify(ShaderLayer) {
        void performCalculations() {
            if (!config::get<bool>("cosmetic.disableshaders", false)) return ShaderLayer::performCalculations();
        }
    };

}
