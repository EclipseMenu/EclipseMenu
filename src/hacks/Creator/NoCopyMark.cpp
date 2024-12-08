#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditLevelLayer.hpp>

namespace eclipse::hacks::Creator {

    class NoCopyMark : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.creator");

            tab->addToggle("creator.nocopymark")
                ->handleKeybinds()
                ->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "No (C) Mark"; }
    };

    REGISTER_HACK(NoCopyMark)

    class $modify(NoCopyMarkELLHook, EditLevelLayer) {
        ADD_HOOKS_DELEGATE("creator.nocopymark")

        void onShare(CCObject* sender) {
            this->m_level->m_originalLevel = 0;
            EditLevelLayer::onShare(sender);
        }
    };

}
