#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/LevelEditorLayer.hpp>

namespace eclipse::hacks::Creator {
    class SmoothTrail : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.creator");
            tab->addToggle("creator.smoothtrail")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Smooth Editor Trail"; }
    };

    REGISTER_HACK(SmoothTrail)

    class $modify(SmoothTrailLELHook, LevelEditorLayer) {
        ADD_HOOKS_DELEGATE("creator.smoothtrail")

        void postUpdate(float dt) override {
            m_trailTimer = 0.1f; // Force trail update every frame
            LevelEditorLayer::postUpdate(dt);
        }
    };
}
