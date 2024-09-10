#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {

    class NoPortalLightning final : public hack::Hack {
        void init() override {
            gui::MenuTab::find("Level")
                ->addToggle("No Portal Lightning", "level.noportallightning")
                ->setDescription("Disables lightning effects when passing through size change portals and more.")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Portal Lightning"; }
    };

    REGISTER_HACK(NoPortalLightning)

    class $modify(NoPortalLightningGJBGLHook, GJBaseGameLayer) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("GJBaseGameLayer::lightningFlash");
        }

        void lightningFlash(cocos2d::CCPoint from, cocos2d::CCPoint to, cocos2d::ccColor3B color, float lineWidth, float duration, int displacement, bool flash, float opacity) {
            if (!config::get<bool>("level.noportallightning"))
                return GJBaseGameLayer::lightningFlash(from, to, color, lineWidth, duration, displacement, flash, opacity);

            auto* gm = GameManager::get();
            auto perfMode = gm->m_performanceMode;
            gm->m_performanceMode = true;
            flash = false;
            GJBaseGameLayer::lightningFlash(from, to, color, lineWidth, duration, displacement, flash, opacity);
            gm->m_performanceMode = perfMode;
        }
    };
}