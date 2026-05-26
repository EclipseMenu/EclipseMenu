#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "../../hack.hpp"   

using namespace geode::prelude;

namespace eclipse::hacks::Level {

    class AutoDisableShake : public Hack {
    public:
        // ---- Hack metadata ----
        std::string getId() const override {
            return "level.auto-disable-shake";
        }

        // ---- UI registration (called once on menu build) ----
        void onInit(MenuTab& tab) override {
            tab.addToggle(getId(), "Auto Disable Shake", [](bool) {})
               .setDescription(
                   "Automatically disables screen shake when a level starts."
               );
        }
    };

   
    REGISTER_HACK(AutoDisableShake)


    class $modify(AutoDisableShakeHook, PlayLayer) {
        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            if (!PlayLayer::init(level, useReplay, dontCreateObjects))
                return false;

            if (config::get<bool>("level.auto-disable-shake", false)) {
              
                this->m_gameState.m_disableShake = true;
            }

            return true;
        }
    };

}
