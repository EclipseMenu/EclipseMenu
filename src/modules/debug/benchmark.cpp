#include "benchmark.hpp"
#include <deque>

namespace eclipse::debug {
    std::unordered_map<std::string, std::deque<uint64_t>> g_times;
    constexpr size_t MAX_TIMES = 10000;

    uint64_t Profiler::averageTimeFor(std::string_view name) {
        std::string nameStr(name);
        if (!g_times.contains(nameStr)) {
            return 0;
        }
        uint64_t total = 0;
        for (auto const& time : g_times[nameStr]) {
            total += time;
        }
        return total / g_times[nameStr].size();
    }

    std::deque<uint64_t> const& Profiler::allTimesFor(std::string_view name) {
        std::string nameStr(name);
        if (!g_times.contains(nameStr)) {
            g_times[nameStr] = {};
        }
        return g_times[nameStr];
    }

    void Profiler::clearTimesFor(std::string_view name) {
        g_times.erase(std::string(name));
    }

    void Profiler::clearAllTimes() {
        g_times.clear();
    }

    std::vector<std::string_view> Profiler::allNames() {
        std::vector<std::string_view> names;
        names.reserve(g_times.size());
        for (auto const& [name, _] : g_times) {
            names.push_back(name);
        }
        return names;
    }

    std::unordered_map<std::string, std::deque<uint64_t>> const& Profiler::getTimes() {
        return g_times;
    }

    void Profiler::registerTime(std::string_view name, uint64_t time) {
        std::string nameStr(name);
        g_times[nameStr].push_back(time);
        if (g_times[nameStr].size() > MAX_TIMES) {
            g_times[nameStr].pop_front();
        }
    }
}
