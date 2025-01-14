#pragma once
#include <functional>
#include <map>
#include <string>

namespace eclipse::api {
    std::map<std::string, std::function<bool()>> const& getCheats();
}
