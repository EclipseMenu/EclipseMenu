#include <Geode/Geode.hpp>

#include <modules/config/config.hpp>

using namespace eclipse;

$on_mod(Loaded) {
    // Load the configuration file.
    config::load();
}