#include "hack.hpp"

#include <Geode/Geode.hpp>

namespace eclipse::hack {

    static std::vector<Hack*> hacks;

    const std::vector<Hack*>& Hack::getHacks() {
        return hacks;
    }

    void Hack::registerHack(Hack* hack) {
        hacks.push_back(hack);
        hack->init();
    }

    Hack* Hack::find(const std::string& id) {
        for (Hack* hack : hacks) {
            if (hack->getId() == id) {
                return hack;
            }
        }
        return nullptr;
    }

}