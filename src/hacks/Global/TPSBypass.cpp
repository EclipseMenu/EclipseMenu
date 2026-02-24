#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/float-toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/utils/assembler.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <sinaps.hpp>

constexpr double MIN_TPS = 0.f;
constexpr double MAX_TPS = 1000000.f;

#ifdef GEODE_IS_MACOS
#define REQUIRE_MODIFIED_DELTA_PATCH
#elif defined(GEODE_IS_ANDROID)
#include <dlfcn.h>
#endif

namespace eclipse::hacks::Global {
    using TicksType =
        GEODE_WINDOWS(uint32_t)
        GEODE_ANDROID64(uint32_t)
        GEODE_ANDROID32(float)
        GEODE_IOS(float)
        GEODE_ARM_MAC(float)
        GEODE_INTEL_MAC(float);

    static TicksType g_expectedTicks = 0;
    #ifdef GEODE_IS_IOS
    // iOS launcher has a secret space to store data, and it's located at 0x8b8000
    // 8 bytes are reserved for geode itself, so we can just skip 8 bytes
    constexpr uintptr_t g_jitlessSpace = 0x8c4008; // omg boob :o
    static TicksType* g_expectedTicksPtr = &g_expectedTicks;

    TicksType& expectedTicks() { return *g_expectedTicksPtr; }
    #else
    TicksType& expectedTicks() { return g_expectedTicks; }
    #endif

    class $hack(TPSBypass) {
        static void forceDisable() {
            config::set("global.tpsbypass.toggle", false);
            // add a safeguard to disable tps bypass if it gets enabled
            config::addDelegate("global.tpsbypass.toggle", []() {
                if (config::get<bool>("global.tpsbypass.toggle", false))
                    config::set("global.tpsbypass.toggle", false);
            });
        }

