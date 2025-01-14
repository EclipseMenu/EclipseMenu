#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/EffectGameObject.hpp>

namespace eclipse::hacks::Level {
    class $hack(MuteLevelSFX) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.mutelevelsfx")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Mute Level SFX"; }
    };

    REGISTER_HACK(MuteLevelSFX)

    class $modify(MuteLevelSFXEGOHook, EffectGameObject) {
        ADD_HOOKS_DELEGATE("level.mutelevelsfx")

        void triggerObject(GJBaseGameLayer* gjbgl, int p1, gd::vector<int> const* p2) override {
            if (!utils::get<PlayLayer>() || (this->m_objectID != 3602 && this->m_objectID != 3603))
                return EffectGameObject::triggerObject(gjbgl, p1, p2);
        }
    };
}
