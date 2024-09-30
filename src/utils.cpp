#include "utils.hpp"

#include <fmt/format.h>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/PlatformToolbox.hpp>
#include <Geode/loader/Mod.hpp>

namespace eclipse::utils {

    std::random_device& getRng() {
        static std::random_device rng;
        return rng;
    }

    std::string getClock(bool useTwelveHours) {
        const char* format = useTwelveHours ? "%I:%M:%S %p" : "%H:%M:%S";

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto tm = std::localtime(&time);

        std::stringstream ss;
        ss << std::put_time(tm, format);
        return ss.str();
    }

    bool hasOpenGLExtension(std::string_view extension) {
        static auto extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
        if (!extensions) return false;
        static std::string_view extensionsString = extensions;
        return extensionsString.find(extension) != std::string_view::npos;
    }

    bool shouldUseLegacyDraw() {
#ifdef GEODE_IS_MACOS
        static bool hasVAO = hasOpenGLExtension("GL_APPLE_vertex_array_object");
#else
        static bool hasVAO = hasOpenGLExtension("GL_ARB_vertex_array_object");
#endif
        auto useLegacy = geode::Mod::get()->getSettingValue<bool>("legacy-render");
        return !hasVAO || useLegacy;
    }

    std::string formatTime(double time) {
        auto hours = static_cast<int>(time / 3600);
        auto minutes = static_cast<int>(time / 60);
        auto seconds = static_cast<int>(time) % 60;
        auto millis = static_cast<int>(time * 1000) % 1000;

        if (hours > 0)
            return fmt::format("{}:{:02d}:{:02d}.{:03d}", hours, minutes, seconds, millis);
        if (minutes > 0)
            return fmt::format("{}:{:02d}.{:03d}", minutes, seconds, millis);
        return fmt::format("{}.{:03d}", seconds, millis);
    }

    float getActualProgress(GJBaseGameLayer* game) {
        float percent;
        if (game->m_level->m_timestamp > 0) {
            percent = static_cast<float>(game->m_gameState.m_levelTime * 240.f) / game->m_level->m_timestamp * 100.f;
        } else {
            percent = reinterpret_cast<cocos2d::CCNode*>(game->m_player1)->getPositionX() / game->m_levelLength * 100.f;
        }
        return std::clamp(percent, 0.f, 100.f);
    }

    void updateCursorState(bool visible) {
        bool canShowInLevel = true;
        if (auto* playLayer = PlayLayer::get()) {
            canShowInLevel = playLayer->m_hasCompletedLevel ||
                             playLayer->m_isPaused ||
                             GameManager::sharedState()->getGameVariable("0024");
        }
        if (visible || canShowInLevel)
            PlatformToolbox::showCursor();
        else
            PlatformToolbox::hideCursor();
    }

}
