#pragma once

namespace eclipse::utils {
    /// @brief A spinlock implementation that uses atomic_flag.
    class spinlock {
        // macOS doesn't support atomic wait yet :P
        #if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
        #define HAS_ATOMIC_WAIT 1
        #else
        #define HAS_ATOMIC_WAIT 0
        #endif

    public:
        /// @brief Locks the thread until the flag is set to the desired state.
        void wait_for(bool state) const {
            #if HAS_ATOMIC_WAIT
            m_flag.wait(!state, std::memory_order_acquire);
            #else
            while (m_flag.test(std::memory_order_acquire) != state)
                std::this_thread::yield();
            #endif
        }

        /// @brief Checks the flag's state without locking the thread.
        [[nodiscard]] bool read() const {
            return m_flag.test(std::memory_order_acquire);
        }

        [[nodiscard]] operator bool() const { return read(); }

        /// @brief Sets the flag to the desired state and notifies any waiting threads.
        void set(bool state) {
            if (state) m_flag.test_and_set(std::memory_order_release);
            else m_flag.clear(std::memory_order_release);
            #if HAS_ATOMIC_WAIT
            m_flag.notify_one();
            #endif
        }

    private:
        std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
    };
}