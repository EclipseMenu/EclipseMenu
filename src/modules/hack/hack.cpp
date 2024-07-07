#include "hack.hpp"

#include <Geode/Geode.hpp>
#include <modules/gui/gui.hpp>

namespace eclipse::hack {

    static std::vector<Hack*> hacks;
    static bool s_lateInit = false;

    const std::vector<Hack*>& Hack::getHacks() {
        return hacks;
    }

    void Hack::registerHack(Hack* hack) {
        hacks.push_back(hack);
        if (s_lateInit) {
            hack->init();
        }
    }

    Hack* Hack::find(const std::string& id) {
        for (Hack* hack : hacks) {
            if (hack->getId() == id) {
                return hack;
            }
        }
        return nullptr;
    }

    void Hack::initializeHacks() {
        // Sort hacks by priority (and then ID)
        std::sort(hacks.begin(), hacks.end(), [](Hack* a, Hack* b) {
            if (a->getPriority() == b->getPriority()) {
                std::string aId = a->getId();
                std::string bId = b->getId();
                return aId < bId;
            }
            return a->getPriority() < b->getPriority();
        });

        for (Hack* hack : hacks) {
            hack->init();
        }
        s_lateInit = true;
    }

    void Hack::lateInitializeHacks() {
        for (Hack* hack : hacks) {
            hack->lateInit();
        }
    }

}