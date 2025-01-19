#include "label-settings-popup.hpp"

#include <hacks/Labels/Label.hpp>
#include <modules/gui/cocos/cocos.hpp>
#include <modules/gui/cocos/nodes/color-picker.hpp>
#include <modules/gui/cocos/nodes/FallbackBMFont.hpp>
#include <modules/gui/theming/manager.hpp>
#include <modules/i18n/translations.hpp>
#include <modules/labels/setting.hpp>

namespace eclipse::gui::cocos {
    constexpr GLuint DISABLED_OPACITY = 255 * 0.3f;

    class PopupTab : public CCMenuItemSpriteExtra {
    public:
        static PopupTab* create(const std::string& text, size_t page, LabelSettingsPopup* popup) {
            auto ret = new PopupTab();
            if (ret->init(text, page, popup)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        void setState(bool active) {
            m_background->setOpacity(active ? 255 : DISABLED_OPACITY);
            setEnabled(!active);
        }

    protected:
        void onClicked(CCObject*) {
            m_popup->selectTab(m_page);
        }

        bool init(const std::string& text, size_t page, LabelSettingsPopup* popup) {
            const auto tm = ThemeManager::get();

            m_background = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
            m_background->setContentSize({ 100.f, 32.f });
            m_background->setScale(0.7f);
            m_background->setColor(tm->getButtonActivatedBackground().toCCColor3B());

            static Label::EmojiMap s_emojis = {
                {U"⚙️", "settings.png"_spr},
                {U"⚡", "script_icon.png"_spr},
                {U"🕔", "event_icon.png"_spr},
                {U"👁️", "preview_icon.png"_spr},
            };
            static std::unordered_map<std::string, std::string_view> s_emojisMap = {
                {"labels.settings", "⚙️"},
                {"labels.text", "⚡"},
                {"labels.events", "🕔"},
                {"labels.preview", "👁️"},
            };

            auto label = TranslatedLabel::createRaw("");
            label->enableEmojiColors(true);
            label->enableEmojis("UISheet.png"_spr, &s_emojis);
            label->setString(fmt::format("{} {}", s_emojisMap[text], i18n::get(text)));
            label->limitLabelWidth(80, 1.f, 0.1f);
            label->setColor(tm->getButtonActivatedForeground().toCCColor3B());
            m_background->addChildAtPosition(label, geode::Anchor::Center);

            m_page = page;
            m_popup = popup;

            return CCMenuItemSpriteExtra::init(m_background, m_background, this, menu_selector(PopupTab::onClicked));
        }

    protected:
        LabelSettingsPopup* m_popup = nullptr;
        size_t m_page = 0;
        cocos2d::extension::CCScale9Sprite* m_background = nullptr;
    };

    bool LabelSettingsPopup::setup(labels::LabelSettings* settings, std::function<void(CallbackEvent)> const& callback) {
        const auto tm = ThemeManager::get();

        m_settings = settings;
        m_callback = callback;

        // The behind background for the entire popup to get the outline
        auto bgBehind = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
        bgBehind->setContentSize(m_mainLayer->getContentSize() * tm->getBorderSize());
        m_bgSprite->setColor(tm->getBorderColor().toCCColor3B());
        bgBehind->setID("bg-behind"_spr);
        m_mainLayer->addChildAtPosition(bgBehind, geode::Anchor::Center);

        // Background for the entire popup
        m_bgSprite = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
        m_bgSprite->setContentSize(m_mainLayer->getContentSize() - 3);
        m_bgSprite->setColor(tm->getTitleBackgroundColor().toCCColor3B());
        m_bgSprite->setID("main-bg"_spr);
        m_mainLayer->addChildAtPosition(m_bgSprite, geode::Anchor::Center);

        // Background for content
        m_contentBG = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
        m_contentBG->setAnchorPoint({0, 1});
        m_contentBG->setPosition(7.5f, 210.f);
        m_contentBG->setColor(tm->getBackgroundColor().toCCColor3B());
        //m_contentBG->setOpacity(128);
        m_contentBG->setContentSize({385.f, 200.f});
        m_contentBG->setID("content-bg"_spr);
        m_mainLayer->addChild(m_contentBG);

        auto tabsMenu = cocos2d::CCMenu::create();
        tabsMenu->setID("tabs-menu"_spr);
        tabsMenu->setContentSize({ 350.f, 28.f });
        tabsMenu->setAnchorPoint({ 0.5f, 0.5f });

        auto tab = PopupTab::create("labels.settings", 0, this);
        tab->setID("settings-tab"_spr);
        m_tabs.push_back(tab);
        tabsMenu->addChild(tab);

        tab = PopupTab::create("labels.text", 1, this);
        tab->setID("text-tab"_spr);
        m_tabs.push_back(tab);
        tabsMenu->addChild(tab);

        tab = PopupTab::create("labels.events", 2, this);
        tab->setID("events-tab"_spr);
        m_tabs.push_back(tab);
        tabsMenu->addChild(tab);

        tab = PopupTab::create("labels.preview", 3, this);
        tab->setID("preview-tab"_spr);
        m_tabs.push_back(tab);
        tabsMenu->addChild(tab);

        tabsMenu->setLayout(geode::RowLayout::create()->setAxisAlignment(geode::AxisAlignment::Even));
        m_mainLayer->addChildAtPosition(tabsMenu, geode::Anchor::Top, { 0.f, -16.f });
        
        this->selectTab(0);
        
        // Register the popup with the engine for cleanup
        CocosRenderer::get()->registerModal(this);

        return true;
    }

    void LabelSettingsPopup::onExit() {
        Popup::onExit();
        CocosRenderer::get()->unregisterModal(this);
    }

    cocos2d::CCSprite* createButton(cocos2d::CCSprite* inner, float scale = 0.4f) {
        const auto tm = ThemeManager::get();
        auto box = cocos2d::CCSprite::createWithSpriteFrameName("rectangle.png"_spr);
        box->setScale(scale);
        if (inner) {
            box->addChildAtPosition(inner, geode::Anchor::Center);
            inner->setColor(tm->getButtonActivatedForeground().toCCColor3B());
        }
        box->setColor(tm->getButtonActivatedBackground().toCCColor3B());
        box->setOpacity(DISABLED_OPACITY);
        return box;
    }

    cocos2d::CCSprite* createToggle(const char* innerFrameName, float scale = 0.4f) {
        const auto tm = ThemeManager::get();
        auto box = cocos2d::CCSprite::createWithSpriteFrameName("rectangle.png"_spr);
        box->setScale(scale);
        if (innerFrameName) {
            auto inner = cocos2d::CCSprite::createWithSpriteFrameName(innerFrameName);
            box->addChildAtPosition(inner, geode::Anchor::Center);
            inner->setColor(tm->getCheckboxCheckmarkColor().toCCColor3B());
        }
        box->setColor(tm->getCheckboxBackgroundColor().toCCColor3B());
        return box;
    }

    cocos2d::CCSprite* createButtonSprite(const char* frame, float scale = 0.4f, std::optional<float> innerScale = std::nullopt) {
        if (frame) {
            auto inner = cocos2d::CCSprite::createWithSpriteFrameName(frame);
            if (innerScale) { inner->setScale(*innerScale); }
            return createButton(inner, scale);
        }
        return createButton(nullptr, scale);
    }

    cocos2d::CCSprite* createAlignButton(BMFontAlignment align) {
        const char* frame = nullptr;
        switch (align) {
            case BMFontAlignment::Left: frame = "falign_left.png"_spr; break;
            case BMFontAlignment::Center: frame = "falign_center.png"_spr; break;
            case BMFontAlignment::Right: frame = "falign_right.png"_spr; break;
            default: break;
        }
        return createButtonSprite(frame, 0.5f, 1.5f);
    }

    cocos2d::CCSprite* createAlignButton(labels::LabelsContainer::Alignment align) {
        const char* frame = nullptr;
        switch (align) {
            default: break;
            case labels::LabelsContainer::Alignment::TopLeft:
            case labels::LabelsContainer::Alignment::TopRight:
            case labels::LabelsContainer::Alignment::BottomLeft:
            case labels::LabelsContainer::Alignment::BottomRight:
                frame = "align_topleft.png"_spr;
                break;
            case labels::LabelsContainer::Alignment::TopCenter:
            case labels::LabelsContainer::Alignment::CenterLeft:
            case labels::LabelsContainer::Alignment::CenterRight:
            case labels::LabelsContainer::Alignment::BottomCenter:
                frame = "align_midleft.png"_spr;
                break;
            case labels::LabelsContainer::Alignment::Center:
                frame = "align_middle.png"_spr;
                break;
        }

        auto inner = cocos2d::CCSprite::createWithSpriteFrameName(frame);
        inner->setScale(1.5f);
        switch (align) {
            default: break;
            case labels::LabelsContainer::Alignment::BottomRight:
                inner->setFlipY(true); [[fallthrough]];
            case labels::LabelsContainer::Alignment::TopRight:
            case labels::LabelsContainer::Alignment::CenterRight:
                inner->setFlipX(true);
                break;

            case labels::LabelsContainer::Alignment::BottomLeft:
                inner->setFlipY(true);
                break;

            case labels::LabelsContainer::Alignment::BottomCenter:
                inner->setFlipX(true); [[fallthrough]];
            case labels::LabelsContainer::Alignment::TopCenter:
                inner->setRotation(90);
                break;
        }

        return createButton(inner, 0.5f);
    }

    /// Helper function to create a grid of buttons with a single callback
    template <typename... Args>
    std::pair<cocos2d::CCMenu*, std::array<cocos2d::CCSprite*, sizeof...(Args)>> createCompactMenu(
        float width, std::function<void(CCMenuItemSpriteExtra*)> const& callback, Args... args
    ) {
        auto menu = cocos2d::CCMenu::create();
        menu->setContentSize({ width, 28.f });

        size_t index = 0;
        for (auto& button : { geode::cocos::CCMenuItemExt::createSpriteExtra(args, callback)... }) {
            button->setTag(index++);
            menu->addChild(button);
        }

        menu->setLayout(
            geode::RowLayout::create()
                ->setGap(5.f)
                ->setGrowCrossAxis(true)
                ->setCrossAxisOverflow(true)
        );

        return { menu, { args... } };
    }

    auto createLabel(const char* text) {
        auto label = TranslatedLabel::create(text);
        label->setAnchorPoint({0, 1});
        label->limitLabelWidth(110.f, 1.f, 0.1f);
        return label;
    }

    cocos2d::CCLayer* LabelSettingsPopup::createSettingsTab() {
        auto layer = CCLayer::create();

        auto [alignMenu, alignSprites] = createCompactMenu(
            100.f, [this](auto item) {
                auto tag = item->getTag();
                for (size_t i = 0; i < m_alignButtons.size(); ++i) {
                    m_alignButtons[i]->setOpacity(i == tag ? 255 : DISABLED_OPACITY);
                }
                m_settings->alignment = static_cast<labels::LabelsContainer::Alignment>(tag);
                m_callback(CallbackEvent::Update);
            },
            createAlignButton(labels::LabelsContainer::Alignment::TopLeft),
            createAlignButton(labels::LabelsContainer::Alignment::TopCenter),
            createAlignButton(labels::LabelsContainer::Alignment::TopRight),
            createAlignButton(labels::LabelsContainer::Alignment::CenterLeft),
            createAlignButton(labels::LabelsContainer::Alignment::Center),
            createAlignButton(labels::LabelsContainer::Alignment::CenterRight),
            createAlignButton(labels::LabelsContainer::Alignment::BottomLeft),
            createAlignButton(labels::LabelsContainer::Alignment::BottomCenter),
            createAlignButton(labels::LabelsContainer::Alignment::BottomRight)
        );
        m_alignButtons = std::move(alignSprites);
        m_alignButtons[static_cast<size_t>(m_settings->alignment)]->setOpacity(255);
        alignMenu->setID("align-menu"_spr);
        layer->addChildAtPosition(alignMenu, geode::Anchor::Center, { 124.f, 28.f });

        auto [fontAlignMenu, fontAlignSprites] = createCompactMenu(
            100.f, [this](auto item) {
                auto tag = item->getTag();
                for (size_t i = 0; i < m_fontAlignButtons.size(); ++i) {
                    m_fontAlignButtons[i]->setOpacity(i == tag ? 255 : DISABLED_OPACITY);
                }
                m_settings->fontAlignment = static_cast<BMFontAlignment>(tag);
                m_callback(CallbackEvent::Update);
            },
            createAlignButton(BMFontAlignment::Left),
            createAlignButton(BMFontAlignment::Center),
            createAlignButton(BMFontAlignment::Right)
        );
        m_fontAlignButtons = std::move(fontAlignSprites);
        m_fontAlignButtons[static_cast<size_t>(m_settings->fontAlignment)]->setOpacity(255);
        fontAlignMenu->setID("font-align-menu"_spr);
        layer->addChildAtPosition(fontAlignMenu, geode::Anchor::Center, { 124.f, -52.f });

        auto column1 = CCNode::create();

        column1->addChild(createLabel("labels.color"));
        column1->addChild(createLabel("labels.opacity"));
        column1->addChild(createLabel("labels.scale"));
        column1->addChild(createLabel("labels.font"));
        column1->addChild(createLabel("labels.absolute"));

        auto absoluteX = geode::TextInput::create(120.f, "0", "font_default.fnt"_spr);
        absoluteX->setCommonFilter(geode::CommonFilter::Float);
        absoluteX->setString(std::to_string(m_settings->offset.x));
        absoluteX->setCallback([this](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            m_settings->offset.x = res.unwrap();
            m_callback(CallbackEvent::Update);
        });
        column1->addChild(absoluteX);

        column1->setID("column1"_spr);
        column1->setContentSize({120.f, 190.f});
        column1->setAnchorPoint({0, 0.5f});
        column1->setLayout(
            geode::ColumnLayout::create()
                ->setAxisReverse(true)
                ->setAxisAlignment(geode::AxisAlignment::Even)
                ->setCrossAxisAlignment(geode::AxisAlignment::Start)
                ->setCrossAxisLineAlignment(geode::AxisAlignment::Start)
                ->setAutoScale(false)
        );
        layer->addChildAtPosition(column1, geode::Anchor::Center, { -180.f, 0.f });

        auto column2 = cocos2d::CCMenu::create();

        column2->addChild(ColorPicker::create(m_settings->color, false, [this](gui::Color const& color) {
            m_settings->color = { color.r, color.g, color.b, m_settings->color.a };
            m_callback(CallbackEvent::Update);
        }));

        auto opacityInput = geode::TextInput::create(120.f, "0.750000", "font_default.fnt"_spr);
        opacityInput->setCommonFilter(geode::CommonFilter::Float);
        opacityInput->setString(std::to_string(m_settings->color.a));
        opacityInput->setCallback([this](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            m_settings->color.a = res.unwrap();
            m_callback(CallbackEvent::Update);
        });
        column2->addChild(opacityInput);

        auto scaleInput = geode::TextInput::create(120.f, "0.300000", "font_default.fnt"_spr);
        scaleInput->setCommonFilter(geode::CommonFilter::Float);
        scaleInput->setString(std::to_string(m_settings->scale));
        scaleInput->setCallback([this](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            m_settings->scale = res.unwrap();
            m_callback(CallbackEvent::Update);
        });
        column2->addChild(scaleInput);

        // TODO: Implement font selection
        column2->addChild(createLabel("<Insert Font Picker Here>"));

        auto absolutePosToggle = geode::cocos::CCMenuItemExt::createToggler(
            createToggle("checkmark.png"_spr), createToggle(nullptr), [this](auto) {
            m_settings->absolutePosition = !m_settings->absolutePosition;
            m_callback(CallbackEvent::Update);
        });
        absolutePosToggle->toggle(m_settings->absolutePosition);
        column2->addChild(absolutePosToggle);

        auto absoluteY = geode::TextInput::create(120.f, "0", "font_default.fnt"_spr);
        absoluteY->setCommonFilter(geode::CommonFilter::Float);
        absoluteY->setString(std::to_string(m_settings->offset.y));
        absoluteY->setCallback([this](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            m_settings->offset.y = res.unwrap();
            m_callback(CallbackEvent::Update);
        });
        column2->addChild(absoluteY);

        column2->setID("column2"_spr);
        column2->setContentSize({120.f, 190.f});
        column2->setAnchorPoint({0, 0.5f});
        column2->setLayout(
            geode::ColumnLayout::create()
                ->setAxisReverse(true)
                ->setAxisAlignment(geode::AxisAlignment::Even)
                ->setCrossAxisAlignment(geode::AxisAlignment::Start)
                ->setCrossAxisLineAlignment(geode::AxisAlignment::Start)
                ->setAutoScale(false)
        );
        layer->addChildAtPosition(column2, geode::Anchor::Center, { -55.f, 0.f });

        return layer;
    }

    cocos2d::CCLayer* LabelSettingsPopup::createTextTab() const {
        auto layer = CCLayer::create();

        auto input = geode::TextInput::create(250.f, "FPS: {fps}", "font_default.fnt"_spr);
        input->setCommonFilter(geode::CommonFilter::Any);
        input->setString(m_settings->text);
        input->setContentSize({ 375.f, 187.5f });
        input->getBGSprite()->setContentSize({ 750.f, 375.f });
        input->updateLayout();
        input->setCallback([this](std::string const& text) {
            m_settings->text = text;
            m_callback(CallbackEvent::Update);
        });

        layer->addChildAtPosition(input, geode::Anchor::Center);

        return layer;
    }

    cocos2d::CCLayer* LabelSettingsPopup::createEventsTab() {
        auto layer = CCLayer::create();


        return layer;
    }

    cocos2d::CCLayer* LabelSettingsPopup::createPreviewTab() {
        auto layer = CCLayer::create();

        m_previewLabel = hacks::Labels::SmartLabel::create(m_settings->text, m_settings->font);
        m_previewLabel->setScale(m_settings->scale);
        m_previewLabel->setColor(m_settings->color.toCCColor3B());
        m_previewLabel->setOpacity(m_settings->color.getAlphaByte());
        m_previewLabel->setAlignment(m_settings->fontAlignment);
        // m_previewLabel->setVisible(m_settings->visible);
        m_previewLabel->update();

        layer->addChildAtPosition(m_previewLabel, geode::Anchor::Center);
        this->schedule(schedule_selector(LabelSettingsPopup::updatePreview));

        return layer;
    }

    void LabelSettingsPopup::updatePreview(float) {
        if (m_settings->hasEvents()) {
            auto [visible, scale, color, font] = m_settings->processEvents();
            m_previewLabel->setFont(font);
            m_previewLabel->setScale(scale);
            m_previewLabel->setColor(color.toCCColor3B());
            m_previewLabel->setOpacity(color.getAlphaByte());
            // m_previewLabel->setVisible(visible);
        }

        m_previewLabel->update();
    }

    void LabelSettingsPopup::selectTab(size_t index) {
        for (size_t i = 0; i < m_tabs.size(); ++i) {
            m_tabs[i]->setState(i == index);
        }

        this->unschedule(schedule_selector(LabelSettingsPopup::updatePreview));

        if (m_currentTab) {
            m_currentTab->removeFromParent();
        }

        switch (index) {
            case 0: m_currentTab = createSettingsTab(); break;
            case 1: m_currentTab = createTextTab(); break;
            case 2: m_currentTab = createEventsTab(); break;
            case 3: m_currentTab = createPreviewTab(); break;
            default: break;
        }

        m_currentTab->setID("current-tab"_spr);
        m_contentBG->addChildAtPosition(m_currentTab, geode::Anchor::Center);
    }

    LabelSettingsPopup* LabelSettingsPopup::create(labels::LabelSettings* settings, std::function<void(CallbackEvent)> const& callback) {
        auto ret = new LabelSettingsPopup;
        if (ret->initAnchored(400.f, 240.f, settings, callback)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
}
