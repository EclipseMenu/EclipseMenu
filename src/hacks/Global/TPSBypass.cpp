#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

#ifndef GEODE_IS_MACOS
constexpr float MIN_TPS = 0.f;
constexpr float MAX_TPS = 100000.f;

namespace eclipse::hacks::Global {
    class TPSBypass : public hack::Hack {
    public:
        void init() override {
            config::setIfEmpty("global.tpsbypass", 240.f);

            auto tab = gui::MenuTab::find("Global");
            tab->addFloatToggle("TPS Bypass", "global.tpsbypass", MIN_TPS, MAX_TPS, "%.0f TPS")
               ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "TPSBypass"; }
        [[nodiscard]] int32_t getPriority() const override { return -14; }
    };

    REGISTER_HACK(TPSBypass)

    class $modify(TPSBypassGJBGLHook, GJBaseGameLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("global.tpsbypass.toggle")

        float getModifiedDelta(float dt) {
            auto tps = config::get<float>("global.tpsbypass", 240.f);
            float speedhack = config::get<bool>("global.speedhack.toggle", false) ? config::get<float>("global.speedhack", 1.f) : 1.f;
            tps *= speedhack;
            auto spt = 1.f / tps;

            if (m_resumeTimer > 0) {
                --m_resumeTimer;
                dt = 0.f;
            }


            auto totalDelta = dt + m_unk3248;
            auto timestep = std::min(m_gameState.m_timeWarp, 1.f) * spt;
            auto steps = std::round(totalDelta / timestep);
            auto newDelta = steps * timestep;
            m_unk3248 = totalDelta - newDelta;
            return static_cast<float>(newDelta);
        }
    };

}
#endif