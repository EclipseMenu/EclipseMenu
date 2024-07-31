#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EffectGameObject.hpp>

namespace eclipse::hacks::Player {

    class NoHideTrigger : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("No Hide Trigger", "player.nohidetrigger")
                ->setDescription("Disables \"Hide Player\" triggers. (Players will still be hidden if an \"Options\" trigger is set up to do so.)")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Hide Trigger"; }
    };

    REGISTER_HACK(NoHideTrigger)

    class $modify(NoHideTriggerEGOHook, EffectGameObject) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("EffectGameObject::triggerObject");
        }

        void triggerObject(GJBaseGameLayer* bgl, int p1, gd::vector<int> const* p2) override {
            if (!config::get<bool>("player.nohidetrigger", false))
                return EffectGameObject::triggerObject(bgl, p1, p2);

            switch (m_objectID) {
                case 1612: // Hide Trigger
                case 1613: // Show Trigger
                    return;
                default:
                    return EffectGameObject::triggerObject(bgl, p1, p2);
            }
        }
    };

}
