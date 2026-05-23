#include <Core/Registry.hpp>

#include <Events.hpp>

namespace eclipse::__detail::event {
    static Table& getGlobalTable() {
        static Table table {
            .version = API_VERSION,

            .Registry_get = &::eclipse::Registry::get,
            .Registry_bindSetting = &::eclipse::Registry::bindSetting,
            .Registry_findSetting = &::eclipse::Registry::findSetting,
            .Registry_loadFromFile = &::eclipse::Registry::loadFromFile,
            .Registry_saveToFile = &::eclipse::Registry::saveToFile
        };
        return table;
    }

    $execute {
        FetchTableEvent()
            .listen([](Table*& out) { out = &getGlobalTable(); })
            .leak();
    }
}
