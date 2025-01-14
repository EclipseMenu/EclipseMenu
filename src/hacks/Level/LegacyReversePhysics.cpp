#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/LevelSettingsObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {
    class $modify(LegacyPhysicsPlayLayer, PlayLayer) {
        struct Fields {
            bool originalFixGravityVal = false;
        };

        void toggleFixGravityBugState(bool newState) {
            m_levelSettings->m_fixGravityBug = newState ? false : m_fields->originalFixGravityVal;
        }

        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
            m_fields->originalFixGravityVal = m_levelSettings->m_fixGravityBug;
            if (config::get<bool>("level.legacyreversephysics", false))
                m_levelSettings->m_fixGravityBug = false;
            return true;
        }
    };

    class $hack(LegacyReversePhysics) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            tab->addToggle("level.legacyreversephysics")->handleKeybinds()->setDescription()
               ->callback([](bool newState) {
                   if (auto pl = utils::get<PlayLayer>()) {
                       static_cast<LegacyPhysicsPlayLayer*>(pl)->toggleFixGravityBugState(newState);
                   }
               });
        }

        //player would be cheating only if level is new physics and legacy physics is active
        //otherwise the hack being on doesn't matter
        [[nodiscard]] bool isCheating() const override {
            CACHE_CONFIG_BOOL(legacyPhysics, "level.legacyreversephysics");
            if (!legacyPhysics) return false;
            auto pl = utils::get<PlayLayer>();
            if (!pl) return false;

            return static_cast<LegacyPhysicsPlayLayer*>(pl)->m_fields->originalFixGravityVal;
        }

        [[nodiscard]] const char* getId() const override { return "Legacy Reverse Physics"; }
    };

    REGISTER_HACK(LegacyReversePhysics)
}
