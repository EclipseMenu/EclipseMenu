#pragma once
#include <modules/gui/gui.hpp>

namespace eclipse::gui::cocos {

    class Popup : public geode::Popup<Tabs> {
    protected:
        bool setup(Tabs tabs) override;
        void onExit() override;

    public:
        static Popup* create(Tabs tabs);

        bool isAncestorOf(cocos2d::CCNode* node) const;
    };

}