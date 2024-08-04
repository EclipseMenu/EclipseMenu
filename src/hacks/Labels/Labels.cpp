#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <modules/labels/variables.hpp>
#include <modules/labels/setting.hpp>

#include <Geode/modify/UILayer.hpp>
#include <utility>

namespace eclipse::hacks::Labels {

    static std::vector<labels::LabelSettings> s_labels;

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
            // call this once per frame
            labels::VariableManager::get().updateFPS();
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
