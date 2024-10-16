#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class NoGlow : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("No Object Glow", "level.noglow")
                ->handleKeybinds()
                ->setDescription("Disables all glow coming from objects");
        }

        [[nodiscard]] const char* getId() const override { return "No Object Glow"; }
    };

    REGISTER_HACK(NoGlow)

    class $modify(NoGlowPLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("level.noglow")

        void addObject(GameObject* obj) {
            obj->m_hasNoGlow = true;
            PlayLayer::addObject(obj);
        }
    };
}
