#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/keybinds/manager.hpp>

#include <Geode/modify/MenuGameLayer.hpp>

namespace eclipse::hacks::Bypass {
    class $hack(MenuGameplay) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");
            tab->addToggle("bypass.menugameplay")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Menu Gameplay"; }
    };

    REGISTER_HACK(MenuGameplay)

    class $modify(MenuGameplayMGLHook, MenuGameLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("bypass.menugameplay")

        void tryJump(float) { /* Disable auto-play */ }

        void update(float dt) override {
            // Spider crashes the game
            if (m_playerObject && !m_playerObject->m_isSpider) {
                constexpr std::array keys = {
                    keybinds::Keys::Up,
                    keybinds::Keys::W,
                    keybinds::Keys::MouseLeft
                };
                for (auto key : keys) {
                    if (keybinds::isKeyPressed(key)) {
                        m_playerObject->pushButton(PlayerButton::Jump);
                    } else if (keybinds::isKeyReleased(key)) {
                        m_playerObject->releaseButton(PlayerButton::Jump);
                    }
                }
            }

            MenuGameLayer::update(dt);
        }
    };
}
