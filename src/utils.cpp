#include "utils.hpp"

#include <fmt/format.h>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/PlatformToolbox.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/loader/Mod.hpp>

#include <modules/config/config.hpp>
#include <modules/gui/color.hpp>
#include <modules/utils/SingletonCache.hpp>

// headers for getBaseSize()
#ifdef GEODE_IS_WINDOWS
    #include <psapi.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <mach-o/getsect.h>
    #include <mach-o/loader.h>
#elif defined(GEODE_IS_ANDROID)
    #include <unistd.h>
    #include <sys/mman.h>
    #include <elf.h>
    #include <fcntl.h>
    #include <link.h>
    #include <cstring>
#endif

namespace eclipse::utils {
    std::random_device& getRng() {
        static std::random_device rng;
        return rng;
    }

    std::string getClock(bool useTwelveHours) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto tm = fmt::localtime(time);
        return useTwelveHours ? fmt::format("{:%I:%M:%S %p}", tm) : fmt::format("{:%H:%M:%S}", tm);
    }

    bool hasOpenGLExtension(std::string_view extension) {
        static auto extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
        if (!extensions) return false;
        static std::string_view extensionsString = extensions;
        return extensionsString.find(extension) != std::string_view::npos;
    }

    bool shouldUseLegacyDraw() {
        #ifdef __APPLE__
        static bool hasVAO = hasOpenGLExtension("GL_APPLE_vertex_array_object");
        #else
        static bool hasVAO = hasOpenGLExtension("GL_ARB_vertex_array_object");
        #endif
        auto useLegacy = geode::Mod::get()->getSettingValue<bool>("legacy-render");
        return !hasVAO || useLegacy;
    }

    std::string formatTime(double time, bool showMillis) {
        auto hours = static_cast<int>(time / 3600);
        auto minutes = static_cast<int>(time / 60) % 60;
        auto seconds = static_cast<int>(time) % 60;
        auto millis = static_cast<int>(time * 1000) % 1000;

        if (showMillis) {
            if (hours > 0) return fmt::format("{}:{:02d}:{:02d}.{:03d}", hours, minutes, seconds, millis);
            if (minutes > 0) return fmt::format("{}:{:02d}.{:03d}", minutes, seconds, millis);
            return fmt::format("{}.{:03d}", seconds, millis);
        }

        if (hours > 0) return fmt::format("{}:{:02d}:{:02d}", hours, minutes, seconds);
        if (minutes > 0) return fmt::format("{}:{:02d}", minutes, seconds);
        return fmt::format("{}", seconds);
    }

    float getActualProgress(GJBaseGameLayer* game) {
        float percent;
        if (game->m_level->m_timestamp > 0) {
            percent = static_cast<float>(game->m_gameState.m_levelTime * 240.f) / game->m_level->m_timestamp * 100.f;
        } else {
            percent = game->m_player1->getPositionX() / game->m_levelLength * 100.f;
        }
        return std::clamp(percent, 0.f, 100.f);
    }

    void updateCursorState(bool visible) {
        bool canShowInLevel = true;
        if (auto* playLayer = utils::get<PlayLayer>()) {
            canShowInLevel = playLayer->m_hasCompletedLevel ||
                             playLayer->m_isPaused ||
                             utils::get<GameManager>()->getGameVariable("0024");
        }
        if (visible || canShowInLevel)
            PlatformToolbox::showCursor();
        else
            PlatformToolbox::hideCursor();
    }

    const char* getMonthName(int month) {
        constexpr std::array months = {
            "January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December"
        };
        return months.at(month);
    }

    template <typename D>
    time_t getTimestamp() {
        if constexpr (std::is_same_v<D, std::chrono::seconds>) {
            return std::chrono::duration_cast<D>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count();
        } else {
            return std::chrono::duration_cast<D>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count();
        }
    }

    template time_t getTimestamp<std::chrono::milliseconds>();
    template time_t getTimestamp<std::chrono::seconds>();

    gui::Color getRainbowColor(float speed, float saturation, float value, float offset) {
        time_t ms = getTimestamp();
        float h = std::fmod(ms * speed + offset, 360.0f);
        return gui::Color::fromHSV(h, saturation, value);
    }

    PlayerMode getGameMode(PlayerObject* player) {
        if (!player) {
            auto gm = utils::get<GameManager>();
            switch (gm->m_playerIconType) {
                case IconType::Cube: default: return PlayerMode::Cube;
                case IconType::Ship:
                case IconType::Jetpack: return PlayerMode::Ship;
                case IconType::Ball: return PlayerMode::Ball;
                case IconType::Ufo: return PlayerMode::UFO;
                case IconType::Wave: return PlayerMode::Wave;
                case IconType::Robot: return PlayerMode::Robot;
                case IconType::Spider: return PlayerMode::Spider;
                case IconType::Swing: return PlayerMode::Swing;
            }
        }

        if (player->m_isShip) return PlayerMode::Ship;
        if (player->m_isBall) return PlayerMode::Ball;
        if (player->m_isBird) return PlayerMode::UFO;
        if (player->m_isDart) return PlayerMode::Wave;
        if (player->m_isRobot) return PlayerMode::Robot;
        if (player->m_isSpider) return PlayerMode::Spider;
        if (player->m_isSwing) return PlayerMode::Swing;
        return PlayerMode::Cube;
    }

    const char* gameModeName(PlayerMode mode) {
        switch (mode) {
            case PlayerMode::Cube: return "Cube";
            case PlayerMode::Ship: return "Ship";
            case PlayerMode::Ball: return "Ball";
            case PlayerMode::UFO: return "UFO";
            case PlayerMode::Wave: return "Wave";
            case PlayerMode::Robot: return "Robot";
            case PlayerMode::Spider: return "Spider";
            case PlayerMode::Swing: return "Swing";
        }
        return "Unknown";
    }

    int getPlayerIcon(PlayerMode mode) {
        auto gm = utils::get<GameManager>();
        switch (mode) {
            case PlayerMode::Cube: return gm->m_playerFrame;
            case PlayerMode::Ship: return gm->m_playerShip;
            case PlayerMode::Ball: return gm->m_playerBall;
            case PlayerMode::UFO: return gm->m_playerBird;
            case PlayerMode::Wave: return gm->m_playerDart;
            case PlayerMode::Robot: return gm->m_playerRobot;
            case PlayerMode::Spider: return gm->m_playerSpider;
            case PlayerMode::Swing: return gm->m_playerSwing;
        }
        return 1;
    }

    float getTPS() {
        return config::get<"global.tpsbypass.toggle", bool>(false)
            ? config::get<"global.tpsbypass", float>(240.f) : 240.f;
    }

    cocos2d::CCMenu* getEclipseUILayer() {
        auto uiLayer = utils::get<UILayer>();
        if (!uiLayer) return nullptr;

        if (auto menu = uiLayer->getChildByID("eclipse-ui"_spr))
            return static_cast<cocos2d::CCMenu*>(menu);

        auto menu = cocos2d::CCMenu::create();
        menu->setID("eclipse-ui"_spr);
        uiLayer->addChild(menu, 1000);
        menu->setPosition({0, 0});
        return menu;
    }

    bool matchesStringFuzzy(std::string_view haystack, std::string_view needle) {
        auto it = std::ranges::search(
            haystack, needle, [](char ch1, char ch2) {
                return std::toupper(ch1) == std::toupper(ch2);
            }
        ).begin();

        return (it != haystack.end());
    }

    size_t getBaseSize() {
        static size_t baseSize = []() -> size_t {
            #ifdef GEODE_IS_WINDOWS

            auto handle = GetModuleHandle(nullptr);
            if (!handle) return 0;

            MODULEINFO info;
            if (!GetModuleInformation(GetCurrentProcess(), handle, &info, sizeof(info)))
                return 0;

            return info.SizeOfImage;

            #elif defined(__APPLE__)

            // i have no idea how to do this on macOS,
            // so for now just hardcode some arbitrary value within the range of the base size :fire:
            return GEODE_INTEL_MAC(0x980000)
                   GEODE_ARM_MAC(0x8B0000)
                   GEODE_IOS(0x8C0000);

            #elif defined(GEODE_IS_ANDROID)

            // this can apparently crash for some people
            // struct dl_phdr_info info;
            // if (dl_iterate_phdr([](struct dl_phdr_info *info, size_t, void *data) {
            //     if (info->dlpi_name[0] == '\0') { // Main executable
            //         *reinterpret_cast<size_t*>(data) = info->dlpi_phnum > 0
            //             ? info->dlpi_phdr[info->dlpi_phnum - 1].p_vaddr + info->dlpi_phdr[info->dlpi_phnum - 1].p_memsz
            //             : 0;
            //         return 1;
            //     }
            //     return 0;
            // }, &info) == 1) {
            //     return info.dlpi_phnum > 0
            //         ? info.dlpi_phdr[info.dlpi_phnum - 1].p_vaddr + info.dlpi_phdr[info.dlpi_phnum - 1].p_memsz
            //         : 0;
            // }
            return 0;

            #else

            static_assert(false, "getBaseSize() is not implemented for this platform");

            #endif
        }();
        return baseSize;
    }
}
