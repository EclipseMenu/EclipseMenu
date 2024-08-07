#include "popup.hpp"
#include <modules/gui/cocos/cocos.hpp>

#include <utility>

namespace eclipse::gui::cocos {

    bool Popup::setup(Tabs tabs) {
        auto winSize = cocos2d::CCDirector::get()->getWinSize();

        // convenience function provided by Popup
        // for adding/setting a title to the popup
        this->setTitle("Eclipse Menu");

        return true;
    }

    Popup* Popup::create(Tabs tabs) {
        auto ret = new Popup;
        if (ret->initAnchored(480.f, 280.f, std::move(tabs))) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    bool Popup::isAncestorOf(cocos2d::CCNode* node) const {
        while (node) {
            if (node == this) return true;
            node = node->getParent();
        }

        return false;
    }

    void Popup::onExit() {
        geode::Popup<Tabs>::onExit();
        cocos::CocosRenderer::get()->shutdown(true);
    }

}
