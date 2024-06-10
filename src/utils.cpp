#include "utils.hpp"

namespace eclipse::utils {

    std::random_device& getRng() {
        static std::random_device rng;
        return rng;
    }

}