#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#ifdef GEODE_IS_DESKTOP

#ifdef GEODE_IS_MACOS
// silly goofy fix because it errors if it comes after geode includes
#define CommentType CommentTypeDummy
#include <CoreGraphics/CoreGraphics.h>
#include <CoreServices/CoreServices.h>
#undef CommentType
#else
#include <Windows.h>
#endif

namespace eclipse::hacks::Global {

    class LockCursor : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");

            tab->addToggle("global.lockcursor")->setDescription()->handleKeybinds();
        }

        void update() override {
            auto* pl = utils::get<PlayLayer>();
            if (pl == nullptr) return; // not in a level
            if (pl->m_hasCompletedLevel || pl->m_isPaused) return; // level is completed or paused
            if (!config::get<bool>("global.lockcursor", false)) return; // not toggled
            if (gui::Engine::get()->isToggled()) return; // gui is open

            GEODE_WINDOWS(
                HWND hwnd = WindowFromDC(wglGetCurrentDC());
                RECT winSize; GetWindowRect(hwnd, &winSize);
                auto width = winSize.right - winSize.left;
                auto height = winSize.bottom - winSize.top;
                auto centerX = width / 2 + winSize.left;
                auto centerY = height / 2 + winSize.top;
                SetCursorPos(centerX, centerY);
            )

            GEODE_MACOS(
                CGEventRef ourEvent = CGEventCreate(NULL);
                auto point = CGEventGetLocation(ourEvent);
                CFRelease(ourEvent);
                CGWarpMouseCursorPosition(point);
            )
        }

        [[nodiscard]] const char* getId() const override { return "Lock Cursor"; }
    };

    REGISTER_HACK(LockCursor)
}

#endif