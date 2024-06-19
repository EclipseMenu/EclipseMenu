#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/LevelInfoLayer.hpp>

namespace eclipse::hacks::Creator {

    class CopyHack : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Creator");
            tab->addToggle("Copy Hack", "creator.copyhack")->setDescription("Allows for unrestricted copying of any level");
        }

        void update() override {}
        [[nodiscard]] const char* getId() const override { return "Copy Hack"; }
    };

    REGISTER_HACK(CopyHack)
    using namespace geode::prelude;
    class $modify(LevelInfoLayer) {
        // Copy Hack
        bool init(GJGameLevel *p0, bool p1) {
            if (!LevelInfoLayer::init(p0, p1)) return false;
            if (config::get<bool>("creator.copyhack", false)) {
                auto gm = GameManager::sharedState();
                if (gm->m_playerUserID == p0->m_userID) return true;
                if (m_cloneBtn == nullptr) return true;
                auto aCloneBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png"), this, menu_selector(LevelInfoLayer::confirmClone));
                aCloneBtn->setPosition(m_cloneBtn->getPosition());
                m_cloneBtn->getParent()->addChild(aCloneBtn);
                m_cloneBtn->setVisible(false);
            }
            return true;
        };
    };
}