        void init() override {
            config::setIfEmpty("global.tpsbypass", 240.f);

        #ifdef GEODE_IS_IOS
            if (geode::Loader::get()->isPatchless()) {
                using namespace assembler::arm64;
                g_expectedTicksPtr = std::bit_cast<TicksType*>(geode::base::get() + g_jitlessSpace);
                static_assert(GEODE_COMP_GD_VERSION == 22074, "TPS Bypass: JIT-less patch is only supported for GD 2.2074");
                #define PATCH_ADDR 0x200C30
                GEODE_MOD_STATIC_PATCH(PATCH_ADDR, Builder()
                    .adrp(Register::x9, g_jitlessSpace - ((PATCH_ADDR >> 12) << 12))
                    .ldr(FloatRegister::s0, Register::x9, 0x8)
                    .pad_nops(20)
                    .build_array<20>());
            } else {
                // we can do normal patches with JIT
        #endif

            auto base = reinterpret_cast<uint8_t*>(geode::base::get());
            auto baseSize = utils::getBaseSize();

            // this patch allows us to manually set the expected amount of ticks per update call
            intptr_t addr = sinaps::not_found;
            std::vector<uint8_t> bytes;
            #ifdef GEODE_IS_WINDOWS
            {
                using namespace assembler::x86_64;
                // 2.2074: 0x232294
                // 2.2081: 0x237a55
                addr = sinaps::find<"FF 90 ? ? ? ? ^ F3 0F 10 ? ? ? ? ? F3 44 0F 10 ? ? ? ? 00 F3 41 0F 5D">(base, baseSize);
                if (addr != sinaps::not_found) {
                    bytes = Builder(addr)
                        .movabs(Register64::rax, std::bit_cast<uint64_t>(&g_expectedTicks))
                        .mov(Register32::r11d, Register64::rax)
                        .jmp(addr + 0x43, true)
                        .nop(4)
                        .build();
                }
            }
            #elif defined(GEODE_IS_ANDROID64)
            {
                using namespace assembler::arm64;
                // 2.2074: 0x87dA40 (google) / 0x87be28 (amazon)
                // 2.2081: 0x89d9f4
                auto func = dlsym(RTLD_DEFAULT, "_ZN15GJBaseGameLayer6updateEf");
                addr = sinaps::find<"AB 19 60 1E 0A 10 62 1E 6A 09 6A 1E">(static_cast<const uint8_t*>(func), 0x500);
                if (addr != sinaps::not_found) {
                    addr += reinterpret_cast<intptr_t>(func) - reinterpret_cast<intptr_t>(base); // we need offset from the base
                    bytes = Builder(addr)
                        .mov(Register::x9, std::bit_cast<uint64_t>(&g_expectedTicks))
                        .ldr(Register::w0, Register::x9)
                        .b(addr + 0x28, true)
                        .build();
                }
            }
            #elif defined(GEODE_IS_ANDROID32)
            {
                using namespace assembler::armv7;
                // 2.2074: 0x4841bc (google) / 0x483f0c (amazon)
                // 2.2081: 0x49725c
                auto func = dlsym(RTLD_DEFAULT, "_ZN15GJBaseGameLayer6updateEf");
                addr = sinaps::find<"EE ? ? ? EE ? ? ^ F7 EE ? 7B 17 EE 90 0A">(static_cast<const uint8_t*>(func), 0x500);
                if (addr != sinaps::not_found) {
                    addr += reinterpret_cast<intptr_t>(func) - reinterpret_cast<intptr_t>(base); // we need offset from the base
                    bytes = Builder(addr)
                        .mov(Register::r1, std::bit_cast<uint32_t>(&g_expectedTicks))
                        .ldr_t(Register::r0, Register::r1)
                        .nop_t()
                        .build();
                }
            }
            #elif defined(GEODE_IS_IOS) || defined(GEODE_IS_ARM_MAC) // lucky me, they're virtually the same
            {
                using namespace assembler::arm64;
                // 2.2074: 0x200C30 (iOS) / 0x119454 (macOS)
                static_assert(false, "TODO: fix for 2.2081");
                addr = sinaps::find<"00 19 20 1E 02 10 22 1E">(base, baseSize);
                if (addr != sinaps::not_found) {
                    bytes = Builder(addr)
                        .mov(Register::x9, std::bit_cast<uint64_t>(&g_expectedTicks))
                        .ldr(FloatRegister::s0, Register::x9)
                        .pad_nops(20) // we need to replace 20 bytes, but mov can take 3-4 instructions depending on address
                        .build();
                }
            }
            #elif defined(GEODE_IS_INTEL_MAC)
            {
                using namespace assembler::x86_64;
                // 2.2074: 0x14233e
                // 2.2081: 0x1516c4
                addr = sinaps::find<"0F 28 ? F3 0F 5D 83 ? ? ? ? F3 0F ? ? F2 0F 10">(base, baseSize);
                if (addr != sinaps::not_found) {
                    bytes = Builder(addr)
                        .movabs(Register64::rax, std::bit_cast<uint64_t>(&g_expectedTicks))
                        .movss(XmmRegister::xmm0, Register64::rax)
                        .jmp(addr + 0x49, true)
                        .build();
                }
            }
            #endif

            if (addr == sinaps::not_found || bytes.empty()) {
                geode::log::error("TPS Bypass: Failed to find patch address or bytes");
                return forceDisable();
            }

            geode::Patch* patch = nullptr;
            if (auto res = geode::Mod::get()->patch(reinterpret_cast<void*>(addr + base), bytes)) {
                patch = res.unwrap();
                geode::log::info("TPS Bypass: Patch enabled at offset 0x{:X}", addr);
            } else {
                geode::log::error("TPS Bypass: Failed to patch GJBaseGameLayer::update: {}", res.unwrapErr());
            }

            // patch toggler
            if (!patch) {
                geode::log::error("TPS Bypass: Failed to patch GJBaseGameLayer::update");
                return forceDisable();
            }

            // toggle the patch if enabled
            config::addDelegate("global.tpsbypass.toggle", [patch] {
                (void) patch->toggle(config::get<bool>("global.tpsbypass.toggle", false));
            });

            // set the initial state of the patch
            (void) patch->toggle(config::get<bool>("global.tpsbypass.toggle", false));

            // on macOS, we also have to patch instructions in GJBaseGameLayer::getModifiedDelta because it's inlined
            #ifdef REQUIRE_MODIFIED_DELTA_PATCH
            auto res = setupModifiedDeltaPatches();
            if (!res) {
                geode::log::error("TPSBypass: {}", res.unwrapErr());
                return forceDisable();
            }
            #endif

            #ifdef GEODE_IS_IOS
            } // closes the `if (geode::Loader::get()->isPatchless()) { ... } else {` block
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
        [[nodiscard]] static std::vector<uint8_t> TPStoBytes() {
            return geode::toBytes<double>(1.0 / config::get<"global.tpsbypass", double>(240.0));
        }

        geode::Result<> setupModifiedDeltaPatches() {
            auto base = reinterpret_cast<uint8_t*>(geode::base::get());
            auto moduleSize = utils::getBaseSize();

            using namespace sinaps::mask;

            #ifdef GEODE_IS_INTEL_MAC
            // on x86, we need to replace two values that are used in GJBaseGameLayer::getModifiedDelta
            // pretty simple, just find original values and replace them with raw bytes
            // UPD 2.2081: float value is no longer there, so only patch double value

            auto addr = sinaps::find<qword<0x3F71111111111111>>(base, moduleSize);
            if (addr == sinaps::not_found) {
                return geode::Err("failed to find getModifiedDelta patch address");
            }

            auto patchRes = geode::Mod::get()->patch(addr + base, TPStoBytes());
            if (!patchRes) return geode::Err(fmt::format("failed to patch address: {}", patchRes.unwrapErr()));
            auto patch = patchRes.unwrap();
            (void) patch->disable();

            // update bytes on value change
            config::addDelegate("global.tpsbypass", [patch]() { (void) patch->updateBytes(TPStoBytes()); });
            auto setState = [patch](bool state) { (void) patch->toggle(state); };

            #elif defined(GEODE_IS_ARM_MAC)
            // on ARM, the value is stored as immediate, with some encoding:
            // 0x0000000100122C54:  E9 E3 00 B2    orr  x9, xzr, #0x1111111111111111
            // 0x0000000100122C58:  29 EE E7 F2    movk x9, #0x3f71, lsl #48
            //
            // this makes it kinda hard to inject any value we want, so instead we're going to store it
            // in a GJBaseGameLayer member, load it with one instruction and then restore it in the closest hook
            //
            // i'm using `GJGameLoadingLayer* m_loadingLayer`, because it's not used anywhere apart from open/exit sequence,
            // and should be fine for our use case. since it's a pointer, we can store double right in it
            // (good luck whoever's going to dereference it lol)

            auto addr = sinaps::find<"E9 E3 00 B2 29 EE E7 F2">(base, moduleSize);
            if (addr == sinaps::not_found) {
                return geode::Err("failed to find getModifiedDelta patch address");
            }

            using namespace assembler::arm64;
            auto bytes = Builder(addr)
                .ldr(Register::x9, Register::x19, offsetof(GJBaseGameLayer, m_loadingLayer))
                .nop()
                .build();

            auto patchRes = geode::Mod::get()->patch(addr + base, std::move(bytes));
            if (!patchRes) return geode::Err(fmt::format("failed to patch address: {}", patchRes.unwrapErr()));
            auto patch = patchRes.unwrap();
            (void) patch->disable();

            auto setState = [patch](bool state) { (void) patch->toggle(state); };
            #endif

            // update patches on enable/disable
            config::addDelegate("global.tpsbypass.toggle", [setState] {
                setState(config::get<bool>("global.tpsbypass.toggle", false));
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
            auto tps = config::get<"global.tpsbypass", double>(240.f);
            return toggle && tps != 240.f;
        }
    };

    REGISTER_HACK(TPSBypass)

    class $modify(TPSBypassGJBGLHook, GJBaseGameLayer) {
        static void onModify(auto& self) {
            hack::safeHooksAll(self.m_hooks);
            if (!geode::Loader::get()->isPatchless()) {
                hack::setupTogglesAll("global.tpsbypass.toggle", self.m_hooks);
            }
        }

        struct Fields {
            double m_extraDelta = 0.0;
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

        #ifndef REQUIRE_MODIFIED_DELTA_PATCH
        float getModifiedDelta(float dt) {
            #ifdef GEODE_IS_IOS
            return getCustomDelta(dt, utils::getTPS());
            #else
            return getCustomDelta(dt, config::get<"global.tpsbypass", double>(240.f));
            #endif
        }
        #endif

        void update(float dt) override {
            auto fields = m_fields.self();
            fields->m_extraDelta += dt;

            // calculate number of steps based on the new TPS
            auto timeWarp = std::min(m_gameState.m_timeWarp, 1.f);

            #ifdef GEODE_IS_IOS
            // on iOS, since the hook is always active, we also check if tpsbypass is enabled
            auto newTPS = utils::getTPS() / timeWarp;
            #else
            auto newTPS = config::get<"global.tpsbypass", double>(240.f) / timeWarp;
            #endif

            auto spt = 1.0 / newTPS;
            auto steps = std::floor(fields->m_extraDelta / spt);
            auto totalDelta = steps * spt;
            fields->m_extraDelta -= totalDelta;
            expectedTicks() = steps;

            #ifdef GEODE_IS_ARM_MAC
            // we're a bit silly here:
            auto originalLoadingLayer = m_loadingLayer;
            this->m_loadingLayer = std::bit_cast<GJGameLoadingLayer*>(1.0 / utils::getTPS());
            #endif

            GJBaseGameLayer::update(totalDelta);

            #ifdef GEODE_IS_ARM_MAC
            this->m_loadingLayer = originalLoadingLayer;
            #endif
        }
    };

    inline TPSBypassGJBGLHook::Fields* getFields(GJBaseGameLayer* self) {
        return reinterpret_cast<TPSBypassGJBGLHook*>(self)->m_fields.self();
    }

    class $modify(TPSBypassPLHook, PlayLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("global.tpsbypass.toggle")

        // we would like to fix the percentage calculation, which uses constant 240 TPS to determine the progress
        int calculationFix() {
            auto timestamp = m_level->m_timestamp;
            auto currentProgress = m_gameState.m_currentProgress;
            // this is only an issue for 2.2+ levels (with TPS greater than 240)
            if (timestamp > 0 && config::get<"global.tpsbypass", double>(240.f) != 240.f) {
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
            auto oldTimestamp = m_gameState.m_commandIndex;
            if (config::get<"global.tpsbypass", double>(240.f) != 240.f) {
                auto ticks = static_cast<uint32_t>(std::round(m_gameState.m_levelTime * 240));
                m_gameState.m_commandIndex = ticks;
            }
            PlayLayer::levelComplete();
            m_gameState.m_commandIndex = oldTimestamp;
        }
    };
}
