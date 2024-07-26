#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EffectGameObject.hpp>

namespace eclipse::hacks::Level {

    class MuteLevelSFX : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Mute Level SFX", "level.mutelevelsfx")
                ->setDescription("Disables SFX/Edit SFX triggers. (Created by RayDeeUx)")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Mute Level SFX"; }
    };

    REGISTER_HACK(MuteLevelSFX)

    class $modify(EffectGameObject) {
        void triggerObject(GJBaseGameLayer* gjbgl, int p1, gd::vector<int> const* p2) {
            if (!PlayLayer::get() || !config::get<bool>("level.mutelevelsfx", false))
                return EffectGameObject::triggerObject(gjbgl, p1, p2);
            if (this->m_objectID != 3602 && this->m_objectID != 3603)
                return EffectGameObject::triggerObject(gjbgl, p1, p2);
        }
    };
}