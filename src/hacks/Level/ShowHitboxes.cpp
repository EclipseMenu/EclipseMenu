#include <ranges>
#include <modules/config/config.hpp>
#include <modules/gui/color.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/CCDrawNode.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

namespace eclipse::hacks::Level {
    static bool s_isDead = false;
    static GameObject* s_collisionObject = nullptr;
    static std::deque<std::pair<cocos2d::CCRect, cocos2d::CCRect>> s_playerTrail1, s_playerTrail2;

    static void forEachObject(GJBaseGameLayer const* game, auto&& callback) {
        int count = game->m_sections.empty() ? -1 : game->m_sections.size();
        for (int i = game->m_leftSectionIndex; i <= game->m_rightSectionIndex && i < count; ++i) {
            auto leftSection = game->m_sections[i];
            if (!leftSection) continue;

            auto leftSectionSize = leftSection->size();
            for (int j = game->m_bottomSectionIndex; j <= game->m_topSectionIndex && j < leftSectionSize; ++j) {
                auto section = leftSection->at(j);
                if (!section) continue;

                auto sectionSize = game->m_sectionSizes[i]->at(j);
                for (int k = 0; k < sectionSize; ++k) {
                    auto obj = section->at(k);
                    if (!obj) continue;

                    callback(obj);
                }
            }
        }
    }

    inline void drawRect(
        cocos2d::CCDrawNode* node, cocos2d::CCRect const& rect, cocos2d::ccColor4F const& color,
        float borderWidth, cocos2d::ccColor4F const& borderColor
    ) {
        std::array vertices = {
            cocos2d::CCPoint{rect.getMinX(), rect.getMinY()},
            cocos2d::CCPoint{rect.getMinX(), rect.getMaxY()},
            cocos2d::CCPoint{rect.getMaxX(), rect.getMaxY()},
            cocos2d::CCPoint{rect.getMaxX(), rect.getMinY()}
        };
        node->drawPolygon(vertices.data(), vertices.size(), color, borderWidth, borderColor);
    }

    inline float getObjectRadius(GameObject const* obj) {
        return std::max(obj->m_scaleX, obj->m_scaleY) * obj->m_objectRadius;
    }

    class $modify(ShowHitboxesGJBGLHook, GJBaseGameLayer) {
        struct Fields {
            cocos2d::CCDrawNode* m_drawNode;
        };

        static ShowHitboxesGJBGLHook* get() {
            return static_cast<ShowHitboxesGJBGLHook*>(utils::get<GJBaseGameLayer>());
        }

        void createDrawNode() {
            auto* parent = m_debugDrawNode->getParent();
            if (!parent) return;

            s_isDead = false;
            s_collisionObject = nullptr;

            auto* drawNode = cocos2d::CCDrawNode::create();
            drawNode->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
            drawNode->setID("hitboxes"_spr);
            drawNode->m_bUseArea = false;
            parent->addChild(drawNode, 1402);

            m_fields->m_drawNode = drawNode;
        }

        cocos2d::CCDrawNode* getDrawNode() {
            return m_fields->m_drawNode;
        }

        static bool shouldShowHitboxes() {
            return config::get<"level.showhitboxes", bool>()
                || (s_isDead && config::get<"level.showhitboxes.ondeath", bool>());
        }

