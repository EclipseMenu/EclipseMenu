#pragma once
#include <chrono>

namespace eclipse::debug {

    /// @brief Timer class to measure time taken by a block of code.
    /// Prints the time taken in nanoseconds to the console, or returns the value if a pointer is provided.
    class Timer {
    public:
        explicit Timer(std::string_view name, uint64_t* result = nullptr) : m_name(name), m_result(result) {
            m_start = std::chrono::high_resolution_clock::now();
        }

        ~Timer() {
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

    /// @brief Benchmark class to get the average execution time of a function.
    /// Can be limited by the number of iterations or the time taken, whichever comes first.
    class Benchmark {
    public:
        Benchmark(
            std::string_view name,
            std::function<void()> func,
            size_t maxIterations = 1'000'000'000, // 1 billion
            size_t maxTime = 10'000'000'000 // 10 seconds
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