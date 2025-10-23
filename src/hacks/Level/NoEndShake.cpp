#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(NoEndShake) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.noendshake")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No End Shake"; }
    };

    REGISTER_HACK(NoEndShake)

    class $modify(NoEndShakePLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("level.noendshake")

        void showCompleteEffect() {
          auto unkUint64_1 = m_gameState.m_unkUint64_1;
          auto cameraShakeFactor = m_gameState.m_cameraShakeFactor;
          auto unkPoint34 = m_gameState.m_unkPoint34;
          auto unkUint16 = m_gameState.m_unkUint16;
          auto unkUint15 = m_gameState.m_unkUint15;
          PlayLayer::showCompleteEffect();
          m_gameState.m_unkUint64_1 = unkUint64_1;
          m_gameState.m_cameraShakeFactor = cameraShakeFactor;
          m_gameState.m_unkPoint34 = unkPoint34;
          m_gameState.m_unkUint16 = unkUint16;
          m_gameState.m_unkUint15 = unkUint15;
        }
    };
}
