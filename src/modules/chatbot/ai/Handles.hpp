#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace eclipse::ai {
    template <class Tag, class N = uint16_t>
    struct Handle {
        using Number = N;
        static constexpr N INVALID = std::numeric_limits<N>::max();

        N id;

        constexpr Handle() : id(INVALID) {}
        constexpr Handle(N id) : id(id) {}

        constexpr bool operator==(Handle const& other) const { return id == other.id; }
        constexpr bool operator==(N otherId) const { return id == otherId; }

        constexpr explicit operator bool() const { return id != INVALID; }
        constexpr operator N() const { return id; }
    };

    template <class Tag>
    class HandleRegistry {
    public:
        using ID = Handle<Tag>;

        ID intern(std::string const& name) {
            auto [it, inserted] = m_nameToId.emplace(name, static_cast<ID>(m_names.size()));
            if (inserted) m_names.push_back(name);
            return it->second;
        }

        std::optional<ID> find(std::string_view name) const {
            auto it = m_nameToId.find(name);
            if (it == m_nameToId.end()) return std::nullopt;
            return it->second;
        }

        std::string const& name(ID id) const {
            if (id.id >= m_names.size()) return INVALID_NAME;
            return m_names[id.id];
        }

        [[nodiscard]] size_t size() const { return m_names.size(); }
        [[nodiscard]] bool empty() const { return m_names.empty(); }
        void clear() {
            m_nameToId.clear();
            m_names.clear();
        }

        auto begin() const { return m_names.begin(); }
        auto end() const { return m_names.end(); }

    private:
        inline static std::string const INVALID_NAME = "<invalid>";
        geode::utils::StringMap<ID> m_nameToId;
        std::vector<std::string> m_names;
    };

    struct IntentTag {};
    struct EmotionTag {};

    using Intent = Handle<IntentTag>;
    using Emotion = Handle<EmotionTag>;
    using IntentRegistry = HandleRegistry<IntentTag>;
    using EmotionRegistry = HandleRegistry<EmotionTag>;
}

template <class Tag>
struct std::hash<eclipse::ai::Handle<Tag>> {
    size_t operator()(eclipse::ai::Handle<Tag> const& handle) const noexcept {
        return std::hash<typename eclipse::ai::Handle<Tag>::Number>{}(handle.id);
    }
};