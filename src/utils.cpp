#include "utils.hpp"

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

    bool hasOpenGLExtension(const std::string& extension) {
        auto extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
        if (!extensions) return false;
        return std::string(extensions).find(extension) != std::string::npos;
    }

    bool shouldUseLegacyDraw() {
        static bool hasVAO = hasOpenGLExtension("GL_ARB_vertex_array_object");
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
        else if (minutes > 0)
            return fmt::format("{}:{:02d}.{:03d}", minutes, seconds, millis);
        else
            return fmt::format("{}.{:03d}", seconds, millis);
    }

    float getActualProgress(PlayLayer* playLayer) {
        float percent;
        if (playLayer->m_level->m_timestamp > 0) {
            percent = static_cast<float>(playLayer->m_gameState.m_levelTime * 240.f) / playLayer->m_level->m_timestamp * 100.f;
        } else {
            percent = reinterpret_cast<cocos2d::CCNode*>(playLayer->m_player1)->getPositionX() / playLayer->m_levelLength * 100.f;
        }
        return std::clamp(percent, 0.f, 100.f);
    }

}