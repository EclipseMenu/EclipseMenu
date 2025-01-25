#pragma once

#include <random>
#include <modules/utils/SingletonCache.hpp>

enum class PlayerMode {
    Cube, Ship, Ball,
    UFO, Wave, Robot,
    Spider, Swing
};

namespace eclipse::gui {
    class Color;
}

namespace eclipse::utils {
    /// @brief Returns a random device.
    /// @return Random device.
    std::random_device& getRng();

    /// @brief Generates a random number between min and max.
    /// @tparam T Type of the number.
    /// @param min Minimum value.
    /// @param max Maximum value.
    /// @return Random number between min and max.
    template <typename T>
    T random(T min, T max) {
        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(getRng());
        } else if constexpr (std::is_floating_point_v<T>) {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(getRng());
        } else {
            static_assert(!sizeof(T*), "T must be an integral or floating point type.");
        }
    }

    /// @brief Generates a random number between 0 and max.
    /// @tparam T Type of the number.
    /// @param max Maximum value.
    /// @return Random number between 0 and max.
    template <typename T>
    T random(T max) {
        return random<T>(0, max);
    }

    /// @brief Allows to access a member of a struct by offset.
    /// @tparam T Type of the member.
    /// @param ptr Pointer to the struct.
    /// @param offset Offset of the member.
    /// @return Reference to the member.
    /// @example auto& member = utils::memberByOffset\<int>(ptr, offsetof(Struct, member));\n
    /// geode::log::info("Member: {}", member);\n
    /// member = 42;
    /// @warning Use with caution, for debugging purposes only.
    template <typename T>
    [[deprecated("Don't use MBO!")]] constexpr T& memberByOffset(void* ptr, size_t offset) {
        return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) + offset);
    }

    /// @brief Gets the current time as a string.
    /// @param useTwelveHours Whether to use twelve hours format.
    /// @return Current time as a string (e.g. "16:34:56", "12:34 PM").
    std::string getClock(bool useTwelveHours = false);

    /// @brief Get whether current OpenGL context has an extension.
    /// @param extension Extension to check.
    /// @return Whether current OpenGL context has the extension.
    bool hasOpenGLExtension(std::string_view extension);

    /// @brief Get whether to use legacy rendering functions for OpenGL.
    /// @return Whether to use legacy rendering functions.
    bool shouldUseLegacyDraw();

    /// @brief Format time in seconds to a string.
    /// @param time Time in seconds.
    /// @return Formatted time string (e.g. "1:23:45.678").
    std::string formatTime(double time);

    /// @brief Bugfixed version of getCurrentPercent.
    /// @param game GJBaseGameLayer to get the progress from.
    /// @return Actual progress of the level.
    float getActualProgress(class GJBaseGameLayer* game);

    /// @brief Make the cursor visible/hidden under certain conditions
    void updateCursorState(bool visible);

    /// @brief Get month name from its number. (0-11)
    const char* getMonthName(int month);

    using millis = std::chrono::milliseconds;
    using seconds = std::chrono::seconds;

    /// @brief Get the current timestamp.
    template <typename D = millis>
    time_t getTimestamp();

    /// @brief Get a rainbow color for specific parameters
    /// @param speed The speed of the rainbow effect
    /// @param saturation The S (saturation) component of the HSV color
    /// @param value The V (value) component of the HSV color
    /// @param offset The offset in milliseconds for the rainbow effect
    gui::Color getRainbowColor(float speed, float saturation, float value, float offset = 0.f);

    /// @brief Get the current player game mode. If player is nullptr, returns selected profile icon.
    PlayerMode getGameMode(class PlayerObject* player);

    /// @brief Get the name of a game mode.
    const char* gameModeName(PlayerMode mode);

    /// @brief Get icon frame for a specific mode.
    int getPlayerIcon(PlayerMode mode);

    /// @brief Get current chosen TPS.
    float getTPS();

    /// @brief Get custom CCMenu created in UILayer.
    class cocos2d::CCMenu* getEclipseUILayer();

    /// @brief Checks if a string matches another one (case-insensitive)
    bool matchesStringFuzzy(std::string_view haystack, std::string_view needle);

    /// @brief Returns a specific page of items from a given Vector.
    /// @tparam T Type of item.
    /// @param array Vector to paginate.
    /// @param size Amount of items per page.
    /// @param page Page number to retrieve.
    /// @return A paginated span containing the items of the specified page.
    template <typename T>
    std::span<T> paginate(const std::vector<T>& array, int size, int page) {
        if (size <= 0) return {};
        int startIndex = page * size;
        int endIndex = std::min(startIndex + size, static_cast<int>(array.size()));
        if (startIndex >= array.size()) return {};
        // result.insert(result.end(), array.begin() + startIndex, array.begin() + endIndex);
        return {const_cast<T*>(array.data()) + startIndex, static_cast<size_t>(endIndex - startIndex)};
    }

    /// @brief Returns a "gradual" page of items from a given Vector.
    /// @tparam T Type of item.
    /// @param array Vector to paginate.
    /// @param size Amount of items per "gradual" page.
    /// @param page "Gradual" page number to retrieve.
    /// @return A paginated span containing the items of the specified "gradual" page.
    template <typename T>
    std::span<T> gradualPaginate(const std::vector<T>& array, int size, int page) {
        if (size <= 0) return {};
        if (page < 0) return {};
        int startIndex = page;
        int endIndex = std::min(startIndex + size, static_cast<int>(array.size()));
        if (startIndex >= array.size()) return {};
        // result.insert(result.end(), array.begin() + startIndex, array.begin() + endIndex);
        return {const_cast<T*>(array.data()) + startIndex, static_cast<size_t>(endIndex - startIndex)};
    }
}
