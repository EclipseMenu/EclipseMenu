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
            tab->addToggle("StartPos Switcher", "level.startpos_switcher")
                ->setDescription("Allows you to switch between StartPos objects")
                ->addOptions([](auto* options) {
                    options->addKeybind("Previous StartPos", "level.startpos_switcher.previous");
                    options->addKeybind("Next StartPos", "level.startpos_switcher.next");
                    options->addToggle("Reset Camera", "level.startpos_switcher.reset_camera");
                });
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
            playLayer->m_isTestMode = index >= 0;

            if (playLayer->m_isPracticeMode)
                playLayer->resetLevelFromStart();

            playLayer->resetLevel();
            playLayer->startMusic();
            playLayer->updateTestModeLabel();
        }

        void update() override {
            auto* playLayer = PlayLayer::get();
            if (!playLayer) return;

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
            return PlayLayer::init(level, useReplay, dontCreateObjects);
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

        void createObjectsFromSetupFinished() {
            PlayLayer::createObjectsFromSetupFinished();

            std::sort(startPosObjects.begin(), startPosObjects.end(), [](GameObject *a, GameObject *b) {
                return a->getPositionX() < b->getPositionX();
            });

            currentStartPosIndex = -1;
            if(m_startPosObject) {
                auto it = std::find(startPosObjects.begin(), startPosObjects.end(), m_startPosObject);
                if (it != startPosObjects.end()) {
                    currentStartPosIndex = static_cast<int32_t>(std::distance(startPosObjects.begin(), it));
                }
            }

            // TODO: Spawn label
        }
    };

}