#include <modules/gui/gui.hpp>
#include <modules/gui/color.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCDrawNode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GameObject.hpp>

namespace eclipse::hacks::Level {

    /// @brief Check whether hitboxes made by RobTop should be drawn. (e.g. in practice mode)
    inline bool robtopHitboxCheck() {
        if (auto* pl = PlayLayer::get())
            return pl->m_isPracticeMode && GameManager::get()->getGameVariable("0166");
        else if (auto* ed = LevelEditorLayer::get())
            return GameManager::get()->getGameVariable("0045");
        return false;
    }

    /// @brief Gracefully disable hitboxes to return to the original state
    inline void toggleOffHitboxes() {
        if (config::get<bool>("level.showhitboxes", false)) return;
        if (auto* gjbgl = GJBaseGameLayer::get())
            gjbgl->m_debugDrawNode->setVisible(robtopHitboxCheck());
    }

    static bool s_isDead = false;
    static bool s_skipDrawHook = false;
    static bool s_slopeHitboxFix = false;

    static std::deque<std::pair<cocos2d::CCRect, cocos2d::CCRect>> s_playerTrail1, s_playerTrail2;

    class ShowHitboxes : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            gui::ToggleComponent* toggle = tab->addToggle("Show Hitboxes", "level.showhitboxes")->handleKeybinds();

