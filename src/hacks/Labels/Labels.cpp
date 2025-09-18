#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/button.hpp>
#include <modules/gui/components/combo.hpp>
#include <modules/gui/components/input-float.hpp>
#include <modules/gui/components/label-settings.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/i18n/translations.hpp>

#include <modules/labels/setting.hpp>
#include <modules/labels/variables.hpp>

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <modules/gui/cocos/cocos.hpp>

#include "Label.hpp"

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
        int jumpMaxCPS;
        int leftMaxCPS;
        int rightMaxCPS;

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

            updateMaxCPS(PlayerButton::Jump);
            updateMaxCPS(PlayerButton::Left);
            updateMaxCPS(PlayerButton::Right);
        }

        void resetOnDeath() {
            jumpClicks.second = 0;
            leftClicks.second = 0;
            rightClicks.second = 0;
            jumpMaxCPS = 0;
            leftMaxCPS = 0;
            rightMaxCPS = 0;
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
                default: return;
            }

            clickInfo->first.push_back(getTimestamp());
            clickInfo->second++;
        }

        int getCPS(const PlayerButton btn) const {
            switch (btn) {
                case PlayerButton::Jump: return jumpClicks.first.size();
                case PlayerButton::Left: return leftClicks.first.size();
                case PlayerButton::Right: return rightClicks.first.size();
                default: return 0;
            }
        }

        int getMaxCPS(const PlayerButton btn) const {
            switch (btn) {
                case PlayerButton::Jump: return jumpMaxCPS;
                case PlayerButton::Left: return leftMaxCPS;
                case PlayerButton::Right: return rightMaxCPS;
                default: return 0;
            }
        }

        void updateMaxCPS(const PlayerButton btn) {
            auto cps = getCPS(btn);
            switch (btn) {
                case PlayerButton::Jump:
                    jumpMaxCPS = std::max(cps, jumpMaxCPS);
                    break;
                case PlayerButton::Left:
                    leftMaxCPS = std::max(cps, leftMaxCPS);
                    break;
                case PlayerButton::Right:
                    rightMaxCPS = std::max(cps, rightMaxCPS);
                    break;
                default: break;
            }
        }

        int getClicks(const PlayerButton btn) const {
            switch (btn) {
                case PlayerButton::Jump: return jumpClicks.second;
                case PlayerButton::Left: return leftClicks.second;
                case PlayerButton::Right: return rightClicks.second;
                default: return 0;
            }
        }
    };

    static ClickStorage s_clicksP1;
    static ClickStorage s_clicksP2;

    class $modify(LabelsPOHook, PlayerObject) {
        bool pushButton(PlayerButton btn) {
            bool result = PlayerObject::pushButton(btn);

            const auto* gjbgl = utils::get<GJBaseGameLayer>();
            if (!gjbgl) return result;

            const bool isP1 = this == gjbgl->m_player1;
            const bool isP2 = this == gjbgl->m_player2;

            if (isP1)
                s_clicksP1.addClick(btn);
            else if (isP2)
                s_clicksP2.addClick(btn);

            return result;
        }
    };

    class $modify(LabelsPLHook, PlayLayer) {
        bool init(GJGameLevel* level, bool unk1, bool unk2) {
            if (!PlayLayer::init(level, unk1, unk2)) return false;

            // removes the testmode label (thank you mat for this :D)
            if (this->getChildrenCount()) {
                geode::cocos::CCArrayExt<cocos2d::CCNode*> children = this->getChildren();
                for (auto* child : children) {
                    using namespace std::literals::string_view_literals;
                    if (auto* label = geode::cast::typeinfo_cast<cocos2d::CCLabelBMFont*>(child); label && label->getString() == "Testmode"sv) {
                        label->setVisible(false);
                        break;
                    }
                }
            }

            return true;
        }

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
            std::vector<std::pair<SmartLabel*, std::function<void(SmartLabel*)>>> m_absoluteLabels;
            bool m_isEditor = false;
        };

        void createLabels() {
            auto* fields = m_fields.self();

            // Create the containers
            auto alignment = LabelsContainer::Alignment::TopLeft;
            for (auto& container : fields->m_containers) {
                container = LabelsContainer::create(alignment);
                container->setID(fmt::format("label-container-{}", static_cast<int>(alignment)));
                fields->m_mainContainer->addChild(container);
                alignment = static_cast<LabelsContainer::Alignment>(static_cast<int>(alignment) + 1);
            }

            // Add the cheat indicator
            {
                auto* cheatIndicator = SmartLabel::create(".", "bigFont.fnt");
                if (!cheatIndicator) {
                    geode::log::error("Failed to create cheat indicator label");
                    return;
                }
                cheatIndicator->setHeightMultiplier(0.37f);
                cheatIndicator->setID("cheat-indicator"_spr);
                auto* container = fields->m_containers[config::get<int>("labels.cheat-indicator.alignment", 0)];
                container->addLabel(cheatIndicator, [](SmartLabel* label) {
                    bool visible = config::get<bool>("labels.cheat-indicator.visible", false);

                    if (!visible) {
                        label->setVisible(false);
                        return;
                    }

                    bool isCheating = config::getTemp("hasCheats", false);
                    bool hasTripped = config::getTemp("trippedSafeMode", false);
                    bool showOnlyCheating = config::get<bool>("labels.cheat-indicator.only-cheating", false);

                    if (showOnlyCheating && !(isCheating || hasTripped)) {
                        label->setVisible(false);
                        return;
                    }

                    label->setVisible(true);
                    label->setScale(config::get<float>("labels.cheat-indicator.scale", 0.5f));
                    label->setOpacity(
                        static_cast<GLubyte>(config::get<float>("labels.cheat-indicator.opacity", 0.35f) * 255)
                    );

                    // Cheating - Red, Tripped - Orange, Normal - Green
                    auto color = isCheating ? gui::Color::RED : hasTripped
                                            ? gui::Color{0.72f, 0.37f, 0.f}
                                            : gui::Color::GREEN;
                    label->setColor(color.toCCColor3B());
                });
            }

            // Add the labels
            {
                size_t i = 0;
                auto winSize = utils::get<cocos2d::CCDirector>()->getWinSize();
                for (auto& setting : s_labels) {
                    if (!setting.visible && !setting.hasEvents()) continue; // Skip invisible labels

                    auto label = SmartLabel::create(setting.text, setting.font);
                    label->setID(fmt::format("label-{}", i++));
                    label->setScale(setting.scale);
                    label->setColor(setting.color.toCCColor3B());
                    label->setOpacity(setting.color.getAlphaByte());
                    label->setVisible(setting.visible);
                    label->setAlignment(setting.fontAlignment);

                    if (setting.absolutePosition) {
                        auto offset = setting.offset;
                        switch (setting.alignment) {
                            case LabelsContainer::Alignment::TopLeft:
                                label->setPosition(offset.x, winSize.height - offset.y);
                                label->setAnchorPoint({0, 1});
                                break;
                            case LabelsContainer::Alignment::TopCenter:
                                label->setPosition(winSize.width / 2 + offset.x, winSize.height - offset.y);
                                label->setAnchorPoint({0.5, 1});
                                break;
                            case LabelsContainer::Alignment::TopRight:
                                label->setPosition(winSize.width - offset.x, winSize.height - offset.y);
                                label->setAnchorPoint({1, 1});
                                break;
                            case LabelsContainer::Alignment::CenterLeft:
                                label->setPosition(offset.x, winSize.height / 2 - offset.y);
                                label->setAnchorPoint({0, 0.5});
                                break;
                            case LabelsContainer::Alignment::Center:
                                label->setPosition(winSize.width / 2 + offset.x, winSize.height / 2 - offset.y);
                                label->setAnchorPoint({0.5, 0.5});
                                break;
                            case LabelsContainer::Alignment::CenterRight:
                                label->setPosition(winSize.width - offset.x, winSize.height / 2 - offset.y);
                                label->setAnchorPoint({1, 0.5});
                                break;
                            case LabelsContainer::Alignment::BottomLeft:
                                label->setPosition(offset.x, offset.y);
                                label->setAnchorPoint({0, 0});
                                break;
                            case LabelsContainer::Alignment::BottomCenter:
                                label->setPosition(winSize.width / 2 + offset.x, offset.y);
                                label->setAnchorPoint({0.5, 0});
                                break;
                            case LabelsContainer::Alignment::BottomRight:
                                label->setPosition(winSize.width - offset.x, offset.y);
                                label->setAnchorPoint({1, 0});
                                break;
                        }

                        fields->m_absoluteLabels.emplace_back(label, [&setting](SmartLabel* label) {
                            if (setting.hasEvents()) {
                                auto [visible, scale, color, font] = setting.processEvents();
                                label->setFont(font);
                                label->setScale(scale);
                                label->setColor(color.toCCColor3B());
                                label->setOpacity(color.getAlphaByte());
                                label->setVisible(visible);
                            }

                            label->update();
                        });

                        fields->m_mainContainer->addChild(label);
                    } else {
                        auto* container = fields->m_containers[static_cast<int>(setting.alignment)];
                        container->addLabel(label, [&setting](SmartLabel* label) {
                            if (setting.hasEvents()) {
                                auto [visible, scale, color, font] = setting.processEvents();
                                label->setFont(font);
                                label->setScale(scale);
                                label->setColor(color.toCCColor3B());
                                label->setOpacity(color.getAlphaByte());
                                label->setVisible(visible);
                            }
                        });
                    }
                }
            }
        }

        void realignContainers(bool recreate = false) {
            auto fields = m_fields.self();
            if (recreate) {
                fields->m_mainContainer->removeAllChildren();
                fields->m_absoluteLabels.clear();
                createLabels();
            }

            updateLabels(0.f);
            for (auto& container : m_fields->m_containers) {
                container->updateLayout(false);
            }
        }

        void updateLabels(float) {
            auto visible = config::get<"labels.visible", bool>(true);
            auto showInEditor = config::get<"labels.show-in-editor", bool>();

            auto fields = m_fields.self();
            bool actualVisibility = visible && (showInEditor || !fields->m_isEditor);

            if (actualVisibility) labels::VariableManager::get().refetch();

            for (auto& container : fields->m_containers) {
                container->setVisible(actualVisibility);
                container->update();
            }
            for (auto& [label, update] : fields->m_absoluteLabels) {
                label->setVisible(actualVisibility);
                if (actualVisibility) update(label);
            }
        }

        bool init(GJBaseGameLayer* bgl) {
            if (!UILayer::init(bgl)) return false;

            auto fields = m_fields.self();
            fields->m_isEditor = geode::cast::typeinfo_cast<LevelEditorLayer*>(bgl) != nullptr;
            fields->m_mainContainer = cocos2d::CCNode::create();
            fields->m_mainContainer->setID("label-containers"_spr);

            createLabels();

            // Update layouts for containers
            geode::queueInMainThread([this] {
                updateLabels(0.f);
                for (auto& container : m_fields->m_containers) {
                    container->updateLayout(false);
                }
            });

            this->addChild(fields->m_mainContainer, 1000);
            this->schedule(schedule_selector(LabelsUILHook::updateLabels));

            return true;
        }
    };

    class $hack(Labels) {
        static void updateLabels(bool recreate = false) {
            auto* layer = utils::get<UILayer>();
            if (!layer) return;

            auto* labelsLayer = reinterpret_cast<LabelsUILHook*>(layer);
            labelsLayer->realignContainers(recreate);
        }

        static void refreshCocosUI() {
            if (auto cocos = gui::cocos::CocosRenderer::get()) {
                cocos->refreshPage();
            }
        }

        static const std::vector<labels::LabelSettings> DEFAULT_LABELS;

        void init() override {
            auto tab = gui::MenuTab::find("tab.labels");
            config::setIfEmpty("labels.visible", true);
            config::setIfEmpty("labels.cheat-indicator.endscreen", true);
            config::setIfEmpty("labels.cheat-indicator.scale", 0.5f);
            config::setIfEmpty("labels.cheat-indicator.opacity", 0.35f);

            s_labels = config::get<std::vector<labels::LabelSettings>>("labels", DEFAULT_LABELS);

            tab->addToggle("labels.visible")
               ->callback([](bool) { updateLabels(); })
               ->setDescription()
               ->handleKeybinds();
            tab->addToggle("labels.show-in-editor")
               ->callback([](bool) { updateLabels(); })
               ->setDescription()
               ->handleKeybinds();
            tab->addToggle("labels.cheat-indicator.visible")
               ->callback([](bool) { updateLabels(); })
               ->setDescription()
               ->handleKeybinds()
               ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                   options->addToggle("labels.cheat-indicator.endscreen")
                          ->setDescription();
                   options->addToggle("labels.cheat-indicator.only-cheating")
                          ->setDescription();
                   options->addInputFloat("labels.cheat-indicator.scale", 0.1f, 2.f, "%.1f")
                          ->setDescription()
                          ->callback([](float) { updateLabels(); });
                   options->addInputFloat("labels.cheat-indicator.opacity", 0.f, 1.f, "%.2f")
                          ->setDescription();
                   options->addCombo("labels.cheat-indicator.alignment", {
                              i18n::get_("labels.alignment.top-left"),
                              i18n::get_("labels.alignment.top-center"),
                              i18n::get_("labels.alignment.top-right"),
                              i18n::get_("labels.alignment.center-left"),
                              i18n::get_("labels.alignment.center"),
                              i18n::get_("labels.alignment.center-right"),
                              i18n::get_("labels.alignment.bottom-left"),
                              i18n::get_("labels.alignment.bottom-center"),
                              i18n::get_("labels.alignment.bottom-right"),
                          }, 2
                      )->callback([](int) { updateLabels(true); })
                      ->setDescription();
               });
            tab->addButton("labels.import")->callback([this] {
                using FileEvent = geode::Task<geode::Result<std::filesystem::path>>;
                static geode::EventListener<FileEvent> s_listener;
                geode::utils::file::FilePickOptions::Filter filter;
                filter.description = "Eclipse Label (*.ecl)";
                filter.files.insert("*.ecl");
                s_listener.bind([this](FileEvent::Event* event) {
                    if (auto value = event->getValue()) {
                        auto path = value->unwrapOr("");
                        std::error_code ec;
                        if (path.empty() || !std::filesystem::exists(path, ec))
                            return;

                        gui::Engine::queueAfterDrawing([this, path] {
                            std::ifstream file(path);

                            nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
                            file.close();

                            if (json.is_discarded()) {
                                return Popup::create(
                                    i18n::get_("labels.import-failed"),
                                    i18n::get_("labels.import-failed.msg")
                                );
                            }

                            s_labels.emplace_back(json.get<labels::LabelSettings>());
                            config::set("labels", s_labels);
                            updateLabels(true);
                            createLabelComponent();
                        });
                    }
                });

                s_listener.setFilter(
                    geode::utils::file::pick(
                        geode::utils::file::PickMode::OpenFile,
                        {geode::Mod::get()->getSaveDir(), {filter}}
                    )
                );
            });
            tab->addButton("labels.add-new")->callback([this] {
                gui::Engine::queueAfterDrawing([this] {
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
            });
            std::vector<std::string> presets;
            presets.reserve(DEFAULT_LABELS.size());
            for (const auto& preset : DEFAULT_LABELS) {
                presets.push_back(preset.name);
            }
            tab->addCombo("labels.presets", presets, 0)
               ->callback([this](int value) {
                   if (value < 0 || value >= DEFAULT_LABELS.size()) return;
                   gui::Engine::queueAfterDrawing([this, value] {
                       auto preset = DEFAULT_LABELS[value];
                       preset.id = labels::LabelSettings::instanceCount++;
                       preset.visible = true;
                       s_labels.push_back(std::move(preset));
                       config::set("labels", s_labels);
                       updateLabels(true);
                       createLabelComponent();
                   });
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
                const auto maxJumpCPS = s_clicksP1.getMaxCPS(PlayerButton::Jump);
                const auto maxLeftCPS = s_clicksP1.getMaxCPS(PlayerButton::Left);
                const auto maxRightCPS = s_clicksP1.getMaxCPS(PlayerButton::Right);
                manager.setVariable("cps1", jumpCPS);
                manager.setVariable("cps2", leftCPS);
                manager.setVariable("cps3", rightCPS);
                manager.setVariable("clicks1", jumpTotal);
                manager.setVariable("clicks2", leftTotal);
                manager.setVariable("clicks3", rightTotal);
                manager.setVariable("maxCps1", maxJumpCPS);
                manager.setVariable("maxCps2", maxLeftCPS);
                manager.setVariable("maxCps3", maxRightCPS);
                manager.setVariable("cps", jumpCPS + leftCPS + rightCPS);
                manager.setVariable("clicks", jumpTotal + leftTotal + rightTotal);
                manager.setVariable("maxCps", maxJumpCPS + maxLeftCPS + maxRightCPS);
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
                const auto maxJumpCPS = s_clicksP2.getMaxCPS(PlayerButton::Jump);
                const auto maxLeftCPS = s_clicksP2.getMaxCPS(PlayerButton::Left);
                const auto maxRightCPS = s_clicksP2.getMaxCPS(PlayerButton::Right);
                manager.setVariable("cps1P2", jumpCPS);
                manager.setVariable("cps2P2", leftCPS);
                manager.setVariable("cps3P2", rightCPS);
                manager.setVariable("clicks1P2", jumpTotal);
                manager.setVariable("clicks2P2", leftTotal);
                manager.setVariable("clicks3P2", rightTotal);
                manager.setVariable("maxCps1P2", maxJumpCPS);
                manager.setVariable("maxCps2P2", maxLeftCPS);
                manager.setVariable("maxCps3P2", maxRightCPS);
                manager.setVariable("cpsP2", jumpCPS + leftCPS + rightCPS);
                manager.setVariable("clicksP2", jumpTotal + leftTotal + rightTotal);
                manager.setVariable("maxCpsP2", maxJumpCPS + maxLeftCPS + maxRightCPS);
            }
        }

        [[nodiscard]] const char* getId() const override { return "Labels"; }

        void createLabelComponent() {
            auto tab = gui::MenuTab::find("tab.labels");
            for (const auto& toggle : m_labelToggles) {
                tab->removeComponent(toggle);
            }

            m_labelToggles.clear();
            for (auto& setting : s_labels) {
                auto toggle = tab->addLabelSetting(&setting);
                toggle->deleteCallback([this, &setting] {
                          Popup::create(
                              i18n::get_("labels.delete-prompt"),
                              i18n::get_("labels.delete-prompt.msg"),
                              i18n::get_("common.yes"),
                              i18n::get_("common.no"),
                              [this, &setting](bool yes) {
                                  if (!yes) return;
                                  gui::Engine::queueAfterDrawing(
                                      [&] {
                                          auto it = std::ranges::find_if(
                                              s_labels, [&setting](const labels::LabelSettings& s) {
                                                  return s.id == setting.id;
                                              }
                                          );

                                          if (it == s_labels.end()) return;

                                          s_labels.erase(it);
                                          config::set("labels", s_labels);
                                          updateLabels(true);
                                          createLabelComponent();
                                      }
                                  );
                              }
                          );
                      })
                      ->editCallback([] {
                          config::set("labels", s_labels);
                          updateLabels(true);
                      })
                      ->exportCallback([&setting] {
                          setting.promptSave();
                      })
                      ->moveCallback([this, &setting](bool up) {
                          auto it = std::ranges::find_if(
                              s_labels, [&setting](const labels::LabelSettings& s) {
                                  return s.id == setting.id;
                              }
                          );

                          if (it == s_labels.end())
                              return; // should never happen but just in case
                          if ((up && it == s_labels.begin()) || (!up && it == s_labels.end() - 1))
                              return; // index out of bounds

                          // swap the elements
                          auto index = std::distance(s_labels.begin(), it);
                          auto newIndex = up ? index - 1 : index + 1;
                          std::iter_swap(it, s_labels.begin() + newIndex);

                          // update the config
                          config::set("labels", s_labels);
                          updateLabels(true);

                          // refresh ui
                          gui::Engine::queueAfterDrawing([this] {
                              createLabelComponent();
                          });
                      })
                      ->handleKeybinds();

                m_labelToggles.push_back(toggle);
            }

            refreshCocosUI();
        }

        std::vector<std::shared_ptr<gui::LabelSettingsComponent>> m_labelToggles;
    };

    const std::vector<labels::LabelSettings> Labels::DEFAULT_LABELS = {
        {"FPS", "FPS: {round(fps)}", false},
        {"Testmode", "{isPracticeMode ? emojis.practice + 'Practice' : isTestMode ?? emojis.startPos + 'Testmode'}", false},
        {"Run From", "{!isPlatformer && (isPracticeMode || isTestMode) ?? 'From: ' + floor(runStart) + '%'}", false},
        {"Attempt", "Attempt {attempt}", false},
        {"Percentage", "{isPlatformer ? time : progress + '%'}", false},
        {"Level Time", "{time}", false},
        {"Best Run", "Best run: {runFrom}-{bestRun}%", false},
        {"Clock", "{clock}", false},
        {
            "CPS", "{cps}/{clicks}/{maxCps} CPS", false,
            false, 0.35f, gui::Color(1.f, 1.f, 1.f, 0.3f),
            "bigFont.fnt", LabelsContainer::Alignment::TopLeft,
            BMFontAlignment::Left, {0, 0}, {{
                true, labels::LabelEvent::Type::OnButtonHold,
                "", std::nullopt, std::nullopt,
                gui::Color(0, 1, 0), 0.6f, std::nullopt,
                0.f, 0.f, 0.1f
            }}
        },
        {
            "Noclip Accuracy", "{ noclip ?? $'Accuracy: {noclipAccuracy}%'}", false,
            false, 0.35f, gui::Color(1.f, 1.f, 1.f, 0.3f),
            "bigFont.fnt", LabelsContainer::Alignment::TopLeft,
            BMFontAlignment::Left, {0, 0}, {{
                true, labels::LabelEvent::Type::OnNoclipDeath,
                "", std::nullopt, std::nullopt,
                gui::Color(1, 0, 0), 0.6f, std::nullopt,
                0.f, 0.f, 0.1f
            }}
        },
        {
            "Noclip Deaths", "{ noclip ?? 'Deaths: ' + noclipDeaths}", false,
            false, 0.35f, gui::Color(1.f, 1.f, 1.f, 0.3f),
            "bigFont.fnt", LabelsContainer::Alignment::TopLeft,
            BMFontAlignment::Left, {0, 0}, {{
                true, labels::LabelEvent::Type::OnNoclipDeath,
                "", std::nullopt, std::nullopt,
                gui::Color(1, 0, 0), 0.6f, std::nullopt,
                0.f, 0.f, 0.1f
            }}
        },
    };



    REGISTER_HACK(Labels)
}
