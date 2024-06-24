#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/keybinds/manager.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    static std::vector<StartPosObject*> startPosObjects;
    static int32_t currentStartPosIndex = 0;

    class StartPosSwitcher : public hack::Hack {
        void init() override {
            config::setIfEmpty("level.startpos_switcher", false);
            config::setIfEmpty("level.startpos_switcher.reset_camera", false);
            config::setIfEmpty("level.startpos_switcher.previous", keybinds::Keys::Q);
            config::setIfEmpty("level.startpos_switcher.next", keybinds::Keys::E);

            auto tab = gui::MenuTab::find("Level");
            auto startposSwitcher = tab->addToggle("StartPos Switcher", "level.startpos_switcher");
            startposSwitcher->setDescription("Allows you to switch between StartPos objects");
            // TODO: Implement options for widgets
            // startposSwitcher->addOptions([](auto* options) {
            //
            // });
        }

        static void pickStartPos(PlayLayer* playLayer, int32_t index) {
            if (startPosObjects.empty()) return;

            auto count = static_cast<int32_t>(startPosObjects.size());
            if (index >= count) {
                index = -1;
            } else if (index < -1) {
                index = count - 1;
            }

            currentStartPosIndex = index;
            playLayer->m_currentCheckpoint = nullptr;

            auto* startPos = index >= 0 ? startPosObjects[index] : nullptr;
            playLayer->setStartPosObject(startPos);

            if (playLayer->m_isPracticeMode)
                playLayer->resetLevelFromStart();

            playLayer->resetLevel();
            playLayer->startMusic();
        }

        void update() override {
            auto* playLayer = PlayLayer::get();
            if (!playLayer) {
                // TODO: cleanup
                return;
            }

            if (!config::get<bool>("level.startpos_switcher", false)) return;

            if (keybinds::isKeyPressed(config::get<keybinds::Keys>("level.startpos_switcher.previous"))) {
                pickStartPos(playLayer, currentStartPosIndex - 1);
            } else if (keybinds::isKeyPressed(config::get<keybinds::Keys>("level.startpos_switcher.next"))) {
                pickStartPos(playLayer, currentStartPosIndex + 1);
            }
        }

        [[nodiscard]] const char* getId() const override { return "StartPos Switcher"; }
    };

    REGISTER_HACK(StartPosSwitcher)

    class $modify(PlayLayer) {
        bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
            startPosObjects.clear();

            if (!PlayLayer::init(level, useReplay, dontCreateObjects))
                return false;

            auto count = static_cast<int32_t>(startPosObjects.size());
            currentStartPosIndex = m_isTestMode ? count - 1 : -1;

            // TODO: Spawn label

            return true;
        }

        void resetLevel() {
            PlayLayer::resetLevel();

            // TODO: Update label

            // Reset camera
            if (currentStartPosIndex >= 0 && config::get<bool>("level.startpos_switcher.reset_camera", true)) {
                this->resetCamera();
            }
        }

        void addObject(GameObject *object) {
            PlayLayer::addObject(object);

            uint32_t id = object->m_objectID;
            if (id == 31) {
                startPosObjects.push_back(geode::cast::typeinfo_cast<StartPosObject*>(object));
            }
        }
    };

}