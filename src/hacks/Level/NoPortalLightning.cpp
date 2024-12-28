#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {

    class NoPortalLightning final : public hack::Hack {
        void init() override {
            gui::MenuTab::find("tab.level")
                ->addToggle("level.noportallightning")
                ->setDescription()
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Portal Lightning"; }
    };

    REGISTER_HACK(NoPortalLightning)

    class $modify(NoPortalLightningGJBGLHook, GJBaseGameLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("level.noportallightning")

        void lightningFlash(cocos2d::CCPoint from, cocos2d::CCPoint to, cocos2d::ccColor3B color, float lineWidth, float duration, int displacement, bool flash, float opacity) {
            auto* gm = utils::get<GameManager>();
            auto perfMode = gm->m_performanceMode;
            gm->m_performanceMode = true;
            flash = false;
            GJBaseGameLayer::lightningFlash(from, to, color, lineWidth, duration, displacement, flash, opacity);
            gm->m_performanceMode = perfMode;
        }
    };
}