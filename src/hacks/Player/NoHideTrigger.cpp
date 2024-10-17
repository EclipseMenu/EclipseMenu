#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Player {

    class NoHideTrigger : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("No Hide Trigger", "player.nohidetrigger")
                ->setDescription("Keeps the player visible.")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Hide Trigger"; }
    };

    REGISTER_HACK(NoHideTrigger)

    class $modify(NoHideTriggerEGOHook, EffectGameObject) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.nohidetrigger")

        void triggerObject(GJBaseGameLayer* bgl, int p1, gd::vector<int> const* p2) override {
            switch (m_objectID) {
                case 1612: // Hide Trigger
                case 1613: // Show Trigger
                    return;
                default:
                    return EffectGameObject::triggerObject(bgl, p1, p2);
            }
        }
    };

    class $modify(NoHideTriggerGJBGLHook, GJBaseGameLayer) {
        ADD_HOOKS_DELEGATE("player.nohidetrigger")

        void processOptionsTrigger(GameOptionsTrigger* options) {
            auto originalHideP1 = options->m_hideP1;
            auto originalHideP2 = options->m_hideP2;
            options->m_hideP1 = GameOptionsSetting::Disabled;
            options->m_hideP2 = GameOptionsSetting::Disabled;
            GJBaseGameLayer::processOptionsTrigger(options);
            options->m_hideP1 = originalHideP1;
            options->m_hideP2 = originalHideP2;
        }
    };

}
