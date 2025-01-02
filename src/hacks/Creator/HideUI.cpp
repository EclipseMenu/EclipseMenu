#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/EditorUI.hpp>

namespace eclipse::hacks::Creator {

    class HideUI : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.creator");
            tab->addToggle("creator.hideui")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Hide UI"; }
    };

    REGISTER_HACK(HideUI)

    static bool s_lastState = false;

    class $modify(HideUIEUIHook, EditorUI) {
        void onUpdate(float) {
            const bool isHidden = config::get<bool>("creator.hideui", false);
            if (s_lastState == isHidden)
                return;

            s_lastState = isHidden;
            this->setVisible(!isHidden);
        }

        bool init(LevelEditorLayer* editorLayer) {
            if (!EditorUI::init(editorLayer)) return false;

            this->schedule(schedule_selector(HideUIEUIHook::onUpdate), 0.f);
            return true;
        }
    };

}
