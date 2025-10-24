#include <cmath>
#include <modules/config/config.hpp>
#include <modules/gui/color.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/CCDrawNode.hpp>

using namespace geode::prelude;

namespace eclipse::hacks::Player {

    static cocos2d::CCDrawNode* s_currentStreak = nullptr;
    static cocos2d::CCDrawNode* s_currentStreak2 = nullptr;

    class $hack(CustomWaveTrail) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");

            config::setIfEmpty("player.customwavetrail.scale", 2.f);
            config::setIfEmpty("player.customwavetrail.speed", 0.5f);
            config::setIfEmpty("player.customwavetrail.saturation", 100.f);
            config::setIfEmpty("player.customwavetrail.value", 100.f);
            config::setIfEmpty("player.customwavetrail.color", gui::Color::WHITE);
            config::setIfEmpty("player.customwavetrail.outline.color", gui::Color::BLACK);
            config::setIfEmpty("player.customwavetrail.outline.stroke", 2.f);
            config::setIfEmpty("player.customwavetrail.outline.blur", 0);

            tab->addToggle("player.customwavetrail")->handleKeybinds()->setDescription()
               ->addOptions([](auto options) {
                   options->addInputFloat("player.customwavetrail.scale", 0.f, 10.f, "%.2f");
                   options->addToggle("player.customwavetrail.rainbow")->addOptions([](auto opt) {
                       opt->addInputFloat("player.customwavetrail.speed", 0.f, FLT_MAX, "%.2f");
                       opt->addInputFloat("player.customwavetrail.saturation", 0.f, 100.f, "%.2f");
                       opt->addInputFloat("player.customwavetrail.value", 0.f, 100.f, "%.2f");
                   });
                   options->addToggle("player.customwavetrail.customcolor")->addOptions([](auto opt) {
                       opt->addColorComponent("player.customwavetrail.color");
                   });
                   options->addToggle("player.customwavetrail.outline")->addOptions([](auto opt) {
                       opt->addInputFloat("player.customwavetrail.outline.stroke", 0.f, 10.f, "%.2f");
                       opt->addInputInt("player.customwavetrail.outline.blur", 0, 32);
                       opt->addColorComponent("player.customwavetrail.outline.color");
                   });
               });
        }

        [[nodiscard]] const char* getId() const override { return "Custom Wave Trail"; }
    };

    REGISTER_HACK(CustomWaveTrail)

    class $modify(WaveTrailSizePLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("player.customwavetrail.outline")
        void resetLevel() {
            s_currentStreak = nullptr;
            s_currentStreak2 = nullptr;
            PlayLayer::resetLevel();
        }
    };

    class $modify(WaveTrailSizeHSHook, HardStreak) {
        ADD_HOOKS_DELEGATE("player.customwavetrail")
        void updateStroke(float dt) {
            if (config::get<"player.customwavetrail.rainbow", bool>(false)) {
                auto speed = config::get<"player.customwavetrail.speed", float>(0.5f);
                auto saturation = config::get<"player.customwavetrail.saturation", float>(100.f);
                auto value = config::get<"player.customwavetrail.value", float>(100.f);
                this->setColor(utils::getRainbowColor(speed / 10.f, saturation / 100.f, value / 100.f).toCCColor3B());
            } else if (config::get<"player.customwavetrail.customcolor", bool>(false)) {
                auto color = config::get<"player.customwavetrail.color", gui::Color>(gui::Color::WHITE);
                this->setColor(color.toCCColor3B());
            }

            this->m_pulseSize = config::get<"player.customwavetrail.scale", float>(2.f);

            if (s_currentStreak == nullptr || s_currentStreak2 == nullptr) {
                if (auto PL = utils::get<PlayLayer>()) {
                    if (PL->m_player1 && s_currentStreak == nullptr) {
                        if (this == PL->m_player1->m_waveTrail) {
                            s_currentStreak = this;
                            PL->m_player1->m_waveTrail->updateStroke(dt);
                        }
                    }
                    if (PL->m_player2 && s_currentStreak2 == nullptr) {
                        if (this == PL->m_player2->m_waveTrail) {
                            s_currentStreak2 = this;
                            PL->m_player2->m_waveTrail->updateStroke(dt);
                        }
                    }
                }
            }

            HardStreak::updateStroke(dt);
        }
    };

    class $modify(WaveTrailStrokeHSHook, cocos2d::CCDrawNode) {
        ADD_HOOKS_DELEGATE("player.customwavetrail.outline")
        bool drawPolygon(cocos2d::CCPoint *verts, unsigned int count, const cocos2d::ccColor4F &fillColor, float borderWidth, const cocos2d::ccColor4F &borderColor) {
            if ((fillColor.r == 1.F && fillColor.g == 1.F && fillColor.b == 1.F && fillColor.a != 1.F) || ((s_currentStreak != this) && (s_currentStreak2 != this)))
                return CCDrawNode::drawPolygon(verts, count, fillColor, borderWidth, borderColor);

            auto color = config::get<"player.customwavetrail.outline.color", gui::Color>(gui::Color::BLACK);
            auto width = config::get<"player.customwavetrail.outline.stroke", float>(2.F);

            this->setBlendFunc(cocos2d::CCSprite::create()->getBlendFunc());
            this->setZOrder(-1);

            const int glowLayers = config::get<"player.customwavetrail.outline.blur", int>(0);
            if (glowLayers == 0) {
                std::vector<CCPoint> newVerts(count);
                for (unsigned int i = 0; i < count; i++) {
                    newVerts[i] = verts[i];
                }
                float offset = width + (width / count);
                newVerts[0].y -= offset;
                newVerts[3].y -= offset;
                newVerts[1].y += offset;
                newVerts[2].y += offset;
                this->drawSegment(newVerts[0], newVerts[3], width, color);
                this->drawSegment(newVerts[1], newVerts[2], width, color);
                return CCDrawNode::drawPolygon(verts, count, fillColor, borderWidth, borderColor);
            }
            //width = width / glowLayers;
            for (int i = 0; i < glowLayers; i++) {
                float layerWidth = width * (1.0f + (i * 0.8f));
                float opacity = std::max({0.05f, 0.8f * (float)std::pow(0.7f, i)});
                auto glowColor = color;
                glowColor.a *= opacity;

                std::vector<CCPoint> newVerts(count);
                for (unsigned int j = 0; j < count; j++) {
                    newVerts[j] = verts[j];
                }

                float offset = layerWidth + (layerWidth / count);
                newVerts[0].y -= offset;
                newVerts[3].y -= offset;
                newVerts[1].y += offset;
                newVerts[2].y += offset;

                this->drawSegment(newVerts[0], newVerts[3], layerWidth, glowColor);
                this->drawSegment(newVerts[1], newVerts[2], layerWidth, glowColor);
            }
            return CCDrawNode::drawPolygon(verts, count, fillColor, borderWidth, borderColor);
        }
    };
}
