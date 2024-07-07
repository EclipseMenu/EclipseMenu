#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCScheduler.hpp>

namespace eclipse::hacks::Global {

    class Speedhack : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Global");

            config::setIfEmpty("global.speedhack.toggle", false);
            config::setIfEmpty("global.speedhack", 1.f);

            tab->addFloatToggle("Speedhack", "global.speedhack", 0.0001f, 1000.f, "%.4f");
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("global.speedhack.toggle", false); }
        [[nodiscard]] const char* getId() const override { return "Speedhack"; }
        [[nodiscard]] int32_t getPriority() const override { return -10; }
    };

    REGISTER_HACK(Speedhack)

    class $modify(cocos2d::CCScheduler)
    {
        void update(float dt)
        {
            float speedhack =
            config::get<bool>("global.speedhack.toggle", false) ? config::get<float>("global.speedhack", 1.f) : 1.f;

            if(speedhack <= 0)
                speedhack = 1.f;

            dt *= speedhack;

            cocos2d::CCScheduler::update(dt);
        }
    };

}
