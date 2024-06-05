#pragma once
#include <Geode/Geode.hpp>

#define REGISTER_HACK(hackClass) $execute { eclipse::hack::Hack::registerHack(new hackClass()); }

namespace eclipse::hack {

    /// @brief Base class for all hacks.
    class Hack {
    public:
        /// @brief Adds the hack to the list of hacks.
        static void registerHack(Hack* hack);

        /// @brief Finds a hack by its ID.
        [[nodiscard]] static Hack* find(const std::string& id);

        /// @brief Get all registered hacks.
        [[nodiscard]] static const std::vector<Hack*>& getHacks();

        /// @brief Initializes the hack. Used to add GUI elements.
        virtual void init() = 0;

        /// @brief Callback for CCSchedule::update, called every frame.
        virtual void update() = 0;

        /// @brief Returns whether the hack should be considered cheating and is enabled.
        [[nodiscard]] virtual bool isCheating() { return false; }

        /// @brief Get the hack's ID. (unique identifier)
        [[nodiscard]] virtual const char* getId() const = 0;
    };

}