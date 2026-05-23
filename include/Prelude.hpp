#pragma once
#include <Geode/Result.hpp>
#include <Geode/utils/function.hpp>
#include <Geode/utils/general.hpp>

#include "Macros.hpp"

namespace eclipse {
    using geode::Function;
    using geode::utils::StringMap;
    using geode::Result;
    using geode::Ok;
    using geode::Err;
    using geode::Mod;
    using geode::Loader;

    namespace log = geode::log;
    namespace utils {
        using namespace geode::utils;
    }
}

namespace eclipse::prelude {
    using namespace eclipse;
    using namespace cocos2d;
}