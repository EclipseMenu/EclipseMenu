#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/UILayer.hpp>

namespace eclipse::hacks::Level {
    class CheckpointDelay : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.checkpointdelay")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Checkpoint Delay Fix"; }
    };

    REGISTER_HACK(CheckpointDelay)

    class $modify(CheckpointDelayUILHook, UILayer) {
        ADD_HOOKS_DELEGATE("level.checkpointdelay")

        void onCheck(cocos2d::CCObject* sender) {
            UILayer::onCheck(sender);

            auto pl = utils::get<PlayLayer>();
            if(!pl) return;

            pl->m_tryPlaceCheckpoint = false;
            pl->markCheckpoint();
        }
    };
}