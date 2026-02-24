#pragma once
#include <functional.hpp>
#include <map>
#include <string>

namespace eclipse::api {
    geode::utils::StringMap<geode::Function<bool()>>& getCheats();
}
