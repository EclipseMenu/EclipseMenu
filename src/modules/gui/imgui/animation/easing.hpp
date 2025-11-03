#pragma once

#include <string>
#include <array>

namespace eclipse::gui::animation {
    /// @brief Function pointer for an easing function.
    /// @param t The time.
    /// @return The value at time `t`.
    using EasingFunction = double (*)(double);

    /// @brief Easing functions.
    enum class Easing {
        Linear = 0,
        Sine = 1,
        Quadratic = 2,
        Cubic = 3,
        Quartic = 4,
        Quintic = 5,
        Exponential = 6,
        Circular = 7,
        Backwards = 8,
        Elastic = 9,
        Bounce = 10,
    };

    /// @brief Easing names.
    constexpr std::array<const char*, 11> EASING_NAMES = {
        "Linear",
        "Sine",
        "Quadratic",
        "Cubic",
        "Quartic",
        "Quintic",
        "Exponential",
        "Circular",
        "Backwards",
        "Elastic",
        "Bounce",
    };

    constexpr size_t EASING_COUNT = EASING_NAMES.size();

    /// @brief Easing modes.
    enum class EasingMode {
        EaseIn = 0,
        EaseOut = 1,
        EaseInOut = 2,
    };

    /// @brief Easing mode names.
    constexpr std::array<const char*, 3> EASING_MODE_NAMES = {
        "EaseIn",
        "EaseOut",
        "EaseInOut",
    };

    /* == Easing functions == */
    /*  https://easings.net/  */
    namespace easing {
        /// @brief Linear easing
        /// @note x = t
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double linear(double t);

        /* == Sine == */

        /// @brief EaseIn Sine easing
        /// @note x = 1 - cos((t * PI) / 2)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInSine(double t);

        /// @brief EaseOut Sine easing
        /// @note x = sin((t * PI) / 2)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeOutSine(double t);

        /// @brief EaseInOut Sine easing
        /// @note x = -(cos(PI * t) - 1) / 2
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInOutSine(double t);

        /* == Quadratic == */

        /// @brief EaseIn Quadratic easing (x = t * t)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInQuad(double t);

        /// @brief EaseOut Quadratic easing
        /// @note x = 1 - (1 - t) * (1 - t)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeOutQuad(double t);

        /// @brief EaseInOut Quadratic easing
        /// @note x = t < 0.5 ? (2 * t * t) : (1 - pow(-2 * t + 2, 2) / 2)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInOutQuad(double t);

        /* == Cubic == */

        /// @brief EaseIn Cubic easing
        /// @note x = t * t * t
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInCubic(double t);

        /// @brief EaseOut Cubic easing
        /// @note x = 1 - pow(1 - t, 3)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeOutCubic(double t);

        /// @brief EaseInOut Cubic easing
        /// @note x = t < 0.5 ? (4 * t * t * t) : (1 - pow(-2 * t + 2, 3) / 2)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInOutCubic(double t);

        /* == Quartic == */

        /// @brief EaseIn Quartic easing
        /// @note x = t * t * t * t
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInQuart(double t);

        /// @brief EaseOut Quartic easing
        /// @note x = 1 - pow(1 - t, 4)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeOutQuart(double t);

        /// @brief EaseInOut Quartic easing
        /// @note x = t < 0.5 ? (8 * t * t * t * t) : (1 - pow(-2 * t + 2, 4) / 2)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInOutQuart(double t);

        /* == Quintic == */

        /// @brief EaseIn Quintic easing
        /// @note x = t * t * t * t * t
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInQuint(double t);

        /// @brief EaseOut Quintic easing
        /// @note x = 1 - pow(1 - t, 5)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeOutQuint(double t);

        /// @brief EaseInOut Quintic easing
        /// @note x = t < 0.5 ? (16 * t * t * t * t * t) : (1 - pow(-2 * t + 2, 5) / 2)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInOutQuint(double t);

        /* == Exponential == */

        /// @brief EaseIn Exponential easing
        /// @note x = t == 0 ? 0 : pow(2, 10 * t - 10)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInExpo(double t);

        /// @brief EaseOut Exponential easing
        /// @note x = t == 1 ? 1 : 1 - pow(2, -10 * t)
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeOutExpo(double t);

        /// @brief EaseInOut Exponential easing
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInOutExpo(double t);

        /* == Circular == */

        /// @brief EaseIn Circular easing
        /// @note x = 1 - sqrt(1 - pow(t, 2))
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInCirc(double t);

        /// @brief EaseIn Circular easing
        /// @note x = sqrt(1 - pow(t - 1, 2))
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeOutCirc(double t);

        /// @brief EaseIn Circular easing
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInOutCirc(double t);

        /* == Backwards == */

        /// @brief EaseIn Backwards easing
        /// @note x =
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInBack(double t);

        /// @brief EaseOut Backwards easing
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeOutBack(double t);

        /// @brief EaseInOut Backwards easing
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInOutBack(double t);

        /* == Elastic == */

        /// @brief EaseIn Elastic easing
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInElastic(double t);

        /// @brief EaseOut Elastic easing
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeOutElastic(double t);

        /// @brief EaseInOut Elastic easing
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInOutElastic(double t);

        /* == Bounce == */

        /// @brief EaseIn Bounce easing
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInBounce(double t);

        /// @brief EaseOut Bounce easing
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeOutBounce(double t);

        /// @brief EaseInOut Bounce easing
        /// @param t Time from 0 to 1
        /// @return `x` value at time `t`
        double easeInOutBounce(double t);
    }

    /// @brief Easing functions pointers.
    /// Rows are `Easing` types and columns are `EasingMode`.
    /// @example EASING_FUNCTIONS[Easing::Sine][EasingMode::EaseOut] == easing::easeOutSine;
    constexpr EasingFunction EASING_FUNCTIONS[][3] = {
        {easing::linear,        easing::linear,         easing::linear},
        {easing::easeInSine,    easing::easeOutSine,    easing::easeInOutSine},
        {easing::easeInQuad,    easing::easeOutQuad,    easing::easeInOutQuad},
        {easing::easeInCubic,   easing::easeOutCubic,   easing::easeInOutCubic},
        {easing::easeInQuart,   easing::easeOutQuart,   easing::easeInOutQuart},
        {easing::easeInQuint,   easing::easeOutQuint,   easing::easeInOutQuint},
        {easing::easeInExpo,    easing::easeOutExpo,    easing::easeInOutExpo},
        {easing::easeInCirc,    easing::easeOutCirc,    easing::easeInOutCirc},
        {easing::easeInBack,    easing::easeOutBack,    easing::easeInOutBack},
        {easing::easeInElastic, easing::easeOutElastic, easing::easeInOutElastic},
        {easing::easeInBounce,  easing::easeOutBounce,  easing::easeInOutBounce},
    };

    /// @brief Gets the easing function for the given easing.
    /// @param easing The easing.
    /// @return The easing function.
    EasingFunction getEasingFunction(Easing easing, EasingMode mode);
}