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
    // /// @brief Check whether hitboxes made by RobTop should be drawn. (e.g. in practice mode)
    // inline bool robtopHitboxCheck() {
    //     if (auto* pl = utils::get<PlayLayer>())
    //         return pl->m_isPracticeMode && utils::get<GameManager>()->getGameVariable("0166");
    //     if (utils::get<LevelEditorLayer>())
    //         return utils::get<GameManager>()->getGameVariable("0045");
    //     return false;
    // }
    //
    // /// @brief Gracefully disable hitboxes to return to the original state
    // inline void toggleOffHitboxes() {
    //     if (config::get<bool>("level.showhitboxes", false)) return;
    //     if (auto* gjbgl = utils::get<GJBaseGameLayer>())
    //         gjbgl->m_debugDrawNode->setVisible(robtopHitboxCheck());
    // }
    //
    // static bool s_isDead = false;
    // static bool s_skipDrawHook = false;
    // static bool s_slopeHitboxFix = false;
    // static GameObject* s_collisionObject = nullptr;
    //
    // static std::deque<std::pair<cocos2d::CCRect, cocos2d::CCRect>> s_playerTrail1, s_playerTrail2;
    //
    // class $hack(ShowHitboxes) {
    //     void init() override {
    //         auto tab = gui::MenuTab::find("tab.level");
    //
    //         auto toggle = tab->addToggle("level.showhitboxes")->handleKeybinds()->setDescription();
    //
    //         toggle->callback([](bool value) {
    //             if (auto pl = utils::get<PlayLayer>()) {
    //                 // since progress bar isn't added immediately, we need to check if it exists
    //                 if (pl->m_progressBar == nullptr) return;
    //                 pl->updateProgressbar();
    //             }
    //
    //             if (utils::get<LevelEditorLayer>()) {
    //                 utils::get<LevelEditorLayer>()->updateEditor(0);
    //                 utils::get<LevelEditorLayer>()->updateOptions();
    //             }
    //             toggleOffHitboxes();
    //         });
    //
    //         config::setIfEmpty("level.showhitboxes.editor", true);
    //         config::setIfEmpty("level.showhitboxes.bordersize", 0.25f);
    //         config::setIfEmpty("level.showhitboxes.fillalpha", 0.25f);
    //         config::setIfEmpty("level.showhitboxes.traillength", 240.0f);
    //         config::setIfEmpty("level.showhitboxes.fillalpha.toggle", true);
    //         config::setIfEmpty("level.showhitboxes.solid_color", gui::Color(0, 0.247, 1));
    //         config::setIfEmpty("level.showhitboxes.danger_color", gui::Color::RED);
    //         config::setIfEmpty("level.showhitboxes.player_color", gui::Color::RED);
    //         config::setIfEmpty("level.showhitboxes.player_color_inner", gui::Color(0, 1, 0.2f));
    //         config::setIfEmpty("level.showhitboxes.player_color_rotated", gui::Color::YELLOW);
    //         config::setIfEmpty("level.showhitboxes.other_color", gui::Color::GREEN);
    //
    //         toggle->addOptions([](std::shared_ptr<gui::MenuTab> options) {
    //             options->addToggle("level.showhitboxes.editor");
    //             options->addToggle("level.showhitboxes.hideplayer");
    //             options->addToggle("level.showhitboxes.ondeath")->handleKeybinds()->addOptions([](std::shared_ptr<gui::MenuTab> optionsOnDeath) {
    //                 optionsOnDeath->addToggle("level.showhitboxes.ondeath.player");
    //             });
    //             options->addToggle("level.showhitboxes.customcolors")->addOptions([](std::shared_ptr<gui::MenuTab> optionsColor) {
    //                 optionsColor->addColorComponent("level.showhitboxes.solid_color");
    //                 optionsColor->addColorComponent("level.showhitboxes.danger_color");
    //                 optionsColor->addColorComponent("level.showhitboxes.other_color");
    //                 optionsColor->addColorComponent("level.showhitboxes.player_color");
    //                 optionsColor->addColorComponent("level.showhitboxes.player_color_inner");
    //                 optionsColor->addColorComponent("level.showhitboxes.player_color_rotated");
    //             });
    //             options->addInputFloat("level.showhitboxes.bordersize", 0.01f, 10.f, "%.2f");
    //             options->addFloatToggle("level.showhitboxes.fillalpha", 0.f, 1.f);
    //             options->addFloatToggle("level.showhitboxes.traillength", 1.f, 2000.f, "%.0f");
    //         });
    //     }
    //
    //     [[nodiscard]] bool isCheating() const override {
    //         auto enabled = config::get<"level.showhitboxes", bool>();
    //         auto onDeath = config::get<"level.showhitboxes.ondeath", bool>();
    //         if (onDeath)
    //             return false; // on-death hitboxes are fine
    //
    //         if (auto* pl = utils::get<PlayLayer>())
    //             // if not in practice with enabled hitboxes
    //             return enabled && !pl->m_isPracticeMode;
    //
    //         return false;
    //     }
    //
    //     [[nodiscard]] const char* getId() const override { return "Show Hitboxes"; }
    // };
    //
    // REGISTER_HACK(ShowHitboxes)
    //
    // enum HitboxType {
    //     Solid,
    //     Danger,
    //     Player,
    //     Other
    // };
    //
    // inline bool shouldDrawHitboxes() {
    //     return config::get<"level.showhitboxes", bool>()
    //         || (s_isDead && config::get<"level.showhitboxes.ondeath", bool>());
    // }
    //
    // inline HitboxType getHitboxType(const gui::Color& color) {
    //     if (color.r == 0.0f)
    //         return color.b == 1.0f ? HitboxType::Solid : HitboxType::Other;
    //     if (color.g == 1.0f)
    //         return HitboxType::Player;
    //     return HitboxType::Danger;
    // }
    //
    // inline void drawRect(
    //     cocos2d::CCDrawNode* node, const cocos2d::CCRect& rect, const gui::Color& color,
    //     float borderWidth, const gui::Color& borderColor
    // ) {
    //     std::array vertices = {
    //         cocos2d::CCPoint(rect.getMinX(), rect.getMinY()),
    //         cocos2d::CCPoint(rect.getMinX(), rect.getMaxY()),
    //         cocos2d::CCPoint(rect.getMaxX(), rect.getMaxY()),
    //         cocos2d::CCPoint(rect.getMaxX(), rect.getMinY())
    //     };
    //     s_skipDrawHook = true;
    //
    //     node->drawPolygon(vertices.data(), vertices.size(), color, borderWidth, borderColor);
    // }
    //
    // void drawForPlayer(
    //     cocos2d::CCDrawNode* node, PlayerObject* player, const gui::Color& color, float borderWidth,
    //     const gui::Color& innerColor
    // ) {
    //     cocos2d::CCRect rect1 = player->getObjectRect();
    //     cocos2d::CCRect rect2 = player->m_vehicleSize >= 1.f
    //                                 ? player->getObjectRect(0.25f, 0.25f)
    //                                 : player->getObjectRect(0.4f, 0.4f);
    //
    //     drawRect(node, rect1, color, borderWidth, {color.r, color.g, color.b, 1.f});
    //     drawRect(node, rect2, innerColor, borderWidth, {innerColor.r, innerColor.g, innerColor.b, 1.f});
    // }
    //
    // void customDraw(cocos2d::CCDrawNode* drawNode, gui::Color& color, float& borderSize, gui::Color& borderColor) {
    //     if (s_skipDrawHook) {
    //         s_skipDrawHook = false;
    //         return;
    //     }
    //
    //     GJBaseGameLayer* bgl = utils::get<GJBaseGameLayer>();
    //
    //     if (!bgl || drawNode != bgl->m_debugDrawNode) return;
    //     if (!config::get<"level.showhitboxes", bool>(false)) return;
    //
    //     bool hidePlayer = false;
    //
    //     bool customColors = config::get<"level.showhitboxes.customcolors", bool>();
    //
    //     switch (HitboxType type = getHitboxType(borderColor)) {
    //         case HitboxType::Solid:
    //             borderColor = !customColors ? borderColor
    //                 : config::get<"level.showhitboxes.solid_color", gui::Color>(gui::Color(0, 0.247, 1));
    //             break;
    //         case HitboxType::Danger:
    //             borderColor = !customColors ? borderColor
    //                 : config::get<"level.showhitboxes.danger_color", gui::Color>(gui::Color(1, 0, 0));
    //             break;
    //         case HitboxType::Player:
    //             borderColor = !customColors ? borderColor
    //                 : config::get<"level.showhitboxes.player_color_rotated", gui::Color>(gui::Color(1, 1, 0));
    //             hidePlayer = config::get<"level.showhitboxes.hideplayer", bool>(false);
    //             if (hidePlayer)
    //                 borderColor = gui::Color(0.f, 0.f, 0.f, 0.f);
    //             break;
    //         case HitboxType::Other:
    //             borderColor = !customColors ? borderColor
    //                 : config::get<"level.showhitboxes.other_color", gui::Color>(gui::Color(0, 1, 0));
    //             break;
    //     }
    //
    //     borderSize = hidePlayer ? 0.f : config::get<"level.showhitboxes.bordersize", float>(borderSize);
    //
    //     if (config::get<"level.showhitboxes.fillalpha.toggle", bool>()) {
    //         color.r = borderColor.r;
    //         color.g = borderColor.g;
    //         color.b = borderColor.b;
    //         color.a = hidePlayer ? 0.f : config::get<"level.showhitboxes.fillalpha", float>(0.25f);
    //     }
    // }
    //
    // class $modify(ShowHitboxesCCDNHook, cocos2d::CCDrawNode) {
    //     bool drawPolygon(
    //         cocos2d::CCPoint* vertex, unsigned int count, const cocos2d::ccColor4F& fillColor,
    //         float borderWidth, const cocos2d::ccColor4F& borderColor
    //     ) {
    //         borderWidth = abs(borderWidth);
    //
    //         customDraw(this, (gui::Color&) fillColor, borderWidth, (gui::Color&) borderColor);
    //         return CCDrawNode::drawPolygon(vertex, count, fillColor, borderWidth, borderColor);
    //     }
    //
    //     bool drawCircle(
    //         const cocos2d::CCPoint& position, float radius, const cocos2d::ccColor4F& color,
    //         float borderWidth, const cocos2d::ccColor4F& borderColor, unsigned int segments
    //     ) {
    //         borderWidth = abs(borderWidth);
    //
    //         customDraw(this, (gui::Color&) color, borderWidth, (gui::Color&) borderColor);
    //         return CCDrawNode::drawCircle(position, radius, color, borderWidth, borderColor, segments);
    //     }
    // };
    //
    // void forceDraw(GJBaseGameLayer* self, bool editor) {
    //     if (editor && !config::get<"level.showhitboxes.editor", bool>()) return;
    //     bool show = config::get<"level.showhitboxes", bool>();
    //     bool robtopShow = editor || robtopHitboxCheck();
    //     self->m_debugDrawNode->setVisible(show || robtopShow);
    //
    //     bool onDeath = config::get<"level.showhitboxes.ondeath", bool>();
    //     bool onDeathCollide = config::get<"level.showhitboxes.ondeath.player", bool>();
    //
    //     if (!show) return;
    //     if (onDeath || onDeathCollide) {
    //         self->m_debugDrawNode->setVisible(s_isDead || robtopShow);
    //         if (!s_isDead && !editor) return;
    //         if (s_isDead && s_collisionObject != nullptr && onDeathCollide) {
    //             self->m_debugDrawNode->clear();
    //             bool customColors = config::get<"level.showhitboxes.customcolors", bool>();
    //             gui::Color borderColor;
    //             switch (s_collisionObject->m_objectType) {
    //                 case GameObjectType::Solid:
    //                     borderColor = customColors ? config::get<"level.showhitboxes.solid_color", gui::Color>(gui::Color(0, 0.247, 1)) : gui::Color(0, 0.247, 1);
    //                     break;
    //                 case GameObjectType::Hazard:
    //                     borderColor = customColors ? config::get<"level.showhitboxes.danger_color", gui::Color>(gui::Color(1, 0, 0)) : gui::Color(1, 0, 0);
    //                     break;
    //                 default:
    //                     borderColor = customColors ? config::get<"level.showhitboxes.other_color", gui::Color>(gui::Color(0, 1, 0)) : gui::Color(0, 1, 0);
    //                     break;
    //             }
    //             drawRect(
    //                 self->m_debugDrawNode,
    //                 s_collisionObject->getObjectRect(),
    //                 gui::Color(borderColor.r, borderColor.g, borderColor.b, 0.f),
    //                 0.25f,
    //                 borderColor
    //             );
    //         }
    //     }
    //
    //
    //     if (!config::get<"level.showhitboxes.hideplayer", bool>()) {
    //         bool customColors = config::get<"level.showhitboxes.customcolors", bool>();
    //
    //         auto borderSize = config::get<"level.showhitboxes.bordersize", float>(0.25f);
    //
    //         float alpha = config::get<"level.showhitboxes.fillalpha.toggle", bool>()
    //                           ? config::get<"level.showhitboxes.fillalpha", float>(0.25f)
    //                           : 0.f;
    //
    //         gui::Color playerColor = customColors
    //                                      ? config::get<"level.showhitboxes.player_color", gui::Color>(gui::Color(1, 1, 0))
    //                                      : gui::Color(1, 0, 0, alpha);
    //         gui::Color playerColorInner = customColors
    //                                           ? config::get<"level.showhitboxes.player_color_inner", gui::Color>(gui::Color(1, 1, 0))
    //                                           : gui::Color(0, 1, 0, alpha);
    //         playerColor.a = alpha;
    //         playerColorInner.a = alpha;
    //
    //         drawForPlayer(self->m_debugDrawNode, self->m_player1, playerColor, borderSize, playerColorInner);
    //         if (self->m_gameState.m_isDualMode)
    //             drawForPlayer(self->m_debugDrawNode, self->m_player2, playerColor, borderSize, playerColorInner);
    //
    //         if (config::get<bool>("level.showhitboxes.traillength.toggle", false)) {
    //             for (const auto& [rect, _] : s_playerTrail1)
    //                 drawRect(
    //                     self->m_debugDrawNode, rect,
    //                     playerColor, borderSize,
    //                     { playerColor.r, playerColor.g, playerColor.b, 1.f }
    //                 );
    //
    //             for (const auto& [rect, _] : s_playerTrail2)
    //                 drawRect(
    //                     self->m_debugDrawNode, rect,
    //                     playerColor, borderSize,
    //                     { playerColor.r, playerColor.g, playerColor.b, 1.f }
    //                 );
    //
    //             for (const auto& [_, rect] : s_playerTrail1)
    //                 drawRect(
    //                     self->m_debugDrawNode, rect,
    //                     playerColorInner, borderSize,
    //                     { playerColorInner.r, playerColorInner.g, playerColorInner.b, 1.f }
    //                 );
    //
    //             for (const auto& [_, rect] : s_playerTrail2)
    //                 drawRect(
    //                     self->m_debugDrawNode, rect,
    //                     playerColorInner, borderSize,
    //                     { playerColorInner.r, playerColorInner.g, playerColorInner.b, 1.f }
    //                 );
    //         }
    //     }
    // }
    //
    // class $modify(ShowHitboxesLELHook, LevelEditorLayer) {
    //     void updateEditor(float dt) {
    //         LevelEditorLayer::updateEditor(dt);
    //
    //         forceDraw(this, true);
    //     }
    // };
    //
    // class $modify(ShowHitboxesPLHook, PlayLayer) {
    //     void updateProgressbar() {
    //         PlayLayer::updateProgressbar();
    //
    //         // only call updateDebugDraw if it wasn't called yet to prevent overdraw
    //         if (shouldDrawHitboxes() && !robtopHitboxCheck())
    //             PlayLayer::updateDebugDraw();
    //
    //         forceDraw(this, false);
    //     }
    //
    //     void resetLevel() {
    //         PlayLayer::resetLevel();
    //
    //         s_isDead = false;
    //         s_collisionObject = nullptr;
    //
    //         s_playerTrail1.clear();
    //         s_playerTrail2.clear();
    //     }
    //
    //     void destroyPlayer(PlayerObject *p0, GameObject *p1) {
    //         PlayLayer::destroyPlayer(p0, p1);
    //         if (m_anticheatSpike == p1) return;
    //         if (p1 != nullptr)
    //             s_collisionObject = p1;
    //     }
    // };
    //
    // class $modify(ShowHitboxesPOHook, PlayerObject) {
    //     ENABLE_FIRST_HOOKS_ALL()
    //
    //     void playerDestroyed(bool p0) {
    //         if (auto* pl = utils::get<PlayLayer>())
    //             s_isDead = this == pl->m_player1 || this == pl->m_player2;
    //         PlayerObject::playerDestroyed(p0);
    //     }
    //     void collidedWithObject(float p0, GameObject* p1, cocos2d::CCRect p2, bool p3) {
    //         s_collisionObject = p1;
    //         PlayerObject::collidedWithObject(p0, p1, p2, p3);
    //     }
    // };
    //
    // class $modify(ShowHitboxesBGLHook, GJBaseGameLayer) {
    //     bool init() override {
    //         s_collisionObject = nullptr;
    //         return GJBaseGameLayer::init();
    //     }
    //
    //     void processCommands(float dt) {
    //         GJBaseGameLayer::processCommands(dt);
    //
    //         if (s_isDead || !config::get<"level.showhitboxes.traillength.toggle", bool>(false))
    //             return;
    //
    //         s_playerTrail1.emplace_back(
    //             m_player1->getObjectRect(),
    //             m_player1->m_vehicleSize >= 1.f
    //                 ? m_player1->getObjectRect(0.25f, 0.25f)
    //                 : m_player1->getObjectRect(0.4f, 0.4f)
    //         );
    //
    //         if (m_gameState.m_isDualMode) {
    //             s_playerTrail2.emplace_back(
    //                 m_player2->getObjectRect(),
    //                 m_player2->m_vehicleSize >= 1.f
    //                     ? m_player2->getObjectRect(0.25f, 0.25f)
    //                     : m_player2->getObjectRect(0.4f, 0.4f)
    //             );
    //         }
    //
    //         auto max = static_cast<int>(config::get<"level.showhitboxes.traillength", float>(240.f));
    //
    //         while (s_playerTrail1.size() > max)
    //             s_playerTrail1.pop_front();
    //
    //         while (s_playerTrail2.size() > max)
    //             s_playerTrail2.pop_front();
    //     }
    //
    //     void updateDebugDraw() override {
    //         auto ptr1 = reinterpret_cast<uintptr_t>(this);
    //         auto ptr2 = reinterpret_cast<uintptr_t>(utils::get<LevelEditorLayer>());
    //
    //         // unlock hitboxes in editor even if they are disabled
    //         if (ptr1 == ptr2 && config::get<"level.showhitboxes.editor", bool>(false))
    //             this->m_isDebugDrawEnabled |= config::get<"level.showhitboxes", bool>(false);
    //
    //         s_slopeHitboxFix = true;
    //         GJBaseGameLayer::updateDebugDraw();
    //         s_slopeHitboxFix = false;
    //     }
    // };
    //
    // class $modify(ShowHitboxesGOHook, GameObject) {
    //     ENABLE_SAFE_HOOKS_ALL()
    //
    //     void determineSlopeDirection() {
    //         /*
    //          * This is a fix for the slope hitbox becoming flipped during mirror portal transition.
    //          * I explained it in details for a Misc Bugfixes PR:
    //          * https://github.com/Cvolton/miscbugfixes-geode/pull/10
    //          */
    //         if (s_slopeHitboxFix) return;
    //         GameObject::determineSlopeDirection();
    //     }
    // };
    //
    // class $modify(ShowHitboxesEditorHook, EditorUI) {
    //     void onPlaytest(cocos2d::CCObject *sender) {
    //         EditorUI::onPlaytest(sender);
    //
    //         s_isDead = false;
    //         s_collisionObject = nullptr;
    //
    //         s_playerTrail1.clear();
    //         s_playerTrail2.clear();
    //     }
    // };

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

            toggle->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                options->addToggle("level.showhitboxes.editor");
                options->addToggle("level.showhitboxes.hideplayer");
                options->addToggle("level.showhitboxes.customcolors")->addOptions([](std::shared_ptr<gui::MenuTab> optionsColor) {
                    optionsColor->addColorComponent("level.showhitboxes.solid_color");
                    optionsColor->addColorComponent("level.showhitboxes.danger_color");
                    optionsColor->addColorComponent("level.showhitboxes.other_color");
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
        }

        [[nodiscard]] bool isCheating() const override {
            auto enabled = config::get<"level.showhitboxes", bool>();
            auto onDeath = config::get<"level.showhitboxes.ondeath", bool>();
            if (onDeath)
                return false; // on-death hitboxes are fine

            if (auto* pl = utils::get<PlayLayer>())
                // if not in practice with enabled hitboxes
                    return enabled && !pl->m_isPracticeMode;

            return false;
        }

        [[nodiscard]] const char* getId() const override { return "Show Hitboxes"; }
    };

    REGISTER_HACK(ShowHitboxes)

    static void forEachObject(GJBaseGameLayer const* game, std::function<void(GameObject*)> const& callback) {
        int count = game->m_sections.empty() ? -1 : game->m_sections.size();
        for (int i = game->m_leftSectionIndex; i <= game->m_rightSectionIndex && i < count; ++i) {
            auto leftSection = game->m_sections[i];
            if (!leftSection) continue;

            auto leftSectionSize = leftSection->size();
            for (int j = game->m_bottomSectionIndex; j <= game->m_topSectionIndex && j < leftSectionSize; ++j) {
                auto section = leftSection->at(j);
                if (!section) continue;

                auto sectionSize = section->size();
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
            cocos2d::CCPoint(rect.getMinX(), rect.getMinY()),
            cocos2d::CCPoint(rect.getMinX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMinY())
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

        void createDrawNode() {
            auto* parent = m_debugDrawNode->getParent();
            if (!parent) return;

            auto* drawNode = cocos2d::CCDrawNode::create();
            drawNode->setID("hitboxes"_spr);
            parent->addChild(drawNode, 1402);

            m_fields->m_drawNode = drawNode;
        }

        cocos2d::CCDrawNode* getDrawNode() {
            return m_fields->m_drawNode;
        }

        void visitHitboxes() {
            if (!config::get<"level.showhitboxes", bool>(false)) return;

            auto* drawNode = m_fields->m_drawNode;
            if (!drawNode) return;

            drawNode->clear();
            auto solidColor = config::get<"level.showhitboxes.solid_color", gui::Color>(gui::Color(0, 0.247, 1));
            auto dangerColor = config::get<"level.showhitboxes.danger_color", gui::Color>(gui::Color(1, 0, 0));
            auto borderSize = config::get<"level.showhitboxes.bordersize", float>(0.25f);

            forEachObject(this, [&](GameObject* obj) {
                switch (obj->m_objectType) {
                    case GameObjectType::Solid: {
                        if (!obj->m_isActivated) break;
                        drawRect(drawNode, obj->getObjectRect(), solidColor, borderSize, solidColor);
                        break;
                    }
                    case GameObjectType::AnimatedHazard:
                    case GameObjectType::Hazard: {
                        if (!obj->m_isActivated || obj == m_anticheatSpike) break;
                        if (auto radius = getObjectRadius(obj); radius > 0) {
                            drawNode->drawCircle(
                                obj->getPosition(), radius,
                                dangerColor, borderSize,
                                dangerColor, 16
                            );
                        } else {
                            drawRect(drawNode, obj->getObjectRect(), dangerColor, borderSize, dangerColor);
                        }
                        break;
                    }

                    default: break;
                }
            });
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
            $hitbox->visitHitboxes();
        }
    };
}
