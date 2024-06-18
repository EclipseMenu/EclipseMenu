//from prism its superseded anyways im lazy
#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditLevelLayer.hpp>

namespace eclipse::hacks::Creator {

    class NoCopyMark : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Creator");
            tab->addToggle("No (C) Mark", "creator.nocopymark")->setDescription("Removes the copy mark from your levels when uploading");
        }

        void update() override {}
        [[nodiscard]] const char* getId() const override { return "No (C) Mark"; }
    };

    REGISTER_HACK(NoCopyMark)

    class $modify(EditLevelLayer) {
        // Verify Hack, No (C) Mark
        void onShare(CCObject* sender) {
        if (config::get<bool>("creator.nocopymark", false)) {
            this->m_level->m_originalLevel = 0;
        }
            EditLevelLayer::onShare(sender);
        }
    };

}
