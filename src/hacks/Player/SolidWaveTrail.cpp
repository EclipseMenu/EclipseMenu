#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/Geode.hpp>
#include <Geode/modify/CCDrawNode.hpp>

using namespace geode::prelude;

namespace eclipse::hacks::Player {

    class SolidWaveTrail : public hack::Hack {    // Hack name and desciption
        void init() override {
            auto tab = gui::MenuTab::find("Player");
            tab->addToggle("Solid Wave Trail", "player.solidwavetrail")
                ->setDescription("Makes the player wave trail a solid color")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Solid Wave Trail"; }
    };

   REGISTER_HACK(SolidWaveTrail)

   class $modify(SolidWaveTrailCCDNHook, cocos2d::CCDrawNode)  {    // The actual hack code, and yes it is taken from Prism Menu
       bool drawPolygon(cocos2d::CCPoint *p0, unsigned int p1, const cocos2d::ccColor4F &p2, float p3, const cocos2d::ccColor4F &p4) {
           if (!config::get<bool>("player.solidwavetrail", false))
             return CCDrawNode::drawPolygon(p0,p1,p2,p3,p4);
           if (p2.r == 1.F && p2.g == 1.F && p2.b == 1.F && p2.a != 1.F) 
             return true; 
           this->setBlendFunc(CCSprite::create()->getBlendFunc());
           this->setZOrder(-1); 
           return CCDrawNode::drawPolygon(p0,p1,p2,p3,p4);
       }
   };

}
