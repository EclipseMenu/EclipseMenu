#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/keybinds/manager.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/UILayer.hpp>

namespace eclipse::hacks::Level {

    static std::vector<StartPosObject*> startPosObjects;
    static int32_t currentStartPosIndex = 0;

    class StartPosSwitcher : public hack::Hack {
    public:
        void init() override {
            config::setIfEmpty("level.startpos_switcher", false);
            config::setIfEmpty("level.startpos_switcher.reset_camera", false);
            config::setIfEmpty("level.startpos_switcher.previous", keybinds::Keys::Q);
            config::setIfEmpty("level.startpos_switcher.next", keybinds::Keys::E);
            config::setIfEmpty("level.startpos_switcher.label", true);
            config::setIfEmpty("label.startpos_switcher.scale", 0.7f);
            config::setIfEmpty("label.startpos_switcher.buttons", true);
            config::setIfEmpty("label.startpos_switcher.color", gui::Color(1.f, 1.f, 1.f, 0.6f));
            config::setIfEmpty("label.startpos_switcher.alpha_mod", 0.4f);

            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("StartPos Switcher", "level.startpos_switcher")
                ->handleKeybinds()
                ->setDescription("Allows you to switch between StartPos objects.")
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                    options->addKeybind("Previous StartPos", "level.startpos_switcher.previous");
                    options->addKeybind("Next StartPos", "level.startpos_switcher.next");
                    options->addToggle("Reset Camera", "level.startpos_switcher.reset_camera");
                    options->addToggle("Show Label", "level.startpos_switcher.label")
                        ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                            options->addInputFloat("Label Scale", "label.startpos_switcher.scale", 0.1f, 2.f, "%.2fx");
                            options->addInputFloat("Opacity Modifier", "label.startpos_switcher.alpha_mod", 0.f, 1.f);
                            options->addColorComponent("Label Color", "label.startpos_switcher.color", true);
                            options->addToggle("Show Buttons", "label.startpos_switcher.buttons")
                                ->setDescription("Toggles between showing and hiding the arrow buttons in the StartPos Switcher UI");
                        });
                });
        }

        static void pickStartPos(PlayLayer* playLayer, int32_t index) {
            if (startPosObjects.empty()) return;

            auto count = static_cast<int32_t>(startPosObjects.size());
            if (index >= count)
                index = -1;
            else if (index < -1)
                index = count - 1;

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

            if (keybinds::isKeyPressed(config::get<keybinds::Keys>("level.startpos_switcher.previous")))
                pickStartPos(playLayer, currentStartPosIndex - 1);
            else if (keybinds::isKeyPressed(config::get<keybinds::Keys>("level.startpos_switcher.next")))
                pickStartPos(playLayer, currentStartPosIndex + 1);
        }

        [[nodiscard]] const char* getId() const override { return "StartPos Switcher"; }
    };

    REGISTER_HACK(StartPosSwitcher)

    class StartposSwitcherNode : public cocos2d::CCMenu {
    public:
        static StartposSwitcherNode* create(PlayLayer* playLayer) {
            auto* ret = new StartposSwitcherNode;
            if (ret->init(playLayer)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        bool init(PlayLayer* playLayer) {
            if (!CCMenu::init()) return false;

            m_playLayer = playLayer;

            m_previous = CCMenuItemSpriteExtra::create(
                cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"),
                this, menu_selector(StartposSwitcherNode::onPrevious));
            m_previous->setID("arrow-previous");

            auto* sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
            m_next = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(StartposSwitcherNode::onNext));
            sprite->setFlipX(true);
            m_next->setID("arrow-next");

            m_labelText = fmt::format("{}/{}", currentStartPosIndex + 1, startPosObjects.size());
            m_label = cocos2d::CCLabelBMFont::create(m_labelText.c_str(), "bigFont.fnt");
            m_label->setID("label");

            m_previous->setPosition(-65.0f, 0.0f);
            m_next->setPosition(65.0f, 0.0f);
            m_label->setPosition(0.0f, 0.0f);

            auto scale = config::get<float>("label.startpos_switcher.scale", 0.7f);
            auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
            this->setPosition(winSize.width / 2.f, 30.f * scale);
            this->setAnchorPoint({0.f, 0.f});

            this->addChild(m_previous);
            this->addChild(m_next);
            this->addChild(m_label);

            auto color = config::get<gui::Color>("label.startpos_switcher.color", gui::Color(1.f, 1.f, 1.f, 0.6f));
            auto buttons = config::get<bool>("label.startpos_switcher.buttons", true);

            this->setScale(scale);
            m_label->setColor(color.toCCColor3B());
            this->setOpacity(color.a * 255);
            m_previous->setVisible(buttons);
            m_next->setVisible(buttons);

            this->schedule(schedule_selector(StartposSwitcherNode::update), 0.f);

            return true;
        }

        void onPrevious(CCObject* sender) {
            StartPosSwitcher::pickStartPos(m_playLayer, currentStartPosIndex - 1);
        }

        void onNext(CCObject* sender) {
            StartPosSwitcher::pickStartPos(m_playLayer, currentStartPosIndex + 1);
        }

        void setVisibility(bool visible) {
            m_label->setVisible(visible);
            bool buttons = !visible ? false : config::get<bool>("label.startpos_switcher.buttons", true);
            m_previous->setVisible(buttons);
            m_next->setVisible(buttons);
        }

        void update(float dt) override {
            if (!config::get<bool>("level.startpos_switcher", false) ||
                !config::get<bool>("level.startpos_switcher.label", true)) {
                setVisibility(false);
                m_timeSinceAction = 0.f;
                return;
            }

            setVisibility(true);
            m_timeSinceAction += dt;

            // Update scale and color
            auto scale = config::get<float>("label.startpos_switcher.scale", 0.7f);
            auto color = config::get<gui::Color>("label.startpos_switcher.color", gui::Color(1.f, 1.f, 1.f, 0.6f));
            this->setScale(scale);
            m_label->setColor(color.toCCColor3B());


            constexpr float animationTime = 0.5f;
            if (m_timeSinceAction < 3.f) {
                this->setOpacity(color.a * 255);
            } else {
                auto opacityMod = 1.f - config::get<float>("label.startpos_switcher.alpha_mod", 0.4f);
                auto clampedTime = std::clamp(m_timeSinceAction - 3.f, 0.f, animationTime) / animationTime;
                auto modifier = 1.f - (clampedTime * opacityMod);
                this->setOpacity(color.a * 255 * modifier);
            }

            // Center the node on the screen (center bottom)
            auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
            this->setPosition(winSize.width / 2.f, 30.f * scale);
 
            auto label = fmt::format("{}/{}", currentStartPosIndex + 1, startPosObjects.size());
            if (label != m_labelText) {
                m_labelText = label;
                m_label->setCString(m_labelText.c_str());
                m_timeSinceAction = 0.f;
            }
        }

    private:
        CCMenuItemSpriteExtra* m_previous;
        CCMenuItemSpriteExtra* m_next;
        cocos2d::CCLabelBMFont* m_label;
        PlayLayer* m_playLayer;
        std::string m_labelText;
        float m_timeSinceAction = 0.f;
    };

    class $modify(StartPosSwitcherPLHook, PlayLayer) {
        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            startPosObjects.clear();

            return PlayLayer::init(level, useReplay, dontCreateObjects);
        }

        void resetLevel() {
            PlayLayer::resetLevel();

            // Reset camera
            if (currentStartPosIndex >= 0 && config::get<bool>("level.startpos_switcher.reset_camera", true))
                this->resetCamera();
        }

        void addObject(GameObject* object) {
            PlayLayer::addObject(object);

            if (object->m_objectID == 31)
                startPosObjects.push_back(geode::cast::typeinfo_cast<StartPosObject*>(object));
        }

        void createObjectsFromSetupFinished() {
            PlayLayer::createObjectsFromSetupFinished();
            if (startPosObjects.empty()) return;

            std::ranges::sort(startPosObjects, [](GameObject* a, GameObject* b) {
                return a->getPositionX() < b->getPositionX();
            });

            currentStartPosIndex = -1;
            if (m_startPosObject) {
                auto it = std::ranges::find(startPosObjects, m_startPosObject);
                if (it != startPosObjects.end())
                    currentStartPosIndex = static_cast<int32_t>(std::distance(startPosObjects.begin(), it));
            }

            auto* switcher = StartposSwitcherNode::create(this);
            switcher->setID("startpos-switcher"_spr);
            if (auto uiMenu = geode::cast::typeinfo_cast<cocos2d::CCMenu*>(m_uiLayer->getChildByID("eclipse-ui"_spr))) {
                uiMenu->addChild(switcher, 1000);
            } else { // fallback
                m_uiLayer->addChild(switcher, 1000);
            }
        }
    };

}
