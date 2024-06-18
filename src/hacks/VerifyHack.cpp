//from prism its superseded anyways im lazy
#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditLevelLayer.hpp>

namespace eclipse::hacks::Creator {

    class VerifyHack : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Creator");
            tab->addToggle("Verify Hack", "creator.verifyhack")->setDescription("Created levels don\'t need to be beaten to be uploaded");
        }

        void update() override {}
        [[nodiscard]] const char* getId() const override { return "Verify Hack"; }
    };

    REGISTER_HACK(VerifyHack)

    class $modify(EditLevelLayer) {
        // Verify Hack, No (C) Mark
        void onShare(CCObject* sender) {
        if (!config::get<bool>("creator.verifyhack", false) && !config::get<bool>("creator.nocopymark", false)) return EditLevelLayer::onShare(sender);
        if (config::get<bool>("creator.verifyhack", false)) {
            this->m_level->m_isVerified = true;
            //this->m_level->m_isVerified = isVerified;
        }
            EditLevelLayer::onShare(sender);
        }
    };

}
