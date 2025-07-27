#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/Geode.hpp>
#include <Geode/modify/CCDrawNode.hpp>
#include <Geode/modify/HardStreak.hpp>

namespace eclipse::hacks::Player {
    class $hack(SolidWaveTrail) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.solidwavetrail")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Solid Wave Trail"; }
    };

    REGISTER_HACK(SolidWaveTrail)

    class $modify(SolidWaveTrailHSHook, HardStreak) {
        ENABLE_SAFE_HOOKS_ALL()

        struct Fields {
            std::optional<bool> m_prevIsSolid = std::nullopt;
        };

        void updateStroke(float dt) {
          // the first call to updateStroke has a dt of 0
          // the game sets the blending function after the first call to updateStroke
          // so delay the below code for that update so that gd doesn't override the blending function
          // if you enter a level while having solid wave trail on
          if (dt == 0) return HardStreak::updateStroke(dt);
          bool solidWaveTrailIsOn = config::get<bool>("player.solidwavetrail", false);
          bool shouldChangeBlend = false;
          if (solidWaveTrailIsOn && !m_fields->m_prevIsSolid) {
              // solid wave trail was just turned on
              m_fields->m_prevIsSolid = m_isSolid;
              m_isSolid = true;
              shouldChangeBlend = true;
          }  else if (!solidWaveTrailIsOn && m_fields->m_prevIsSolid) {
              // solid wave trail was just turned off
              m_isSolid = *m_fields->m_prevIsSolid;
              m_fields->m_prevIsSolid = std::nullopt;
              shouldChangeBlend = true;
          }
          HardStreak::updateStroke(dt);
          if (shouldChangeBlend) {
            if (m_isSolid) setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
            else setBlendFunc({GL_SRC_ALPHA, GL_ONE});
          }
      }
    };
}
