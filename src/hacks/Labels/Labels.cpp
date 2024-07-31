/*#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/labels/variables.hpp>

#include "LabelContainer.hpp"

#include <Geode/modify/UILayer.hpp>
#include <utility>

namespace eclipse::hacks::Labels {

    /// @brief Settings for a label to store in the config.
    class LabelSettings {
    public:
        LabelsContainer::Alignment alignment = LabelsContainer::Alignment::TopLeft;
        std::string text;
        std::string font = "bigFont.fnt";
        float scale = 0.6f;
        gui::Color color = gui::Color::WHITE;
        bool visible = true;
    };

    void from_json(const nlohmann::json& json, LabelSettings& settings) {
        settings.text = json.value("text", "");
        settings.font = json.value("font", "bigFont.fnt");
        settings.scale = json.value("scale", 0.6f);
        settings.color = json.value("color", gui::Color::WHITE);
        settings.visible = json.value("visible", true);
        settings.alignment = static_cast<LabelsContainer::Alignment>(json.value("alignment", 0));
    }

    void to_json(nlohmann::json& json, const LabelSettings& settings) {
        json = nlohmann::json{
            {"text", settings.text},
            {"font", settings.font},
            {"scale", settings.scale},
            {"color", settings.color},
            {"visible", settings.visible},
            {"alignment", static_cast<int>(settings.alignment)}
        };
    }

    class $modify(LabelsUILHook, UILayer) {
        struct Fields {
            std::array<LabelsContainer*, 9> m_containers;
        };

        void realignContainers() {
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

            cocos2d::CCNode* containers = cocos2d::CCNode::create();
            containers->setID("label-containers"_spr);

            // Create the containers
            LabelsContainer::Alignment alignment = LabelsContainer::Alignment::TopLeft;
            for (auto& container : m_fields->m_containers) {
                container = LabelsContainer::create(alignment);
                container->setID(fmt::format("label-container-{}", static_cast<int>(alignment)));
                containers->addChild(container);
                alignment = static_cast<LabelsContainer::Alignment>(static_cast<int>(alignment) + 1);
            }

            // Add the cheat indicator
            {
                geode::log::debug("Creating cheat indicator");
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
            auto settings = config::get<std::vector<LabelSettings>>("labels", {});
            size_t i = 0;
            for (auto& setting : settings) {
                geode::log::debug("Creating label: {}", setting.text);
                auto label = SmartLabel::create(setting.text, setting.font);
                label->setID(fmt::format("label-{}", i++));
                auto* container = m_fields->m_containers[static_cast<int>(setting.alignment)];
                container->addLabel(label, [setting](SmartLabel* label) {
                    label->setScript(setting.text);
                    label->setFntFile(setting.font.c_str());
                    label->setScale(setting.scale);
                    label->setColor(setting.color.toCCColor3B());
                    label->setOpacity(static_cast<GLubyte>(setting.color.a * 255));
                    label->setVisible(setting.visible);
                });
            }

            // Update layouts for containers
            geode::queueInMainThread([this]{
                updateLabels(0.f);
                for (auto& container : m_fields->m_containers) {
                    container->updateLayout(false);
                }
            });

            this->addChild(containers, 1000);
            this->schedule(schedule_selector(LabelsUILayer::updateLabels));

            return true;
        }
    };

    class Labels : public hack::Hack {
        static void updateLabels() {
            auto* gjbgl = GJBaseGameLayer::get();
            if (!gjbgl) return;

            auto* layer = gjbgl->m_uiLayer;
            if (!layer) return;

            auto* labelsLayer = static_cast<LabelsUILayer*>(layer);
            labelsLayer->realignContainers();
        }

        void init() override {
            auto tab = gui::MenuTab::find("Labels");
            config::setIfEmpty("labels.visible", true);
            config::setIfEmpty("labels.cheat-indicator.scale", 0.5f);
            config::setIfEmpty("labels.cheat-indicator.opacity", 0.35f);

            config::set<std::vector<LabelSettings>>("labels", {
                {LabelsContainer::Alignment::TopLeft, "Hello, world!", "bigFont.fnt", 0.6f, gui::Color(1.f, 1.f, 1.f, 0.6f)},
                {LabelsContainer::Alignment::TopCenter, "Funny clock: {clock} - {clock12}", "goldFont.fnt", 0.4f},
                {LabelsContainer::Alignment::TopRight, "FPS: {fps}", "chatFont.fnt"},
                {LabelsContainer::Alignment::CenterLeft, "{isPlatformer ? 'in platformer' : 'in normal level'}", "bigFont.fnt", 0.5f},
                {LabelsContainer::Alignment::Center, "{levelName} by {author}", "bigFont.fnt", 0.5f},
                {LabelsContainer::Alignment::CenterRight, "Stars: {levelStars}", "bigFont.fnt", 0.5f},
                {LabelsContainer::Alignment::BottomLeft, "Random number: {random(0, 100)}", "bigFont.fnt", 0.5f},
                {LabelsContainer::Alignment::BottomCenter, "I love math? {2 + 2 * 2 == 6 ? 'no' : 'yes'}", "bigFont.fnt", 0.5f},
                {LabelsContainer::Alignment::BottomRight, "{toUpper('this is written in caps')}", "gjFont14.fnt", 0.3f, gui::Color::RED}
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
                        ->setDescription("The scale of the cheat indicator.");
                    options->addInputFloat("Opacity", "labels.cheat-indicator.opacity", 0.f, 1.f, "%.2f")
                        ->setDescription("The opacity of the cheat indicator.");
                    options->addCombo("Alignment", "labels.cheat-indicator.alignment", {
                        "Top Left", "Top Center", "Top Right",
                        "Center Left", "Center", "Center Right",
                        "Bottom Left", "Bottom Center", "Bottom Right"
                    }, 2);
                });
        }

        [[nodiscard]] const char* getId() const override { return "Labels"; }
    };

    REGISTER_HACK(Labels)
}*/
