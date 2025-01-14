#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/Geode.hpp>
#include <Geode/modify/CCDrawNode.hpp>

namespace eclipse::hacks::Player {
    class $hack(SolidWaveTrail) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.solidwavetrail")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Solid Wave Trail"; }
    };

    REGISTER_HACK(SolidWaveTrail)

    class $modify(SolidWaveTrailCCDNHook, cocos2d::CCDrawNode) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.solidwavetrail")

        bool drawPolygon(cocos2d::CCPoint* p0, unsigned int p1, const cocos2d::ccColor4F& p2, float p3, const cocos2d::ccColor4F& p4) {
            if (p2.r == 1.F && p2.g == 1.F && p2.b == 1.F && p2.a != 1.F) return true;

            this->setBlendFunc(cocos2d::CCSprite::create()->getBlendFunc());
            this->setZOrder(-1);
            return CCDrawNode::drawPolygon(p0, p1, p2, p3, p4);
        }
    };
}
