#pragma once
#include <functional.hpp>
#include <map>
#include <string>

namespace eclipse::api {
    std::map<std::string, StdFunction<bool()>> const& getCheats();
}
