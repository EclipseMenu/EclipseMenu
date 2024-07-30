#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/keybinds/manager.hpp>

#include <Geode/modify/MenuGameLayer.hpp>

namespace eclipse::hacks::Bypass {

    class MenuGameplay : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Menu Gameplay", "bypass.menugameplay")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Menu Gameplay"; }
    };

    REGISTER_HACK(MenuGameplay)

    class $modify(MenuGameplayMGLHook, MenuGameLayer) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("MenuGameLayer::tryJump");
            SAFE_PRIORITY("MenuGameLayer::update");
        }

        void tryJump(float dt) {
            // Disable auto-play
            if (config::get<bool>("bypass.menugameplay", false)) return;
            MenuGameLayer::tryJump(dt);
        }

        void update(float dt) {
            if (config::get<bool>("bypass.menugameplay", false)) {
                // Spider crashes the game
                if (m_playerObject && !m_playerObject->m_isSpider) {
                    const std::array<keybinds::Keys, 3> keys = {
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
            }

            MenuGameLayer::update(dt);
        }
    };

}
