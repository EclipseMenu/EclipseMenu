#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/LevelEditorLayer.hpp>

namespace eclipse::hacks::Creator {

    class SmoothTrail : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Creator");

            tab->addToggle("Smooth Editor Trail", "creator.smoothtrail")
                ->handleKeybinds()
                ->setDescription("Makes the trail in editor playback smoother.");
        }

        [[nodiscard]] const char* getId() const override { return "Smooth Editor Trail"; }
    };

    REGISTER_HACK(SmoothTrail)

    class $modify(LevelEditorLayer) {
        void postUpdate(float dt) {
            if (config::get("creator.smoothtrail", false))
                m_trailTimer = 0.1f; // Force trail update every frame

            LevelEditorLayer::postUpdate(dt);
        }
    };

}
