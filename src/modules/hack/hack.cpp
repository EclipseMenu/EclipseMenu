#include "hack.hpp"

#include <modules/gui/gui.hpp>

namespace eclipse::hack {

    static bool s_lateInit = false;

    bool isLateInit() {
        return s_lateInit;
    }

    std::vector<HackPtr>& getHacks() {
        static std::vector<HackPtr> hacks;
        return hacks;
    }

    std::vector<HackPtr>& getUpdatedHacks() {
        static std::vector<HackPtr> hacks;
        return hacks;
    }

    std::vector<HackPtr>& getCheatingHacks() {
        static std::vector<HackPtr> hacks;
        return hacks;
    }

    void registerHack(HackPtr&& hack) {
        getHacks().push_back(hack);
        if (s_lateInit)
            hack->init();
    }

    WeakHackPtr find(std::string_view id) {
        for (auto hack : getHacks()) {
            if (hack->getId() == id) {
                return hack;
            }
        }
        return {};
    }

    void initializeHacks() {
        auto& hacks = getHacks();

        // Sort hacks by priority (and then ID)
        std::ranges::sort(hacks, [](auto& a, auto& b) {
            if (a->getPriority() == b->getPriority()) {
                std::string_view aId = a->getId();
                std::string_view bId = b->getId();
                return aId < bId;
            }

            return a->getPriority() < b->getPriority();
        });

        for (const auto hack : hacks)
            hack->init();

        s_lateInit = true;
    }

    void lateInitializeHacks() {
        for (const auto hack : getHacks())
            hack->lateInit();
    }

}