        void visitHitboxes() {
            auto* drawNode = m_fields->m_drawNode;
            if (!drawNode) return;

            drawNode->clear();

            if (!shouldShowHitboxes() || (m_isEditor && !config::get<"level.showhitboxes.editor", bool>(true)))
                return;

            auto solidColor = config::get<"level.showhitboxes.solid_color", gui::Color>(gui::Color(0, 0.247, 1));
            auto dangerColor = config::get<"level.showhitboxes.danger_color", gui::Color>(gui::Color(1, 0, 0));
            auto passableColor = config::get<"level.showhitboxes.passable_color", gui::Color>(gui::Color(0, 1, 1));
            auto otherColor = config::get<"level.showhitboxes.other_color", gui::Color>(gui::Color(0, 1, 0));
            auto triggersColor = config::get<"level.showhitboxes.triggers_color", gui::Color>(gui::Color(1, 0, 0.9f));
            auto playerColor = config::get<"level.showhitboxes.player_color", gui::Color>(gui::Color(1, 1, 0));
            auto playerColorInner = config::get<"level.showhitboxes.player_color_inner", gui::Color>(gui::Color(0, 1, 0.2f));
            auto playerColorRotated = config::get<"level.showhitboxes.player_color_rotated", gui::Color>(gui::Color(1, 1, 0));

            auto fillAlpha = config::get<"level.showhitboxes.fillalpha", float>(0.25f);
            auto fillAlphaToggle = config::get<"level.showhitboxes.fillalpha.toggle", bool>(true);
            auto borderSize = config::get<"level.showhitboxes.bordersize", float>(0.25f);
            auto renderTriggers = config::get<"level.showhitboxes.showtriggers", bool>(false);

            auto solidColorFill = gui::Color(solidColor, fillAlphaToggle ? fillAlpha : 0.f);
            auto dangerColorFill = gui::Color(dangerColor, fillAlphaToggle ? fillAlpha : 0.f);
            auto passableColorFill = gui::Color(passableColor, fillAlphaToggle ? fillAlpha : 0.f);
            auto otherColorFill = gui::Color(otherColor, fillAlphaToggle ? fillAlpha : 0.f);
            auto triggersColorFill = gui::Color(triggersColor, fillAlphaToggle ? fillAlpha : 0.f);
            auto playerColorFill = gui::Color(playerColor, fillAlphaToggle ? fillAlpha : 0.f);
            auto playerColorInnerFill = gui::Color(playerColorInner, fillAlphaToggle ? fillAlpha : 0.f);
            auto playerColorRotatedFill = gui::Color(playerColorRotated, fillAlphaToggle ? fillAlpha : 0.f);

            const auto visitObject = [&](GameObject* obj) {
                // skip objects that don't have a hitbox or are not activated
                if (obj->m_objectType == GameObjectType::Decoration || !obj->m_isActivated || obj->m_isGroupDisabled)
                    return;

                switch (obj->m_objectType) {
                    default: { // pretty much everything (portals, orbs, etc.)
                        constexpr auto drawFunc = [](
                            cocos2d::CCDrawNode* node, GameObject* obj,
                            cocos2d::ccColor4F const& colorFill, float borderWidth,
                            cocos2d::ccColor4F const& color
                        ) {
                            if (auto orientedBox = obj->m_orientedBox) {
                                node->drawPolygon(
                                    orientedBox->m_corners.data(), 4,
                                    colorFill, borderWidth, color
                                );
                            } else {
                                auto isObjectRectDirty = obj->m_isObjectRectDirty;
                                auto boxOffsetCalculated = obj->m_boxOffsetCalculated;
                                drawRect(node, obj->getObjectRect(), colorFill, borderWidth, color);
                                obj->m_isObjectRectDirty = isObjectRectDirty;
                                obj->m_boxOffsetCalculated = boxOffsetCalculated;
                            }
                        };

                        if (obj->m_objectType == GameObjectType::Modifier) {
                            if (!renderTriggers || !static_cast<EffectGameObject*>(obj)->m_isTouchTriggered) return;
                            return drawFunc(
                                drawNode, obj,
                                triggersColorFill, borderSize,
                                triggersColor
                            );
                        }

                        if (obj == m_player1 || obj == m_player2) {
                            return;
                        }

                        drawFunc(
                            drawNode, obj,
                            otherColorFill, borderSize,
                            otherColor
                        );
                        break;
                    }
                    case GameObjectType::Solid: {
                        auto& objColor = obj->m_isPassable ? passableColor : solidColor;
                        auto& objColorFill = obj->m_isPassable ? passableColorFill : solidColorFill;
                        drawRect(
                            drawNode, obj->getObjectRect(),
                            objColorFill, borderSize,
                            objColor
                        );
                        break;
                    }
                    case GameObjectType::Slope: {
                        auto rect = obj->getObjectRect();
                        std::array vertices = {
                            cocos2d::CCPoint{rect.getMinX(), rect.getMinY()}, // Bottom left
                            cocos2d::CCPoint{rect.getMinX(), rect.getMaxY()}, // Top left
                            cocos2d::CCPoint{rect.getMaxX(), rect.getMinY()}, // Bottom right
                        };

                        cocos2d::CCPoint topRight{rect.getMaxX(), rect.getMaxY()};
                        switch (obj->m_slopeDirection) {
                            case 0: case 7:
                                vertices[1] = topRight;
                                break;
                            case 1: case 5:
                                vertices[0] = topRight;
                                break;
                            case 3: case 6:
                                vertices[2] = topRight;
                                break;
                            default: break;
                        }

                        drawNode->drawPolygon(
                            vertices.data(), vertices.size(),
                            obj->m_isPassable ? passableColorFill : solidColorFill,
                            borderSize, obj->m_isPassable ? passableColor : solidColor
                        );
                        break;
                    }
                    case GameObjectType::AnimatedHazard:
                    case GameObjectType::Hazard: {
                        if (!obj->m_isActivated || obj == m_anticheatSpike) break;
                        if (auto radius = getObjectRadius(obj); radius > 0) {
                            drawNode->drawCircle(
                                obj->getPosition(), radius,
                                dangerColorFill, borderSize,
                                dangerColor, 16
                            );
                        } else if (auto orientedBox = obj->m_orientedBox) {
                            drawNode->drawPolygon(
                                orientedBox->m_corners.data(), 4,
                                dangerColorFill, borderSize, dangerColor
                            );
                        } else {
                            auto isObjectRectDirty = obj->m_isObjectRectDirty;
                            auto boxOffsetCalculated = obj->m_boxOffsetCalculated;
                            drawRect(drawNode, obj->getObjectRect(), dangerColorFill, borderSize, dangerColor);
                            obj->m_isObjectRectDirty = isObjectRectDirty;
                            obj->m_boxOffsetCalculated = boxOffsetCalculated;
                        }
                        break;
                    }
                    case GameObjectType::CollisionObject: break;
                }
            };

            if (
                config::get<"level.showhitboxes.ondeath", bool>(false) && s_isDead &&
                config::get<"level.showhitboxes.ondeath.player", bool>(false) && s_collisionObject
            ) {
                visitObject(s_collisionObject);
            } else {
                forEachObject(this, visitObject);
            }

            // draw player trails
            if (config::get<bool>("level.showhitboxes.traillength.toggle", false)) {
                for (auto const& rect : s_playerTrail1 | std::views::keys) drawRect(
                    drawNode, rect,
                    playerColorFill, borderSize,
                    playerColor
                );

                for (auto const& rect : s_playerTrail2 | std::views::keys) drawRect(
                    drawNode, rect,
                    playerColorFill, borderSize,
                    playerColor
                );

                for (auto const& rect : s_playerTrail1 | std::views::values) drawRect(
                    drawNode, rect,
                    playerColorInnerFill, borderSize,
                    playerColorInner
                );

                for (auto const& rect : s_playerTrail2 | std::views::values) drawRect(
                    drawNode, rect,
                    playerColorInnerFill, borderSize,
                    playerColorInner
                );
            }

            // draw player hitboxes
            const auto drawPlayer = [&](PlayerObject* player) {
                if (!player) return;

                auto rect1 = player->getObjectRect();
                auto rect2 = player->getObjectRect(0.3f, 0.3f);

                if (auto ob = player->m_orientedBox) {
                    drawNode->drawPolygon(ob->m_corners.data(), 4, playerColorRotatedFill, borderSize, playerColorRotated);
                }

                drawRect(drawNode, rect1, playerColorFill, borderSize, playerColor);
                drawRect(drawNode, rect2, playerColorInnerFill, borderSize, playerColorInner);
            };

            drawPlayer(m_player1);
            if (m_gameState.m_isDualMode) {
                drawPlayer(m_player2);
            }
        }

        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            if (s_isDead || !config::get<"level.showhitboxes.traillength.toggle", bool>(false))
                return;

