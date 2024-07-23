#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#ifdef __APPLE__

// silly goofy fix because it errors if it comes after geode includes
#define CommentType CommentTypeDummy
#include <CoreGraphics/CoreGraphics.h>
#include <CoreServices/CoreServices.h>
#undef CommentType

#endif

namespace eclipse::hacks::Global {

    class LockCursor : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Global");
            tab->addToggle("Lock Cursor", "global.lockcursor")->handleKeybinds();
        }

        void update() override {
            if (!PlayLayer::get()) return;

            #ifdef __APPLE__

            if (!gui::Engine::get()->isToggled() && config::get<bool>("global.lockcursor", false) && !PlayLayer::get()->m_hasCompletedLevel && !PlayLayer::get()->m_isPaused) {
                CGEventRef ourEvent = CGEventCreate(NULL);
                auto point = CGEventGetLocation(ourEvent);
                CFRelease(ourEvent);

                CGWarpMouseCursorPosition(point);
            }

            #endif
        }

        [[nodiscard]] const char* getId() const override { return "Lock Cursor"; }
    };

    REGISTER_HACK(LockCursor)
}