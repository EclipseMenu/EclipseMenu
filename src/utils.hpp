#pragma once

#include <random>

namespace eclipse::utils {

    /// @brief Returns a random device.
    /// @return Random device.
    std::random_device& getRng();

    /// @brief Generates a random number between min and max.
    /// @tparam T Type of the number.
    /// @param min Minimum value.
    /// @param max Maximum value.
    /// @return Random number between min and max.
    template<typename T>
    inline T random(T min, T max) {
        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(getRng());
        } else if constexpr (std::is_floating_point_v<T>) {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(getRng());
        }
        return T();
    }

    /// @brief Generates a random number between 0 and max.
    /// @tparam T Type of the number.
    /// @param max Maximum value.
    /// @return Random number between 0 and max.
    template<typename T>
    inline T random(T max) {
        return random<T>(0, max);
    }

}