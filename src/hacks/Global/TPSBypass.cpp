#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/float-toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/utils/assembler.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/GJEffectManager.hpp>

#include <sinaps.hpp>

constexpr float MIN_TPS = 0.f;
constexpr float MAX_TPS = 100000.f;

#ifdef GEODE_IS_MACOS
#define REQUIRE_MODIFIED_DELTA_PATCH
#endif

namespace eclipse::hacks::Global {
    using TicksType =
        GEODE_WINDOWS(uint32_t)
        GEODE_ANDROID64(uint32_t)
        GEODE_ANDROID32(float);

    static TicksType g_expectedTicks = 0;

    class $hack(TPSBypass) {
    public:
        void init() override {
            config::setIfEmpty("global.tpsbypass", 240.f);

            // this patch allows us to manually set the expected amount of ticks per update call
            uintptr_t addr = 0;
            std::vector<uint8_t> bytes;
            #ifdef GEODE_IS_WINDOWS
            {
                using namespace assembler::x86_64;
                addr = geode::base::get() + 0x232294; // TODO: sigscan
                bytes = Builder(addr)
                    .movabs(Register64::rax, std::bit_cast<uint64_t>(&g_expectedTicks))
                    .mov(Register32::r11d, Register64::rax)
                    .jmp(addr + 0x43, true)
                    .nop(4)
                    .build();
            }
            #elif defined(GEODE_IS_ANDROID64)
            {
                using namespace assembler::arm64;
                addr = geode::base::get() + 0x87DA40; // TODO: sigscan
                bytes = Builder(addr)
                    .mov(Register::x9, std::bit_cast<uint64_t>(&g_expectedTicks))
                    .ldr(Register::w0, Register::x9)
                    .b(addr + 0x2c, true)
                    .build();
            }
            #elif defined(GEODE_IS_ANDROID32)
            {
                using namespace assembler::armv7;
                addr = geode::base::get() + 0x4841BC; // TODO: sigscan
                bytes = Builder(addr)
                    .mov(Register::r1, std::bit_cast<uint32_t>(&g_expectedTicks))
                    .ldr_t(Register::r0, Register::r1)
                    .nop_t()
                    .build();
            }
            #endif

            if (!addr || bytes.empty()) {
                geode::log::error("TPS Bypass: Failed to find patch address or bytes");
                config::set("global.tpsbypass.toggle", false);
                // add a safeguard to disable tps bypass if it gets enabled
                config::addDelegate("global.tpsbypass.toggle", []() {
                    if (config::get<bool>("global.tpsbypass.toggle", false))
                        config::set("global.tpsbypass.toggle", false);
                });
                return;
            }

            geode::Patch* patch = nullptr;
            if (auto res = geode::Mod::get()->patch(reinterpret_cast<void*>(addr), bytes)) {
                patch = res.unwrap();
            } else {
                geode::log::error("TPS Bypass: Failed to patch GJBaseGameLayer::update: {}", res.unwrapErr());
            }

            // patch toggler
            if (!patch) {
                geode::log::error("TPS Bypass: Failed to patch GJBaseGameLayer::update");
                config::set("global.tpsbypass.toggle", false);
                // add a safeguard to disable tps bypass if it gets enabled
                config::addDelegate("global.tpsbypass.toggle", []() {
                    if (config::get<bool>("global.tpsbypass.toggle", false))
                        config::set("global.tpsbypass.toggle", false);
                });
                return;
            }

            // toggle the patch if enabled
            config::addDelegate("global.tpsbypass.toggle", [patch] {
                (void) (config::get<bool>("global.tpsbypass.toggle", false)
                    ? patch->enable()
                    : patch->disable());
            });

            // set the initial state of the patch
            (void) (config::get<bool>("global.tpsbypass.toggle", false)
                    ? patch->enable()
                    : patch->disable());

            // on macOS, we also have to patch instructions in GJBaseGameLayer::getModifiedDelta because it's inlined
            #ifdef REQUIRE_MODIFIED_DELTA_PATCH
            auto res = setupModifiedDeltaPatches();
            if (!res) {
                geode::log::error("TPSBypass: {}", res.unwrapErr());
                config::set("global.tpsbypass.toggle", false);
                // add a safeguard to disable tps bypass if it gets enabled
                config::addDelegate("global.tpsbypass.toggle", []() {
                    if (config::get<bool>("global.tpsbypass.toggle", false))
                        config::set("global.tpsbypass.toggle", false);
                });
                return;
            }
            #endif

            auto tab = gui::MenuTab::find("tab.global");
            tab->addFloatToggle("global.tpsbypass", MIN_TPS, MAX_TPS, "%.2f TPS")
               ->setDescription()
               ->handleKeybinds()
               ->toggleCallback([]() {
                   config::set("bot.original.tpsbypass", config::get<bool>("global.tpsbypass.toggle", false));
               });
        }

