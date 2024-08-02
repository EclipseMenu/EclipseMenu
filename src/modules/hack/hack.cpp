#include "hack.hpp"

#include <modules/gui/gui.hpp>

namespace eclipse::hack {

    static std::vector<std::shared_ptr<Hack>> hacks;
    static bool s_lateInit = false;

    const std::vector<std::shared_ptr<Hack>>& Hack::getHacks() {
        return hacks;
    }

    void Hack::registerHack(std::shared_ptr<Hack> hack) {
        hacks.push_back(hack);
        if (s_lateInit)
            hack->init();
    }

    std::weak_ptr<Hack> Hack::find(const std::string& id) {
        for (auto hack : hacks) {
            if (hack->getId() == id) {
                return hack;
            }
        }

        return {};
    }

    void Hack::initializeHacks() {
        // Sort hacks by priority (and then ID)
        std::sort(hacks.begin(), hacks.end(), [](const std::shared_ptr<Hack>& a, const std::shared_ptr<Hack>& b) {
            if (a->getPriority() == b->getPriority()) {
                std::string aId = a->getId();
                std::string bId = b->getId();
                return aId < bId;
            }

            return a->getPriority() < b->getPriority();
        });

        for (const auto hack : hacks)
            hack->init();

        s_lateInit = true;
    }

    void Hack::lateInitializeHacks() {
        for (const auto hack : hacks)
            hack->lateInit();
    }

}