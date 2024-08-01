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

}