        #ifdef REQUIRE_MODIFIED_DELTA_PATCH

        // function to quickly get the bytes for the patch
        #ifdef GEODE_IS_INTEL_MAC
        template <typename T>
        [[nodiscard]] static std::vector<uint8_t> TPStoBytes() {
            if constexpr (std::is_same_v<T, float>) {
                return geode::toBytes(1.f / config::get<"global.tpsbypass", float>(240.f));
            } else if constexpr (std::is_same_v<T, double>) {
                return geode::toBytes<double>(1.0 / config::get<"global.tpsbypass", float>(240.f));
            } else {
                static_assert(alwaysFalse<T>, "TPStoBytes only supports float and double");
            }
        }
        #elif defined(GEODE_IS_ARM_MAC)
        template <typename T>
        [[nodiscard]] static std::vector<uint8_t> TPStoBytes() {
            if constexpr (std::is_same_v<T, float>) {
                auto bytes = assembler::arm64::mov_float(
                    assembler::arm64::Register::w9,
                    1.f / config::get<"global.tpsbypass", float>(240.f)
                );
                return std::vector(bytes.begin(), bytes.end());
            } else if constexpr (std::is_same_v<T, double>) {
                auto bytes = assembler::arm64::mov_double(
                    assembler::arm64::Register::x9,
                    1.0 / config::get<"global.tpsbypass", float>(240.f)
                );
                return std::vector(bytes.begin(), bytes.end());
            } else {
                static_assert(alwaysFalse<T>, "TPStoBytes only supports float and double");
            }
        }
        #endif

        geode::Result<> setupModifiedDeltaPatches() {
            auto base = reinterpret_cast<uint8_t*>(geode::base::get());
            auto moduleSize = utils::getBaseSize();
            geode::log::debug("TPSBypass: base = 0x{:X}", (uintptr_t)base);
            geode::log::debug("TPSBypass: moduleSize = 0x{:X}", moduleSize);

            using namespace sinaps::mask;

            #ifdef GEODE_IS_INTEL_MAC
            // on x86, we need to replace two values that are used in GJBaseGameLayer::getModifiedDelta
            // pretty simple, just find original values and replace them with raw bytes

            auto floatAddr = sinaps::find<dword<0x3B888889>>(base, moduleSize);
            auto doubleAddr = sinaps::find<qword<0x3F71111120000000>>(base, moduleSize);
            #elif defined(GEODE_IS_ARM_MAC)
            // on ARM, it's a bit tricky, since the value is not stored as a constant.
            //
            // assembly in question:
            // ; Load 0.0041667f
            // 29 11 91 52        movz w9, #0x8889
            // 09 71 A7 72        movk w9, #0x3b88, lsl #16
            // ...
            // ; Load 0.00416666688
            // 09 00 A4 D2        movz x9, #0x2000, lsl #16
            // 29 22 C2 F2        movk x9, #0x1111, lsl #32
            // 29 EE E7 F2        movk x9, #0x3f71, lsl #48
            // note that double value only stores upper 48 bits, so we're losing some precision here :(

            auto floatAddr = sinaps::find<"29 11 91 52 09 71 A7 72">(base, moduleSize);
            auto doubleAddr = sinaps::find<"09 00 A4 D2 29 22 C2 F2 29 EE E7 F2">(base, moduleSize);
            #endif

            if (floatAddr == -1 || doubleAddr == -1) {
                return geode::Err(fmt::format("failed to find addresses: float = 0x{:X}, double = 0x{:X}", floatAddr, doubleAddr));
            }

            geode::log::debug("TPSBypass: floatAddr = 0x{:X}", floatAddr);
            geode::log::debug("TPSBypass: doubleAddr = 0x{:X}", doubleAddr);

            auto patch1Res = geode::Mod::get()->patch(floatAddr + base, TPStoBytes<float>());
            if (!patch1Res) return geode::Err(fmt::format("failed to patch float address: {}", patch1Res.unwrapErr()));
            auto patch1 = patch1Res.unwrap();
            (void) patch1->disable();

            auto patch2Res = geode::Mod::get()->patch(doubleAddr + base, TPStoBytes<double>());
            if (!patch2Res) return geode::Err(fmt::format("failed to patch double address: {}", patch2Res.unwrapErr()));
            auto patch2 = patch2Res.unwrap();
            (void) patch2->disable();

            const auto setState = [patch1, patch2](bool state) {
                if (state) {
                    (void) patch1->enable();
                    (void) patch2->enable();
                } else {
                    (void) patch1->disable();
                    (void) patch2->disable();
                }
            };

            const auto setValue = [patch1, patch2]() {
                (void) patch1->updateBytes(TPStoBytes<float>());
                (void) patch2->updateBytes(TPStoBytes<double>());
            };

            // update bytes on value change
            config::addDelegate("global.tpsbypass", [setValue] { setValue(); });

            // update patches on enable/disable
            config::addDelegate("global.tpsbypass.toggle", [setState] {
                setState(config::get<"global.tpsbypass.toggle", bool>());
            });

            // apply initial state
            setState(config::get<"global.tpsbypass.toggle", bool>());

            return geode::Ok();
        }
        #endif

