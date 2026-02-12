#include "easing.hpp"

#include <cmath>
#include <numbers>

namespace eclipse::gui::animation {
    EasingFunction getEasingFunction(Easing easing, EasingMode mode) {
        auto easingIndex = static_cast<size_t>(easing);
        auto modeIndex = static_cast<size_t>(mode);

        // Check if not out-of-bounds
        if (easingIndex > std::size(EASING_FUNCTIONS) - 1 || modeIndex > std::size(EASING_FUNCTIONS[0]) - 1)
            return easing::linear;

        return EASING_FUNCTIONS[easingIndex][modeIndex];
    }

    namespace easing {
        double linear(double t) {
            return t;
        }

        /* == Sine == */

        double easeInSine(double t) {
            return 1.0 - cos(t * std::numbers::pi / 2.0);
        }

        double easeOutSine(double t) {
            return sin(t * std::numbers::pi / 2.0);
        }

        double easeInOutSine(double t) {
            return -(cos(std::numbers::pi * t) - 1.0) / 2.0;
        }

        /* == Quadratic == */

        double easeInQuad(double t) {
            return t * t;
        }

        double easeOutQuad(double t) {
            return 1.0 - (1.0 - t) * (1.0 - t);
        }

        double easeInOutQuad(double t) {
            return t < 0.5 ? (2.0 * t * t) : (1.0 - pow(-2.0 * t + 2.0, 2) / 2.0);
        }

        /* == Cubic == */

        double easeInCubic(double t) {
            return t * t * t;
        }

        double easeOutCubic(double t) {
            return 1.0 - pow(1.0 - t, 3);
        }

        double easeInOutCubic(double t) {
            return t < 0.5 ? (4.0 * t * t * t) : (1.0 - pow(-2.0 * t + 2.0, 3) / 2.0);
        }

        /* == Quartic == */

        double easeInQuart(double t) {
            return t * t * t * t;
        }

        double easeOutQuart(double t) {
            return 1.0 - pow(1.0 - t, 4);
        }

        double easeInOutQuart(double t) {
            return t < 0.5 ? (8.0 * t * t * t * t) : (1.0 - pow(-2.0 * t + 2.0, 4) / 2.0);
        }

        /* == Quintic == */

        double easeInQuint(double t) {
            return t * t * t * t * t;
        }

        double easeOutQuint(double t) {
            return 1.0 - pow(1.0 - t, 5);
        }

        double easeInOutQuint(double t) {
            return t < 0.5 ? (16.0 * t * t * t * t * t) : (1.0 - pow(-2.0 * t + 2.0, 5) / 2.0);
        }

        /* == Exponential == */

        double easeInExpo(double t) {
            return t == 0.0 ? t : pow(2.0, 10.0 * t - 10.0);
        }

        double easeOutExpo(double t) {
            return t == 1.0 ? t : 1.0 - pow(2.0, -10.0 * t);
        }

        double easeInOutExpo(double t) {
            if (t == 0.0 || t == 1.0)
                return t;

            if (t < 0.5)
                return pow(2.0, 20.0 * t - 10.0) / 2.0;

            return (2.0 - pow(2.0, -20.0 * t + 10.0)) / 2.0;
        }

        /* == Circular == */

        double easeInCirc(double t) {
            return 1.0 - sqrt(1.0 - t * t);
        }

        double easeOutCirc(double t) {
            return sqrt(1.0 - pow((t - 1.0), 2));
        }

        double easeInOutCirc(double t) {
            if (t < 0.5)
                return (1.0 - sqrt(1.0 - pow(2.0 * t, 2))) / 2.0;

            return (sqrt(1.0 - pow(-2.0 * t + 2.0, 2)) + 1.0) / 2.0;
        }

        /* == Backwards == */

        double easeInBack(double t) {
            return 2.70158 * t * t * t - 1.70158 * t * t;
        }

        double easeOutBack(double t) {
            return 1.0 + 2.70158 * pow(t - 1.0, 3) + 1.70158 * pow(t - 1.0, 2);
        }

        double easeInOutBack(double t) {
            if (t < 0.5)
                return (pow(2.0 * t, 2) * (7.189819 * t - 2.5949095)) / 2.0;

            return (pow(2.0 * t - 2.0, 2) * (3.5949095 * (t * 2.0 - 2.0) + 2.5949095) + 2.0) / 2.0;
        }

        /* == Elastic == */

        double easeInElastic(double t) {
            if (t == 0.0 || t == 1.0)
                return t;

            double const c4 = (2 * std::numbers::pi) / 3;
            return -pow(2.0, 10.0 * t - 10.0) * sin((t * 10.0 - 10.75) * c4);
        }

        double easeOutElastic(double t) {
            if (t == 0.0 || t == 1.0)
                return t;

            double const c4 = (2 * std::numbers::pi) / 3;
            return pow(2.0, -10.0 * t) * sin((t * 10.0 - 0.75) * c4) + 1.0;
        }

        double easeInOutElastic(double t) {
            if (t == 0.0 || t == 1.0)
                return t;

            if (t < 0.5)
                return -(pow(2.0, 20.0 * t - 10.0) * sin((20.0 * t - 11.125) * (2 * std::numbers::pi) / 4.5)) / 2.0;

            return (pow(2.0, -20.0 * t + 10.0) * sin((20.0 * t - 11.125) * (2 * std::numbers::pi) / 4.5)) / 2.0 + 1.0;
        }

        /* == Bounce == */

        double easeInBounce(double t) {
            return 1.0 - easeOutBounce(1.0 - t);
        }

        double easeOutBounce(double t) {
            double const n1 = 7.5625;
            double const d1 = 2.75;

            if (t < 1.0 / d1)
                return n1 * t * t;
            if (t < 2.0 / d1) {
                auto t2 = t - 1.5 / d1;
                return n1 * t2 * t2 + 0.75;
            }
            if (t < 2.5 / d1) {
                auto t2 = t - 2.25 / d1;
                return n1 * t2 * t2 + 0.9375;
            }
            auto t2 = t - 2.625 / d1;
            return n1 * t2 * t2 + 0.984375;
        }

        double easeInOutBounce(double t) {
            if (t < 0.5)
                return (1.0 - easeOutBounce(1.0 - 2.0 * t)) / 2.0;

            return (1.0 + easeOutBounce(2.0 * t - 1.0)) / 2.0;
        }
    }
}

geode::Result<eclipse::gui::animation::Easing> matjson::Serialize<eclipse::gui::animation::Easing>::fromJson(
    Value const& value
) {
    GEODE_UNWRAP_INTO(auto n, value.asUInt());
    return geode::Ok(static_cast<eclipse::gui::animation::Easing>(n));
}

matjson::Value matjson::Serialize<eclipse::gui::animation::Easing>::toJson(
    eclipse::gui::animation::Easing const& easing
) {
    return static_cast<unsigned int>(easing);
}

geode::Result<eclipse::gui::animation::EasingMode> matjson::Serialize<eclipse::gui::animation::EasingMode>::fromJson(
    Value const& value
) {
    GEODE_UNWRAP_INTO(auto n, value.asUInt());
    return geode::Ok(static_cast<eclipse::gui::animation::EasingMode>(n));
}

matjson::Value matjson::Serialize<eclipse::gui::animation::EasingMode>::toJson(
    eclipse::gui::animation::EasingMode const& mode
) {
    return static_cast<unsigned int>(mode);
}