            toggle->callback([](bool value) {
                if (PlayLayer::get()) PlayLayer::get()->updateProgressbar();

                if (LevelEditorLayer::get()) {
                    LevelEditorLayer::get()->updateEditor(0);
                    LevelEditorLayer::get()->updateOptions();
                }
                toggleOffHitboxes();
            });

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
                options->addToggle("Hide Player", "level.showhitboxes.hideplayer");
                options->addToggle("Hitboxes On Death", "level.showhitboxes.ondeath")->handleKeybinds();
                options->addToggle("Custom Colors", "level.showhitboxes.customcolors")->addOptions([](std::shared_ptr<gui::MenuTab> optionsColor) {
                    optionsColor->addColorComponent("Solid Color", "level.showhitboxes.solid_color");
                    optionsColor->addColorComponent("Danger Color", "level.showhitboxes.danger_color");
                    optionsColor->addColorComponent("Other Color", "level.showhitboxes.other_color");
                    optionsColor->addColorComponent("Player Color", "level.showhitboxes.player_color");
                    optionsColor->addColorComponent("Player Color Inner", "level.showhitboxes.player_color_inner");
                    optionsColor->addColorComponent("Player Color Rotated", "level.showhitboxes.player_color_rotated");
                });
                options->addInputFloat("Border Size", "level.showhitboxes.bordersize", 0.01f, 10.f, "%.2f");
                options->addFloatToggle("Fill Alpha", "level.showhitboxes.fillalpha", 0.f, 1.f);
                options->addFloatToggle("Trail Length", "level.showhitboxes.traillength", 1.f, 2000.f, "%.0f");
            });
        }

        [[nodiscard]] bool isCheating() override {
            bool enabled = config::get<bool>("level.showhitboxes", false); 
            bool onDeath = config::get<bool>("level.showhitboxes.ondeath", false);
            if (onDeath) return false; // on-death hitboxes are fine

            if (auto* pl = PlayLayer::get())
                // if not in practice with enabled hitboxes
                return enabled && !pl->m_isPracticeMode;
            
            return false;
        }
        [[nodiscard]] const char* getId() const override { return "Show Hitboxes"; }
    };

    REGISTER_HACK(ShowHitboxes)

    enum HitboxType {
        Solid,
        Danger,
        Player,
        Other
    };

    inline HitboxType getHitboxType(const gui::Color& color) {
        if (color.r == 0.0f)
            return color.b == 1.0f ? HitboxType::Solid : HitboxType::Other;
        else if (color.g == 1.0f)
            return HitboxType::Player;
        return HitboxType::Danger;
    }

    inline void drawRect(cocos2d::CCDrawNode* node, const cocos2d::CCRect& rect, const gui::Color& color,
                         float borderWidth, const gui::Color& borderColor) {
        std::vector<cocos2d::CCPoint> vertices = {
            cocos2d::CCPoint(rect.getMinX(), rect.getMinY()),
            cocos2d::CCPoint(rect.getMinX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMaxY()),
            cocos2d::CCPoint(rect.getMaxX(), rect.getMinY())
        };
        s_skipDrawHook = true;
        
        node->drawPolygon(vertices.data(), vertices.size(), color, borderWidth, borderColor);
    }

    void drawForPlayer(cocos2d::CCDrawNode* node, PlayerObject* player, const gui::Color& color, float borderWidth, const gui::Color& innerColor) {
        cocos2d::CCRect rect1 = player->getObjectRect();
        cocos2d::CCRect rect2 = player->m_vehicleSize >= 1.f ? player->getObjectRect(0.25f, 0.25f) : player->getObjectRect(0.4f, 0.4f);

        drawRect(node, rect1, color, borderWidth, {color.r, color.g, color.b, 1.f});
        drawRect(node, rect2, innerColor, borderWidth, {innerColor.r, innerColor.g, innerColor.b, 1.f});
    }

    void customDraw(cocos2d::CCDrawNode* drawNode, gui::Color& color, float& borderSize, gui::Color& borderColor) {
        if (s_skipDrawHook) {
            s_skipDrawHook = false;
            return;
        }

        GJBaseGameLayer* bgl = GJBaseGameLayer::get();

        if (!bgl || drawNode != bgl->m_debugDrawNode) return;
        if (!config::get<bool>("level.showhitboxes", false)) return;

        bool hidePlayer = false;

        bool customColors = config::get<bool>("level.showhitboxes.customcolors", false);

        HitboxType type = getHitboxType(borderColor);

        switch (type) {
            case HitboxType::Solid:
                borderColor = !customColors ? borderColor : config::get<gui::Color>("level.showhitboxes.solid_color", gui::Color(0, 0.247, 1));
                break;
            case HitboxType::Danger:
                borderColor = !customColors ? borderColor : config::get<gui::Color>("level.showhitboxes.danger_color", gui::Color(1, 0, 0));
                break;
            case HitboxType::Player:
                borderColor = !customColors ? borderColor : config::get<gui::Color>("level.showhitboxes.player_color_rotated", gui::Color(1, 1, 0));
                hidePlayer = config::get<bool>("level.showhitboxes.hideplayer", false);
                if (hidePlayer)
                    borderColor = gui::Color(0.f, 0.f, 0.f, 0.f);
                break;
            case HitboxType::Other:
                borderColor = !customColors ? borderColor : config::get<gui::Color>("level.showhitboxes.other_color", gui::Color(0, 1, 0));
                break;
        }

        borderSize = hidePlayer ? 0.f : config::get<float>("level.showhitboxes.bordersize", borderSize);

        if (config::get<bool>("level.showhitboxes.fillalpha.toggle", false)) {
            color.r = borderColor.r;
            color.g = borderColor.g;
            color.b = borderColor.b;
            color.a = hidePlayer ? 0.f : config::get<float>("level.showhitboxes.fillalpha", 0.25f);
        }
    }

    class $modify(ShowHitboxesCCDNHook, cocos2d::CCDrawNode) {
        bool drawPolygon(cocos2d::CCPoint* vertex, unsigned int count, const cocos2d::ccColor4F& fillColor,
                         float borderWidth, const cocos2d::ccColor4F& borderColor) {
            borderWidth = abs(borderWidth);

            customDraw(this, (gui::Color&) fillColor, borderWidth, (gui::Color&) borderColor);
            return cocos2d::CCDrawNode::drawPolygon(vertex, count, fillColor, borderWidth, borderColor);
        }

        bool drawCircle(const cocos2d::CCPoint& position, float radius, const cocos2d::ccColor4F& color,
                        float borderWidth, const cocos2d::ccColor4F& borderColor, unsigned int segments) {
            borderWidth = abs(borderWidth);

            customDraw(this, (gui::Color&) color, borderWidth, (gui::Color&) borderColor);
            return cocos2d::CCDrawNode::drawCircle(position, radius, color, borderWidth, borderColor, segments);
        }
    };

    void forceDraw(GJBaseGameLayer* self, bool editor) {
        bool show = config::get<bool>("level.showhitboxes", false);
        bool robtopShow = editor || robtopHitboxCheck();
        self->m_debugDrawNode->setVisible(show || robtopShow);

        bool onDeath = config::get<bool>("level.showhitboxes.ondeath", false);

        if (!show) return;
        if (onDeath) {
            self->m_debugDrawNode->setVisible(s_isDead || robtopShow);
            if (!s_isDead && !editor) return;
        }

        if (!config::get<bool>("level.showhitboxes.hideplayer", false)) {
            bool customColors = config::get<bool>("level.showhitboxes.customcolors", false);

            auto borderSize = config::get<float>("level.showhitboxes.bordersize", 0.25f);

            float alpha = config::get<bool>("level.showhitboxes.fillalpha.toggle", false) ? config::get<float>("level.showhitboxes.fillalpha", 0.25f) : 0.f;

            gui::Color playerColor = customColors ? config::get<gui::Color>("level.showhitboxes.player_color", gui::Color(1, 1, 0)) : gui::Color(1, 0, 0, alpha);
            gui::Color playerColorInner = customColors ? config::get<gui::Color>("level.showhitboxes.player_color_inner", gui::Color(1, 1, 0)) : gui::Color(0, 1, 0, alpha);
            playerColor.a = alpha;
            playerColorInner.a = alpha;

            drawForPlayer(self->m_debugDrawNode, self->m_player1, playerColor, borderSize, playerColorInner);
            if (self->m_gameState.m_isDualMode)
                drawForPlayer(self->m_debugDrawNode, self->m_player2, playerColor, borderSize, playerColorInner);

            if (config::get<bool>("level.showhitboxes.traillength.toggle", false)) {
                for (const auto& [rect, _] : s_playerTrail1)
                    drawRect(self->m_debugDrawNode, rect, playerColor, borderSize, {playerColor.r, playerColor.g, playerColor.b, 1.f});

                for (const auto& [rect, _] : s_playerTrail2)
                    drawRect(self->m_debugDrawNode, rect, playerColor, borderSize, {playerColor.r, playerColor.g, playerColor.b, 1.f});

                for (const auto& [_, rect] : s_playerTrail1)
                    drawRect(self->m_debugDrawNode, rect, playerColorInner, borderSize, {playerColorInner.r, playerColorInner.g, playerColorInner.b, 1.f});

                for (const auto& [_, rect] : s_playerTrail2)
                    drawRect(self->m_debugDrawNode, rect, playerColorInner, borderSize, {playerColorInner.r, playerColorInner.g, playerColorInner.b, 1.f});
            }
        }
    }

    class $modify(ShowHitboxesLELHook, LevelEditorLayer) {
        void updateEditor(float dt) {
            LevelEditorLayer::updateEditor(dt);

            forceDraw(this, true);
        }
    };

    class $modify(ShowHitboxesPLHook, PlayLayer) {
        void updateProgressbar() {
            PlayLayer::updateProgressbar();

            // only call updateDebugDraw if it wasn't called yet to prevent overdraw
            if (config::get<bool>("level.showhitboxes", false) && !robtopHitboxCheck())
                PlayLayer::updateDebugDraw();

            forceDraw(this, false);
        }

        void resetLevel() {
            PlayLayer::resetLevel();

            s_isDead = false;

            s_playerTrail1.clear();
            s_playerTrail2.clear();
        }
    };

    class $modify(ShowHitboxesPOHook, PlayerObject) {
        void playerDestroyed(bool p0) {
            if (auto* pl = PlayLayer::get())
                s_isDead = this == pl->m_player1 || this == pl->m_player2;
            PlayerObject::playerDestroyed(p0);
        }
    };

    class $modify(ShowHitboxesBGLHook, GJBaseGameLayer) {
        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            if (s_isDead || !config::get<bool>("level.showhitboxes.traillength.toggle", false))
                return;

            s_playerTrail1.emplace_back(
                m_player1->getObjectRect(),
                m_player1->m_vehicleSize >= 1.f ? m_player1->getObjectRect(0.25f, 0.25f) : m_player1->getObjectRect(0.4f, 0.4f)
            );

            if (m_gameState.m_isDualMode) {
                s_playerTrail2.emplace_back(
                    m_player2->getObjectRect(),
                    m_player2->m_vehicleSize >= 1.f ? m_player2->getObjectRect(0.25f, 0.25f) : m_player2->getObjectRect(0.4f, 0.4f)
                );
            }

            auto max = static_cast<int>(config::get<float>("level.showhitboxes.traillength", 240.f));

            while (s_playerTrail1.size() > max)
                s_playerTrail1.pop_front();

            while (s_playerTrail2.size() > max)
                s_playerTrail2.pop_front();
        }

        void updateDebugDraw() override {
            auto ptr1 = reinterpret_cast<uintptr_t>(this);
            auto ptr2 = reinterpret_cast<uintptr_t>(LevelEditorLayer::get());
            // unlock hitboxes in editor even if they are disabled
            if (ptr1 == ptr2) this->m_isDebugDrawEnabled |= config::get<bool>("level.showhitboxes", false);

            s_slopeHitboxFix = true;
            GJBaseGameLayer::updateDebugDraw();
            s_slopeHitboxFix = false;
        }
    };

    class $modify(ShowHitboxesGOHook, GameObject) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("GameObject::determineSlopeDirection");
        }

        void determineSlopeDirection() {
            if (s_slopeHitboxFix) return;
            GameObject::determineSlopeDirection();
        }
    };
}