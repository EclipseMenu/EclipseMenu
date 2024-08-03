#pragma once

#include <memory>

#include <utils.hpp>

#define REGISTER_HACK(hackClass) $execute { eclipse::hack::Hack::registerHack<hackClass>(); }

constexpr int32_t SAFE_HOOK_PRIORITY = 0x500000;
constexpr int32_t FIRST_HOOK_PRIORITY = -0x500000;

#define SAFE_PRIORITY(name) do {                            \
    if (!self.setHookPriority(name, SAFE_HOOK_PRIORITY)) {  \
        geode::log::warn("Failed to set " name " hook priority!"); \
    }                                                       \
} while (0)

#define FIRST_PRIORITY(name) do {                           \
    if (!self.setHookPriority(name, FIRST_HOOK_PRIORITY)) { \
        geode::log::warn("Failed to set " name " hook priority!"); \
    }                                                       \
} while (0)

namespace eclipse::hack {

    /// @brief Base class for all hacks.
    class Hack {
    public:
        /// @brief Adds the hack to thred of hacks.
        static void registerHack(std::shared_ptr<Hack> hack);

        /// @brief Registers a hack by its type.
        template<typename T, typename = std::enable_if_t<std::is_base_of_v<Hack, T>>>
        static void registerHack() { registerHack(std::make_shared<T>()); }

        /// @brief Finds a hack by its ID.
        [[nodiscard]] static std::weak_ptr<Hack> find(const std::string& id);

        /// @brief Get all registered hacks.
        [[nodiscard]] static const std::vector<std::shared_ptr<Hack>>& getHacks();

        /// @brief Initializes all hacks.
        static void initializeHacks();

        /// @brief Late initializes all hacks.
        static void lateInitializeHacks();

        /// @brief Initializes the hack. Used to add GUI elements.
        virtual void init() = 0;

        /// @brief Late initialization. Used to run logic after the game has loaded.
        virtual void lateInit() {}

        /// @brief Callback for CCSchedule::update, called every frame.
        virtual void update() {}

        /// @brief Returns whether the hack should be considered cheating and is enabled.
        [[nodiscard]] virtual bool isCheating() { return false; }

        /// @brief Get the hack's ID. (unique identifier)
        [[nodiscard]] virtual const char* getId() const = 0;

        /// @brief Get hack's position priority (used for sorting)
        [[nodiscard]] virtual int32_t getPriority() const { return 0; }
    };

}