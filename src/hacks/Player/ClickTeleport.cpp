#include <imgui.h>
#include <numbers>
#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/gui/components/keybind.hpp>
#include <modules/hack/hack.hpp>
#include <modules/keybinds/manager.hpp>

#ifndef GEODE_IS_MOBILE

namespace eclipse::hacks::Player {
    constexpr double degToRad(double degrees) {
        return degrees * std::numbers::pi / 180;
    }

    static cocos2d::CCPoint rotateVector(cocos2d::CCPoint const& vector, double angle) {
        auto x = vector.x * cos(angle) - vector.y * sin(angle);
        auto y = vector.x * sin(angle) + vector.y * cos(angle);
        return ccp(x, y);
    }

    static cocos2d::CCPoint screenToGame(cocos2d::CCPoint const & screenPos, PlayLayer* playLayer) {
        auto cameraPos = playLayer->m_gameState.m_cameraPosition;
        auto cameraScale = playLayer->m_gameState.m_cameraZoom;
        auto cameraAngle = playLayer->m_gameState.m_cameraAngle;

        // Rotate the position around the camera angle
        auto angle = degToRad(cameraAngle);
        auto rotatedPos = rotateVector(screenPos, angle);

        // Scale the position
        auto scaledPos = ccp(rotatedPos.x / cameraScale, rotatedPos.y / cameraScale);

        // Add the camera position
        auto point = ccp(cameraPos.x + scaledPos.x, cameraPos.y + scaledPos.y);

        return point;
    }

    static cocos2d::CCPoint screenToFrame(ImVec2 const& pos) {
        auto* director = utils::get<cocos2d::CCDirector>();
        auto frameSize = director->getOpenGLView()->getFrameSize();
        auto winSize = director->getWinSize();

        return {
            pos.x / frameSize.width * winSize.width,
            (1.f - pos.y / frameSize.height) * winSize.height
        };
    }

    class $hack(ClickTeleport) {
        void init() override {
            config::setIfEmpty<keybinds::KeybindProps>("player.clicktp.key", keybinds::Keys::MouseRight);

            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.clicktp")->setDescription()->handleKeybinds()
                ->addOptions([](auto options) {
                    options->addKeybind("player.clicktp.key", "player.clicktp.key")
                           ->setInternal()
                           ->setDefaultKey(keybinds::Keys::MouseRight);
                });

            keybinds::Manager::get()->addListener("player.clicktp.key", [](auto evt) {
                if (!evt.down) return;
                if (!config::get<"player.clicktp", bool>(false)) return;

                auto* playLayer = utils::get<PlayLayer>();
                if (!playLayer) return;

                playLayer->m_player1->m_position = screenToGame(geode::cocos::getMousePos(), playLayer);
            });
        }

        void update() override {
            if (!config::get<"player.clicktp", bool>(false)) return;

            // Force the cursor to be visible
            PlatformToolbox::showCursor();
        }

        [[nodiscard]] bool isCheating() const override { return config::get<"player.clicktp", bool>(); }
        [[nodiscard]] const char* getId() const override { return "Click Teleport"; }
    };

    REGISTER_HACK(ClickTeleport)
}

#endif
