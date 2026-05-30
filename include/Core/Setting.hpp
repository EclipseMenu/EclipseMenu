#pragma once
#include <asp/collections/SmallVec.hpp>
#include "../Prelude.hpp"

namespace eclipse {
    template <class T>
    class Setting {
    public:
        using value_type = T;
        using ObserverFn = Function<void(T const&)>;

        constexpr Setting() noexcept(std::is_nothrow_default_constructible_v<T>) = default;
        explicit constexpr Setting(T initial) noexcept(std::is_nothrow_move_constructible_v<T>) : m_value(std::move(initial)) {}

        Setting(Setting const&) = delete;
        Setting(Setting&&) = delete;
        Setting& operator=(Setting const&) = delete;
        Setting& operator=(Setting&&) = delete;

        [[nodiscard]] constexpr T const& value() const noexcept { return m_value; }
        [[nodiscard]] constexpr T& value() noexcept { return m_value; }

        void set(T&& value) {
            if constexpr (requires{ m_value == value; }) {
                if (m_value == value) return;
            }

            m_value = std::forward<T>(value);
            this->notify();
        }

        void set(T const& value) {
            if constexpr (requires{ m_value == value; }) {
                if (m_value == value) return;
            }

            m_value = value;
            this->notify();
        }

        void notify() {
            for (auto& obs : m_observers) {
                obs.fn(m_value);
            }
        }

        size_t listen(ObserverFn fn) {
            size_t id = m_nextObserverId++;
            m_observers.push_back({ id, std::move(fn) });
            return id;
        }

        void unlisten(size_t id) {
            auto it = std::ranges::find_if(
                m_observers,
                [id](Observer const& obs) { return obs.id == id; }
            );

            if (it != m_observers.end()) {
                m_observers.erase(it);
            }
        }

    private:
        struct Observer {
            size_t id;
            ObserverFn fn;
        };

        T m_value{};
        asp::SmallVec<Observer, 4> m_observers;
        size_t m_nextObserverId = 0;
    };
}
