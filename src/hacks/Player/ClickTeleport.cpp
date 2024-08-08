#include <numbers>
#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/keybinds/manager.hpp>
#include <imgui.h>

#ifndef GEODE_IS_ANDROID

namespace eclipse::hacks::Player {

    double degToRad(double degrees) {
        return degrees * std::numbers::pi / 180;
    }

    cocos2d::CCPoint rotateVector(const cocos2d::CCPoint& vector, double angle) {
        auto x = vector.x * cos(angle) - vector.y * sin(angle);
        auto y = vector.x * sin(angle) + vector.y * cos(angle);
        return ccp(x, y);
    }

    cocos2d::CCPoint screenToGame(const cocos2d::CCPoint& screenPos, PlayLayer* playLayer) {
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

    cocos2d::CCPoint screenToFrame(const ImVec2& pos) {
        auto* director = cocos2d::CCDirector::sharedDirector();
        const auto frameSize = director->getOpenGLView()->getFrameSize();
        const auto winSize = director->getWinSize();

        return {
            pos.x / frameSize.width * winSize.width,
            (1.f - pos.y / frameSize.height) * winSize.height
        };
    }

    class ClickTeleport : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("Click Teleport", "player.clicktp")
                ->setDescription("Teleport to the mouse cursor's position when right clicking.")
                ->handleKeybinds();
        }

        void update() override {
            if (!config::get<bool>("player.clicktp", false)) return;

            // Force the cursor to be visible
            PlatformToolbox::showCursor();

            if (keybinds::isKeyPressed(keybinds::Keys::MouseRight)) {
                auto playLayer = PlayLayer::get();
                if (!playLayer) return;
                auto gamePos = screenToGame(geode::cocos::getMousePos(), playLayer);

                playLayer->m_player1->m_position = gamePos;
            }
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("player.clicktp", false); }
        [[nodiscard]] const char* getId() const override { return "Click Teleport"; }
    };

    REGISTER_HACK(ClickTeleport)

}

#endif