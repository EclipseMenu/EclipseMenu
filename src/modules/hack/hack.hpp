#pragma once
#include <Geode/Geode.hpp>

#define REGISTER_HACK(hackClass) $execute { eclipse::hack::Hack::registerHack<hackClass>(); }

constexpr int32_t SAFE_HOOK_PRIORITY = 0x500000;

#define SAFE_PRIORITY(name) do {                            \
    if (!self.setHookPriority(name, SAFE_HOOK_PRIORITY)) {  \
        geode::log::warn("Failed to set " name " hook priority!"); \
    }                                                       \
} while (0)


namespace eclipse::hack {

    /// @brief Base class for all hacks.
    class Hack {
    public:
        /// @brief Adds the hack to the list of hacks.
        static void registerHack(Hack* hack);

        /// @brief Registers a hack by its type.
        template<typename T, typename = std::enable_if_t<std::is_base_of_v<Hack, T>>>
        static void registerHack() { registerHack(new T()); }

        /// @brief Finds a hack by its ID.
        [[nodiscard]] static Hack* find(const std::string& id);

        /// @brief Get all registered hacks.
        [[nodiscard]] static const std::vector<Hack*>& getHacks();

        /// @brief Initializes all hacks.
        static void initializeHacks();

        /// @brief Late initializes all hacks.
        static void lateInitializeHacks();

        /// @brief Initializes the hack. Used to add GUI elements.
        virtual void init() = 0;

        /// @brief Late initialization. Used to run logic after the game has loaded.
        virtual void lateInit() = 0;

        /// @brief Callback for CCSchedule::update, called every frame.
        virtual void update() = 0;

        /// @brief Returns whether the hack should be considered cheating and is enabled.
        [[nodiscard]] virtual bool isCheating() { return false; }

        /// @brief Get the hack's ID. (unique identifier)
        [[nodiscard]] virtual const char* getId() const = 0;
    };

}