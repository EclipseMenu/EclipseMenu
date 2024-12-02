#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <modules/labels/variables.hpp>

#ifndef GEODE_IS_MACOS
constexpr float MIN_TPS = 0.f;
constexpr float MAX_TPS = 100000.f;

namespace eclipse::hacks::Global {
    class TPSBypass : public hack::Hack {
    public:
        void init() override {
            config::setIfEmpty("global.tpsbypass", 240.f);

            auto tab = gui::MenuTab::find("Global");
            tab->addFloatToggle("Physics Bypass", "global.tpsbypass", MIN_TPS, MAX_TPS, "%.0f TPS")
               ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Physics Bypass"; }
        [[nodiscard]] int32_t getPriority() const override { return -14; }
        [[nodiscard]] bool isCheating() override {
            return config::get<bool>("global.tpsbypass.toggle", false) &&
                config::get<float>("global.tpsbypass", 240.f) != 240.f;
        }
    };

    REGISTER_HACK(TPSBypass)

    class $modify(TPSBypassGJBGLHook, GJBaseGameLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("global.tpsbypass.toggle")

        struct Fields {
            double m_extraDelta = 0.0;
            float m_realDelta = 0.0;
            bool m_shouldHide = false;
            bool m_isEditor = LevelEditorLayer::get() != nullptr;
            bool m_shouldBreak = false;
        };

        float getCustomDelta(float dt, float tps, bool applyExtraDelta = true) {
            auto spt = 1.f / tps;

            if (applyExtraDelta && m_resumeTimer > 0) {
                --m_resumeTimer;
                dt = 0.f;
            }

            auto totalDelta = dt + m_extraDelta;
            auto timestep = std::min(m_gameState.m_timeWarp, 1.f) * spt;
            auto steps = std::round(totalDelta / timestep);
            auto newDelta = steps * timestep;
            if (applyExtraDelta) m_extraDelta = totalDelta - newDelta;
            return static_cast<float>(newDelta);
        }

        float getModifiedDelta(float dt) {
            return getCustomDelta(dt, config::get("global.tpsbypass", 240.f));
        }

        bool shouldContinue(const Fields* fields) const {
            if (!fields->m_isEditor) return true;

            // in editor, player hitbox is removed from section when it dies,
            // so we need to check if it's still there
            return !fields->m_shouldBreak;
        }

        void update(float dt) override {
            auto fields = m_fields.self();
            fields->m_extraDelta += dt;
            fields->m_shouldBreak = false;

            // store current frame delta for later use in updateVisibility
            fields->m_realDelta = getCustomDelta(dt, 240.f, false);

            auto newTPS = config::get("global.tpsbypass", 240.f);
            auto newDelta = 1.0 / newTPS;

            if (fields->m_extraDelta >= newDelta) {
                // call original update several times, until the extra delta is less than the new delta
                size_t steps = fields->m_extraDelta / newDelta;
                fields->m_extraDelta -= steps * newDelta;

                auto start = utils::getTimestamp();
                auto ms = dt * 1000;
                fields->m_shouldHide = true;
                while (steps > 1 && shouldContinue(fields)) {
                    GJBaseGameLayer::update(newDelta);
                    auto end = utils::getTimestamp();
                    // if the update took too long, break out of the loop
                    if (end - start > ms) break;
                    --steps;
                }
                fields->m_shouldHide = false;

                if (shouldContinue(fields)) {
                    // call one last time with the remaining delta
                    GJBaseGameLayer::update(newDelta * steps);
                }
            }
        }
    };

    inline TPSBypassGJBGLHook::Fields* getFields(GJBaseGameLayer* self) {
        return reinterpret_cast<TPSBypassGJBGLHook*>(self)->m_fields.self();
    }

    // Skip some functions to make the game run faster during extra updates period

    class $modify(TPSBypassPLHook, PlayLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("global.tpsbypass.toggle")

        // PlayLayer postUpdate handles practice mode checkpoints, labels and also calls updateVisibility
        void postUpdate(float dt) override {
            auto fields = getFields(this);
            if (fields->m_shouldHide) return;
            PlayLayer::postUpdate(fields->m_realDelta);
        }

        // we also would like to fix the percentage calculation, which uses constant 240 TPS to determine the progress
        int calculationFix() {
            auto timestamp = m_level->m_timestamp;
            auto currentProgress = m_gameState.m_currentProgress;
            if (timestamp > 0) { // this is only an issue for 2.2+ levels
                // recalculate m_currentProgress based on the actual time passed
                auto progress = utils::getActualProgress(this);
                m_gameState.m_currentProgress = timestamp * progress / 100.f;
            }
            return currentProgress;
        }

        void updateProgressbar() {
            auto currentProgress = calculationFix();
            PlayLayer::updateProgressbar();
            m_gameState.m_currentProgress = currentProgress;
        }

        void destroyPlayer(PlayerObject* player, GameObject* object) override {
            auto currentProgress = calculationFix();
            PlayLayer::destroyPlayer(player, object);
            m_gameState.m_currentProgress = currentProgress;
        }

        void levelComplete() {
            // levelComplete uses m_gameState.m_unkUint2 to store the timestamp
            // also we can't rely on m_level->m_timestamp, because it might not be updated yet
            auto oldTimestamp = m_gameState.m_unkUint2;
            auto ticks = static_cast<uint32_t>(std::round(m_gameState.m_levelTime * 240));
            m_gameState.m_unkUint2 = ticks;
            PlayLayer::levelComplete();
            m_gameState.m_unkUint2 = oldTimestamp;
        }
    };

    class $modify(TPSBypassLELHook, LevelEditorLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("global.tpsbypass.toggle")

        // Editor postUpdate handles the exit of playback mode and player trail drawing and calls updateVisibility
        void postUpdate(float dt) override {
            // editor disables playback mode in postUpdate, so we should call the function if we're dead
            auto fields = getFields(this);
            if (fields->m_shouldHide && !m_player1->m_maybeIsColliding && !m_player2->m_maybeIsColliding) return;
            // m_maybeIsColliding will be reset in our inner update call next iteration,
            // so we need to store it here to check if we should break out of the loop
            fields->m_shouldBreak = m_player1->m_maybeIsColliding;
            LevelEditorLayer::postUpdate(fields->m_realDelta);
        }
    };

}
#endif