#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCScheduler.hpp>

namespace eclipse::hacks::Global {

    class Speedhack : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");

            // speedhack should always be disabled by default
            // to prevent freezing the game while loading
            config::set("global.speedhack.toggle", false);
            config::setIfEmpty("global.speedhack", 1.f);

            tab->addFloatToggle("global.speedhack", "global.speedhack", 0.0001f, 1000.f, "%.4f")
                ->setDescription()
                ->handleKeybinds();
        }

        [[nodiscard]] bool isCheating() override { 
            return config::get<bool>("global.speedhack.toggle", false) && 
                   config::get<float>("global.speedhack", 1.f) != 1.f;
        }
        [[nodiscard]] const char* getId() const override { return "Speedhack"; }
        [[nodiscard]] int32_t getPriority() const override { return -10; }
    };

    REGISTER_HACK(Speedhack)

    class $modify(SpeedhackSchedulerHook, cocos2d::CCScheduler) {
        ADD_HOOKS_DELEGATE("global.speedhack.toggle")

        void update(float dt) override {
            auto speed = config::get<float>("global.speedhack", 1.f);

            if (speed <= 0)
                speed = 1.f;

            dt *= speed;

            CCScheduler::update(dt);
        }
    };

}
