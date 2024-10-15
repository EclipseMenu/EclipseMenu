#include "trace.hpp"

#include <Geode/loader/Log.hpp>

namespace eclipse::debug {
    Trace::Trace(std::string_view name) {
        m_name = name;
        geode::log::debug(">>> {}", m_name);
        geode::log::pushNest();
    }

    Trace::~Trace() {
        geode::log::popNest();
        geode::log::debug("<<< {}", m_name);
    }
}
