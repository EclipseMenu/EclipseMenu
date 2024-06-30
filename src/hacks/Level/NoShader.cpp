#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/ShaderLayer.hpp>

namespace eclipse::hacks::Level {

    class NoShader : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("No Shaders", "level.noshader")->setDescription("Disables shaders");
        }

        [[nodiscard]] const char* getId() const override { return "No Shaders"; }
    };

    REGISTER_HACK(NoShader)

    class $modify(ShaderLayer) {
        void visit() {
            if (config::get<bool>("level.noshader", false))
                return CCNode::visit();
            ShaderLayer::visit();
        }
    };
}