        [[nodiscard]] const char* getId() const override { return "Physics Bypass"; }
        [[nodiscard]] int32_t getPriority() const override { return -14; }

        [[nodiscard]] bool isCheating() const override {
            auto toggle = config::get<"global.tpsbypass.toggle", bool>();
            auto tps = config::get<"global.tpsbypass", float>(240.f);
            return toggle && tps != 240.f;
        }
    };

    REGISTER_HACK(TPSBypass)

    class $modify(TPSBypassGJBGLHook, GJBaseGameLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("global.tpsbypass.toggle")

        struct Fields {
            double m_extraDelta = 0.0;
            float m_visualDelta = 0.0;
        };

        static TPSBypassGJBGLHook* get() {
            return static_cast<TPSBypassGJBGLHook*>(GJBaseGameLayer::get());
        }

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

        #ifndef REQUIRE_MODIFIED_DELTA_PATCH
        float getModifiedDelta(float dt) {
            return getCustomDelta(dt, config::get<"global.tpsbypass", float>(240.f));
        }
        #endif

        void update(float dt) override {
            auto fields = m_fields.self();
            fields->m_extraDelta += dt;

            // calculate number of steps based on the new TPS
            auto newTPS = config::get<"global.tpsbypass", float>(240.f);
            auto spt = 1.0 / newTPS;
            auto steps = std::round(fields->m_extraDelta / spt);
            auto totalDelta = steps * spt;
            fields->m_extraDelta -= totalDelta;
            g_expectedTicks = steps;

            // for particles and other visual effects
            fields->m_visualDelta = totalDelta;

            GJBaseGameLayer::update(totalDelta);
        }

        void updateShaderLayer(float dt) {
            GJBaseGameLayer::updateShaderLayer(m_fields->m_visualDelta);
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
            PlayLayer::postUpdate(getFields(this)->m_visualDelta);
        }

        // we also would like to fix the percentage calculation, which uses constant 240 TPS to determine the progress
        int calculationFix() {
            auto timestamp = m_level->m_timestamp;
            auto currentProgress = m_gameState.m_currentProgress;
            // this is only an issue for 2.2+ levels (with TPS greater than 240)
            if (timestamp > 0 && config::get<"global.tpsbypass", float>(240.f) != 240.f) {
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
            if (config::get<"global.tpsbypass", float>(240.f) != 240.f) {
                auto ticks = static_cast<uint32_t>(std::round(m_gameState.m_levelTime * 240));
                m_gameState.m_unkUint2 = ticks;
            }
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
            if (!m_player1->m_maybeIsColliding && !m_player2->m_maybeIsColliding) return;
            LevelEditorLayer::postUpdate(fields->m_visualDelta);
        }
    };

    // few more hooks to restore speed of animations
    class $modify(TPSBypassHSHook, HardStreak) {
        ALL_DELEGATES_AND_SAFE_PRIO("global.tpsbypass.toggle")

        void updateStroke(float dt) {
            auto gjbgl = TPSBypassGJBGLHook::get();
            if (!gjbgl) return HardStreak::updateStroke(dt);
            HardStreak::updateStroke(gjbgl->m_fields->m_visualDelta);
        }
    };

    class $modify(TPSBypassGJEMHook, GJEffectManager) {
        ALL_DELEGATES_AND_SAFE_PRIO("global.tpsbypass.toggle")

        void updateEffects(float dt) {
            auto gjbgl = TPSBypassGJBGLHook::get();
            if (!gjbgl) return GJEffectManager::update(dt);
            GJEffectManager::update(gjbgl->m_fields->m_visualDelta);
        }
    };
}