#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelSettingsObject.hpp>

namespace eclipse::hacks::Level {

    class $modify(LegacyPhysicsPlayLayer, PlayLayer) {
        struct Fields {
            bool originalFixGravityVal = false;
        };

        void toggleFixGravityBugState(bool newState) {
            m_levelSettings->m_fixGravityBug = newState ? false : m_fields->originalFixGravityVal; 
        }

        bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
            if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
            m_fields->originalFixGravityVal = m_levelSettings->m_fixGravityBug;
            if (config::get<bool>("level.legacyreversephysics", false))
                m_levelSettings->m_fixGravityBug = false;
            return true;
        }
    };
    
    class LegacyReversePhysics : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Legacy Reverse Physics", "level.legacyreversephysics")
                ->handleKeybinds()
                ->setDescription("Forces old upside down physics among other old physics changes.")
                ->callback([](bool newState){
                    if (auto pl = PlayLayer::get()) {
                        static_cast<LegacyPhysicsPlayLayer*>(pl)->toggleFixGravityBugState(newState);
                    }
                });
        }

        //player would be cheating only if level is new physics and legacy physics is active
        //otherwise the hack being on doesn't matter
        [[nodiscard]] bool isCheating() override {
            if (!config::get<bool>("level.legacyreversephysics", false)) return false;
            auto pl = PlayLayer::get();
            if (!pl) return false;

            return static_cast<LegacyPhysicsPlayLayer*>(pl)->m_fields->originalFixGravityVal;
        }

        [[nodiscard]] const char* getId() const override { return "Legacy Reverse Physics"; }
    };

    REGISTER_HACK(LegacyReversePhysics)
}