            s_playerTrail1.emplace_back(
                m_player1->getObjectRect(),
                m_player1->getObjectRect(0.3f, 0.3f)
            );

            if (m_gameState.m_isDualMode) {
                s_playerTrail2.emplace_back(
                    m_player2->getObjectRect(),
                    m_player2->getObjectRect(0.3f, 0.3f)
                );
            }

            auto max = static_cast<int>(config::get<"level.showhitboxes.traillength", float>(240.f));

            while (s_playerTrail1.size() > max)
                s_playerTrail1.pop_front();

            while (s_playerTrail2.size() > max)
                s_playerTrail2.pop_front();
        }
    };

    #define $hitbox static_cast<ShowHitboxesGJBGLHook*>(static_cast<GJBaseGameLayer*>(this))

    class $modify(ShowHitboxesPLHook, PlayLayer) {
        void createObjectsFromSetupFinished() {
            PlayLayer::createObjectsFromSetupFinished();
            $hitbox->createDrawNode();
        }

        void updateVisibility(float dt) override {
            PlayLayer::updateVisibility(dt);
            s_isDead = m_player1->m_isDead;
            $hitbox->visitHitboxes();
        }

        void resetLevel() {
            PlayLayer::resetLevel();

            s_isDead = false;
            s_collisionObject = nullptr;

            s_playerTrail1.clear();
            s_playerTrail2.clear();
        }

        void destroyPlayer(PlayerObject* player, GameObject* object) override {
            PlayLayer::destroyPlayer(player, object);
            if (m_anticheatSpike == object) return;
            if (object != nullptr)
                s_collisionObject = object;
        }
    };

    class $modify(ShowHitboxesPOHook, PlayerObject) {
        ENABLE_FIRST_HOOKS_ALL()

        void playerDestroyed(bool p0) {
            if (auto* pl = utils::get<PlayLayer>())
                s_isDead = this == pl->m_player1 || this == pl->m_player2;
            PlayerObject::playerDestroyed(p0);
        }

        void collidedWithObject(float p0, GameObject* p1, cocos2d::CCRect p2, bool p3) {
            PlayerObject::collidedWithObject(p0, p1, p2, p3);
            if (auto* pl = utils::get<PlayLayer>())
                if (this == pl->m_player1 || this == pl->m_player2)
                    s_collisionObject = p1;
        }
    };

    class $modify(ShowHitboxesLELHook, LevelEditorLayer) {
        bool init(GJGameLevel* level, bool unk) {
            if (!LevelEditorLayer::init(level, unk))
                return false;

            $hitbox->createDrawNode();
            return true;
        }

        void updateVisibility(float dt) override {
            LevelEditorLayer::updateVisibility(dt);
            $hitbox->visitHitboxes();
        }
    };

    class $modify(ShowHitboxesEditorHook, EditorUI) {
        void onPlaytest(CCObject* sender) {
            EditorUI::onPlaytest(sender);

            s_isDead = false;
            s_collisionObject = nullptr;

            s_playerTrail1.clear();
            s_playerTrail2.clear();
        }
    };

    class $hack(ShowHitboxes) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            auto toggle = tab->addToggle("level.showhitboxes")->handleKeybinds()->setDescription();

            config::setIfEmpty("level.showhitboxes.editor", true);
            config::setIfEmpty("level.showhitboxes.bordersize", 0.25f);
            config::setIfEmpty("level.showhitboxes.fillalpha", 0.25f);
            config::setIfEmpty("level.showhitboxes.traillength", 240.0f);
            config::setIfEmpty("level.showhitboxes.fillalpha.toggle", true);
            config::setIfEmpty("level.showhitboxes.solid_color", gui::Color(0, 0.247, 1));
            config::setIfEmpty("level.showhitboxes.danger_color", gui::Color::RED);
            config::setIfEmpty("level.showhitboxes.player_color", gui::Color::RED);
            config::setIfEmpty("level.showhitboxes.player_color_inner", gui::Color(0, 1, 0.2f));
            config::setIfEmpty("level.showhitboxes.player_color_rotated", gui::Color::YELLOW);
            config::setIfEmpty("level.showhitboxes.other_color", gui::Color::GREEN);
            config::setIfEmpty("level.showhitboxes.passable_color", gui::Color(0, 1, 1));
            config::setIfEmpty("level.showhitboxes.triggers_color", gui::Color(1, 0, 0.9f));

            toggle->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                options->addToggle("level.showhitboxes.editor");
                options->addToggle("level.showhitboxes.hideplayer");
                options->addToggle("level.showhitboxes.showtriggers");
                options->addToggle("level.showhitboxes.customcolors")->addOptions([](std::shared_ptr<gui::MenuTab> optionsColor) {
                    optionsColor->addColorComponent("level.showhitboxes.solid_color");
                    optionsColor->addColorComponent("level.showhitboxes.danger_color");
                    optionsColor->addColorComponent("level.showhitboxes.other_color");
                    optionsColor->addColorComponent("level.showhitboxes.passable_color");
                    optionsColor->addColorComponent("level.showhitboxes.triggers_color");
                    optionsColor->addColorComponent("level.showhitboxes.player_color");
                    optionsColor->addColorComponent("level.showhitboxes.player_color_inner");
                    optionsColor->addColorComponent("level.showhitboxes.player_color_rotated");
                });
                options->addInputFloat("level.showhitboxes.bordersize", 0.01f, 10.f, "%.2f");
                options->addFloatToggle("level.showhitboxes.fillalpha", 0.f, 1.f);
                options->addFloatToggle("level.showhitboxes.traillength", 1.f, 2000.f, "%.0f");
            });

            tab->addToggle("level.showhitboxes.ondeath")->handleKeybinds()->addOptions([](std::shared_ptr<gui::MenuTab> optionsOnDeath) {
                optionsOnDeath->addToggle("level.showhitboxes.ondeath.player");
            });

            config::addDelegate("level.showhitboxes", []() {
                if (auto hitbox = ShowHitboxesGJBGLHook::get()) {
                    hitbox->visitHitboxes();
                }
            });
        }

        [[nodiscard]] bool isCheating() const override {
            if (config::get<"level.showhitboxes", bool>()) {
                if (auto* pl = utils::get<PlayLayer>()) {
                    // if not in practice with enabled hitboxes
                    return !pl->m_isPracticeMode;
                }
            }

            return false;
        }

        [[nodiscard]] const char* getId() const override { return "Show Hitboxes"; }
    };

    REGISTER_HACK(ShowHitboxes)
}
