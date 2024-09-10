#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <modules/labels/variables.hpp>
#include <modules/labels/setting.hpp>

#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Labels {

    static std::vector<labels::LabelSettings> s_labels;

    time_t getTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }

    // timestamps, total clicks (reset on death)
    using ClickInfo = std::pair<std::deque<time_t>, size_t>;
    struct ClickStorage {
        ClickInfo jumpClicks;
        ClickInfo leftClicks;
        ClickInfo rightClicks;

        void reset() {
            jumpClicks = {};
            leftClicks = {};
            rightClicks = {};
        }

        void cleanup() {
            const auto now = getTimestamp();
            auto removeOld = [now](ClickInfo& info) {
                while (!info.first.empty() && now - info.first.front() > 1000) {
                    info.first.pop_front();
                }
            };

            removeOld(jumpClicks);
            removeOld(leftClicks);
            removeOld(rightClicks);
        }

        void resetOnDeath() {
            jumpClicks.second = 0;
            leftClicks.second = 0;
            rightClicks.second = 0;
            reset();
        }

        void addClick(const PlayerButton btn) {
            ClickInfo* clickInfo;
            switch (btn) {
                case PlayerButton::Jump:
                    clickInfo = &jumpClicks;
                    break;
                case PlayerButton::Left:
                    clickInfo = &leftClicks;
                    break;
                case PlayerButton::Right:
                    clickInfo = &rightClicks;
                    break;
                default:
                    return;
            }

            clickInfo->first.push_back(getTimestamp());
            clickInfo->second++;
        }

        int getCPS(const PlayerButton btn) const {
            switch (btn) {
                case PlayerButton::Jump:
                    return jumpClicks.first.size();
                case PlayerButton::Left:
                    return leftClicks.first.size();
                case PlayerButton::Right:
                    return rightClicks.first.size();
                default:
                    return 0;
            }
        }

        int getClicks(const PlayerButton btn) const {
            switch (btn) {
                case PlayerButton::Jump:
                    return jumpClicks.second;
                case PlayerButton::Left:
                    return leftClicks.second;
                case PlayerButton::Right:
                    return rightClicks.second;
                default:
                    return 0;
            }
        }
    };
    static ClickStorage s_clicksP1;
    static ClickStorage s_clicksP2;

    class $modify(LabelsPOHook, PlayerObject) {
        /*void pushButton(PlayerButton btn) {
            PlayerObject::pushButton(btn);

            const auto* gjbgl = GJBaseGameLayer::get();
            if (!gjbgl) return;

            const bool isP1 = this == gjbgl->m_player1;
            const bool isP2 = this == gjbgl->m_player2;

            if (isP1)
                s_clicksP1.addClick(btn);
            else if (isP2)
                s_clicksP2.addClick(btn);
        }*/
    };

    class $modify(LabelsPLHook, PlayLayer) {
        void resetLevel() {
            PlayLayer::resetLevel();
            s_clicksP1.resetOnDeath();
            s_clicksP2.resetOnDeath();
        }
    };

    class $modify(LabelsUILHook, UILayer) {
        struct Fields {
            cocos2d::CCNode* m_mainContainer;
            std::array<LabelsContainer*, 9> m_containers;
        };

        void createLabels() {
            // Create the containers
            auto alignment = LabelsContainer::Alignment::TopLeft;
            for (auto& container : m_fields->m_containers) {
                container = LabelsContainer::create(alignment);
                container->setID(fmt::format("label-container-{}", static_cast<int>(alignment)));
                m_fields->m_mainContainer->addChild(container);
                alignment = static_cast<LabelsContainer::Alignment>(static_cast<int>(alignment) + 1);
            }

            // Add the cheat indicator
            {
                auto* cheatIndicator = SmartLabel::create(".", "bigFont.fnt");
                cheatIndicator->setHeightMultiplier(0.37f);
                cheatIndicator->setID("cheat-indicator"_spr);
                auto* container = m_fields->m_containers[config::get<int>("labels.cheat-indicator.alignment", 2)];
                container->addLabel(cheatIndicator, [](SmartLabel* label) {
                    label->setScale(config::get<float>("labels.cheat-indicator.scale", 0.5f));
                    bool isCheating = config::getTemp("hasCheats", false);
                    label->setColor(isCheating ? gui::Color::RED.toCCColor3B() : gui::Color::GREEN.toCCColor3B());
                    label->setOpacity(static_cast<GLubyte>(config::get<float>("labels.cheat-indicator.opacity", 0.35f) * 255));
                    label->setVisible(config::get<bool>("labels.cheat-indicator.visible", false));
                });
            }

            // Add the labels
            {
                size_t i = 0;
                for (auto& setting : s_labels) {
                    auto label = SmartLabel::create(setting.text, setting.font);
                    label->setID(fmt::format("label-{}", i++));
                    auto* container = m_fields->m_containers[static_cast<int>(setting.alignment)];
                    container->addLabel(label, [&setting](SmartLabel* label) {
                        label->setScript(setting.text);
                        label->setFntFile(setting.font.c_str());
                        label->setScale(setting.scale);
                        label->setColor(setting.color.toCCColor3B());
                        label->setOpacity(static_cast<GLubyte>(setting.color.a * 255));
                        label->setVisible(setting.visible);
                    });
                }
            }
        }

        void realignContainers(bool recreate = false) {
            if (recreate) {
                m_fields->m_mainContainer->removeAllChildren();
                createLabels();
            }

            updateLabels(0.f);
            for (auto& container : m_fields->m_containers) {
                container->updateLayout(false);
            }
        }

        void updateLabels(float) {
            bool visible = config::get<bool>("labels.visible", true);
            if (visible) labels::VariableManager::get().refetch();
            for (auto& container : m_fields->m_containers) {
                container->setVisible(visible);
                container->update();
            }
        }

        bool init(GJBaseGameLayer* bgl) {
            if (!UILayer::init(bgl)) return false;

            m_fields->m_mainContainer = cocos2d::CCNode::create();
            m_fields->m_mainContainer->setID("label-containers"_spr);

            createLabels();

            // Update layouts for containers
            geode::queueInMainThread([this]{
                updateLabels(0.f);
                for (auto& container : m_fields->m_containers) {
                    container->updateLayout(false);
                }
            });

            this->addChild(m_fields->m_mainContainer, 1000);
            this->schedule(schedule_selector(LabelsUILHook::updateLabels));

            return true;
        }
    };

    class Labels : public hack::Hack {
        static void updateLabels(bool recreate = false) {
            auto* gjbgl = GJBaseGameLayer::get();
            if (!gjbgl) return;

            auto* layer = gjbgl->m_uiLayer;
            if (!layer) return;

            auto* labelsLayer = reinterpret_cast<LabelsUILHook*>(layer);
            labelsLayer->realignContainers(recreate);
        }

        void init() override {
            auto tab = gui::MenuTab::find("Labels");
            config::setIfEmpty("labels.visible", true);
            config::setIfEmpty("labels.cheat-indicator.scale", 0.5f);
            config::setIfEmpty("labels.cheat-indicator.opacity", 0.35f);

            s_labels = config::get<std::vector<labels::LabelSettings>>("labels", {
                {"Testmode", "{isTestMode ? 'Testmode' : ''}", false},
                {"Attempt", "Attempt {attempt}", false},
                {"Percentage", "{isPlatformer ? time : progress + '%'}", false},
                {"Level Time", "{time}", false},
                {"Best Run", "Best run: {runFrom}-{bestRun}%", false},
                {"Clock", "{clock}", false},
                {"FPS", "FPS: {round(fps)}", false},
                {"CPS", "{cps1}/{cps2}/{cps3} CPS", false}, // TODO: Add click trigger
                {"Noclip Accuracy", "Accuracy: {noclipAccuracy}%", false}, // TODO: Add death trigger
                {"Noclip Deaths", "Deaths: {noclipDeaths}", false},
            });

            tab->addToggle("Show labels", "labels.visible")
                ->setDescription("Toggles the visibility of the labels.")
                ->handleKeybinds();
            tab->addToggle("Cheat indicator", "labels.cheat-indicator.visible")
                ->callback([](bool) { updateLabels(); })
                ->setDescription("Shows a red indicator if you have any cheats enabled.")
                ->handleKeybinds()
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                    options->addInputFloat("Scale", "labels.cheat-indicator.scale", 0.1f, 2.f, "%.1f")
                        ->setDescription("The scale of the cheat indicator.")
                        ->callback([](float) { updateLabels(); });
                    options->addInputFloat("Opacity", "labels.cheat-indicator.opacity", 0.f, 1.f, "%.2f")
                        ->setDescription("The opacity of the cheat indicator.");
                    options->addCombo("Alignment", "labels.cheat-indicator.alignment", {
                        "Top Left", "Top Center", "Top Right",
                        "Center Left", "Center", "Center Right",
                        "Bottom Left", "Bottom Center", "Bottom Right"
                    }, 2)->callback([](int) { updateLabels(true); })
                         ->setDescription("The alignment of the cheat indicator.");
                });

            tab->addButton("Add new")->callback([this]{
                labels::LabelSettings newSetting;
                if (!s_labels.empty()) {
                    // Copy some settings from existing labels
                    auto& last = s_labels.back();
                    newSetting.color = last.color;
                    newSetting.font = last.font;
                    newSetting.scale = last.scale;
                    newSetting.alignment = last.alignment;
                }
                newSetting.name = fmt::format("New label {}", newSetting.id);
                s_labels.push_back(newSetting);
                config::set("labels", s_labels);
                updateLabels(true);
                createLabelComponent();
            });

            createLabelComponent();
        }

        void update() override {
            auto& manager = labels::VariableManager::get();

            // call this once per frame
            manager.updateFPS();

            // player 1
            {
                s_clicksP1.cleanup();
                const auto jumpTotal = s_clicksP1.getClicks(PlayerButton::Jump);
                const auto leftTotal = s_clicksP1.getClicks(PlayerButton::Left);
                const auto rightTotal = s_clicksP1.getClicks(PlayerButton::Right);
                const auto jumpCPS = s_clicksP1.getCPS(PlayerButton::Jump);
                const auto leftCPS = s_clicksP1.getCPS(PlayerButton::Left);
                const auto rightCPS = s_clicksP1.getCPS(PlayerButton::Right);
                manager.setVariable("cps1", rift::Value::from(jumpCPS));
                manager.setVariable("cps2", rift::Value::from(leftCPS));
                manager.setVariable("cps3", rift::Value::from(rightCPS));
                manager.setVariable("clicks1", rift::Value::from(jumpTotal));
                manager.setVariable("clicks2", rift::Value::from(leftTotal));
                manager.setVariable("clicks3", rift::Value::from(rightTotal));
                manager.setVariable("cps", rift::Value::from(jumpCPS + leftCPS + rightCPS));
                manager.setVariable("clicks", rift::Value::from(jumpTotal + leftTotal + rightTotal));
            }

            // player 2
            {
                s_clicksP2.cleanup();
                const auto jumpTotal = s_clicksP2.getClicks(PlayerButton::Jump);
                const auto leftTotal = s_clicksP2.getClicks(PlayerButton::Left);
                const auto rightTotal = s_clicksP2.getClicks(PlayerButton::Right);
                const auto jumpCPS = s_clicksP2.getCPS(PlayerButton::Jump);
                const auto leftCPS = s_clicksP2.getCPS(PlayerButton::Left);
                const auto rightCPS = s_clicksP2.getCPS(PlayerButton::Right);
                manager.setVariable("cps1P2", rift::Value::from(jumpCPS));
                manager.setVariable("cps2P2", rift::Value::from(leftCPS));
                manager.setVariable("cps3P2", rift::Value::from(rightCPS));
                manager.setVariable("clicks1P2", rift::Value::from(jumpTotal));
                manager.setVariable("clicks2P2", rift::Value::from(leftTotal));
                manager.setVariable("clicks3P2", rift::Value::from(rightTotal));
                manager.setVariable("cpsP2", rift::Value::from(jumpCPS + leftCPS + rightCPS));
                manager.setVariable("clicksP2", rift::Value::from(jumpTotal + leftTotal + rightTotal));
            }
        }

        [[nodiscard]] const char* getId() const override { return "Labels"; }

        void createLabelComponent() {
            auto tab = gui::MenuTab::find("Labels");
            for (const auto& toggle : m_labelToggles) {
                tab->removeComponent(toggle);
            }

            m_labelToggles.clear();
            for (auto& setting : s_labels) {
                auto toggle = tab->addLabelSetting(&setting);
                toggle->deleteCallback([this, &setting] {
                    auto it = std::find_if(s_labels.begin(), s_labels.end(), [&setting](const labels::LabelSettings& s) {
                        return s.id == setting.id;
                    });

                    if (it == s_labels.end()) return;

                    s_labels.erase(it);
                    config::set("labels", s_labels);
                    updateLabels(true);
                    createLabelComponent();
                })
                ->editCallback([]{
                    config::set("labels", s_labels);
                    updateLabels(true);
                });

                m_labelToggles.push_back(toggle);
            }
        }

        std::vector<std::shared_ptr<gui::LabelSettingsComponent>> m_labelToggles;

    };

    REGISTER_HACK(Labels)
}
