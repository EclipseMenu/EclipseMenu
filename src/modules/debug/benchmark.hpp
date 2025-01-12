#pragma once
#include <chrono>
#include "utils.hpp"

namespace eclipse::debug {
    /// @brief Timer class to measure time taken by a block of code.
    /// Prints the time taken in nanoseconds to the console, or returns the value if a pointer is provided.
    class Timer {
    public:
        explicit Timer(std::string_view name, uint64_t* result = nullptr) : m_name(name), m_result(result) {
            m_start = std::chrono::high_resolution_clock::now();
        }

        ~Timer() { stop(); }

        void stop() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_start);
            if (m_result) {
                *m_result = duration.count();
            } else {
                geode::log::info("{} took {}ns", m_name, duration.count());
            }
        }

    private:
        std::string_view m_name;
        uint64_t* m_result;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    };

    /// @brief Measures the time taken by a block of code over time, and stores the average time taken.
    class Profiler {
    public:
        explicit Profiler(std::string_view name) : m_timer(name, &m_time), m_name(name), m_time(0) {}

        ~Profiler() {
            m_timer.stop();
            registerTime(m_name, m_time);
        }

        static uint64_t averageTimeFor(std::string_view name);
        static std::deque<uint64_t> const& allTimesFor(std::string_view name);
        static void clearTimesFor(std::string_view name);
        static void clearAllTimes();
        static std::vector<std::string_view> allNames();
        static std::unordered_map<std::string, std::deque<uint64_t>> const& getTimes();

    private:
        static void registerTime(std::string_view name, uint64_t time);

    private:
        Timer m_timer;
        std::string_view m_name;
        uint64_t m_time;
    };

    /// @brief Benchmark class to get the average execution time of a function.
    /// Can be limited by the number of iterations or the time taken, whichever comes first.
    class Benchmark {
    public:
        Benchmark(
            std::string_view name,
            std::function<void()> func,
            size_t maxIterations = 1'000'000'000, // 1 billion
            size_t maxTime = 10'000'000'000       // 10 seconds
        ) : m_name(name), m_func(std::move(func)), m_maxIterations(maxIterations), m_maxTime(maxTime) { run(); }

        void run() {
            size_t iterations = 0;
            size_t totalTime = 0;

            do {
                uint64_t time;
                Timer timer(m_name, &time);
                m_func();
                iterations++;
                totalTime += time;
            } while (iterations < m_maxIterations && totalTime < m_maxTime);

            auto averageTime = totalTime / iterations;
            geode::log::info("{} took an average of {}ns ({} iterations)", m_name, averageTime, iterations);
        }

    private:
        std::string_view m_name;
        std::function<void()> m_func;
        size_t m_maxIterations;
        size_t m_maxTime;
    };
}

#ifdef ECLIPSE_TRACING
#define PROFILER(name) eclipse::debug::Profiler GEODE_CONCAT(profiler, __LINE__)(name)
#define PROFILER_FUNC1(line) GET_FUNC_NAME(newName_##line); PROFILER(newName_##line)
#define PROFILER_FUNC2(line) PROFILER_FUNC1(line)
#define PROFILER_FUNC() PROFILER_FUNC2(__LINE__)
#else
#define PROFILER(name)
#define PROFILER_FUNC()
#endif
