#include "label-settings-popup.hpp"

#include <hacks/Labels/Label.hpp>
#include <modules/gui/cocos/cocos.hpp>
#include <modules/gui/cocos/components/LabelSettingsComponent.hpp>
#include <modules/gui/cocos/nodes/color-picker.hpp>
#include <modules/gui/cocos/nodes/FallbackBMFont.hpp>
#include <modules/gui/cocos/nodes/oneof-picker.hpp>
#include <modules/gui/theming/manager.hpp>
#include <modules/i18n/translations.hpp>
#include <modules/labels/setting.hpp>

#include "scroll-layer.hpp"

namespace eclipse::gui::cocos {
    constexpr GLuint DISABLED_OPACITY = 255 * 0.3f;

    class PopupTab : public CCMenuItemSpriteExtra {
    public:
        static PopupTab* create(std::string_view text, size_t page, LabelSettingsPopup* popup) {
            auto ret = new PopupTab();
            if (ret->init(text, page, popup)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        void setState(bool active) {
            auto const tm = ThemeManager::get();
            m_background->setColor(
                active ? tm->getButtonActivatedBackground().toCCColor3B()
                       : tm->getButtonActivatedBackground().darken(0.1F).toCCColor3B()
            );
            setEnabled(!active);
        }

    protected:
        void onClicked(CCObject*) {
            m_popup->selectTab(m_page);
        }

        bool init(std::string_view text, size_t page, LabelSettingsPopup* popup) {
            auto const tm = ThemeManager::get();

            m_background = geode::NineSlice::create("square02b_001.png");
            m_background->setContentSize({ 100.f, 32.f });
            m_background->setScale(0.7f);
            m_background->setColor(tm->getButtonActivatedBackground().toCCColor3B());

            static Label::EmojiMap const s_emojis = {
                {U"⚙️", "settings.png"_spr},
                {U"⚡", "script_icon.png"_spr},
                {U"🕔", "event_icon.png"_spr},
                {U"👁️", "preview_icon.png"_spr},
            };
            static std::unordered_map<std::string_view, std::string_view> s_emojisMap = {
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
        geode::NineSlice* m_background = nullptr;
    };

    class FontPicker : public cocos2d::CCMenu {
    public:
        static FontPicker* create(std::string font, Function<void(std::string const&)>&& callback) {
            auto ret = new FontPicker();
            if (ret->init(std::move(font), std::move(callback))) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        void updatePreview() const {
            m_preview->setString(labels::fontNames[m_page]);
            m_preview->setFont(m_font);
            m_preview->limitLabelWidth(getContentWidth() * 0.66f, 1.f, 0.1f);
        }

        void setFont(std::string font) {
            m_page = labels::getFontIndex(font);
            m_font = std::move(font);
            updatePreview();
        }

    protected:
        bool init(std::string&& font, Function<void(std::string const&)>&& callback) {
            if (!CCMenu::init()) return false;

            m_callback = std::move(callback);
            m_page = labels::getFontIndex(font);
            m_font = std::move(font);

            m_preview = Label::create("", m_font);
            this->addChildAtPosition(m_preview, geode::Anchor::Center);

            this->updatePreview();

            auto const createArrowBtn = [](bool flip) {
                auto const tm = ThemeManager::get();
                auto arrow = cocos2d::CCSprite::createWithSpriteFrameName("arrow.png"_spr);
                arrow->setRotation(90);
                if (flip) arrow->setFlipY(true);
                arrow->setScale(0.5f);
                arrow->setColor(tm->getButtonForegroundColor().toCCColor3B());
                return arrow;
            };

            auto left = CCMenuItemSpriteExtra::create(
                createArrowBtn(true), this,
                menu_selector(FontPicker::onPageButton)
            );
            left->setTag(0);

            auto right = CCMenuItemSpriteExtra::create(
                createArrowBtn(false), this,
                menu_selector(FontPicker::onPageButton)
            );
            right->setTag(1);

            this->addChildAtPosition(left, geode::Anchor::Left, { 5.f, 0.f });
            this->addChildAtPosition(right, geode::Anchor::Right, { -5.f, 0.f });

            return true;
        }

        void onPageButton(CCObject* sender) {
            auto tag = sender->getTag();
            if (tag == 0) {
                scrollPage(-1);
            } else {
                scrollPage(1);
            }
        }

        void scrollPage(int direction) {
            int page = m_page + direction;

            // wrap around
            if (page < 0) page = labels::fontFiles.size() - 1;
            else if (page >= labels::fontFiles.size()) page = 0;

            pickFont(page);
        }

        void pickFont(size_t index) {
            if (index >= labels::fontFiles.size()) return;

            m_page = index;
            m_font = labels::fontFiles[index];

            updatePreview();
            m_callback(m_font);
        }

    protected:
        Function<void(std::string const&)> m_callback;
        Label* m_preview = nullptr;
        std::string m_font;
        size_t m_page = 0;
    };

    bool LabelSettingsPopup::init(LabelSettingsComponent* component) {
        if (!Popup::init(400.f, 240.f))
            return false;

        auto const tm = ThemeManager::get();

        m_component = component;

        // The behind background for the entire popup to get the outline
        auto bgBehind = geode::NineSlice::create("square02b_001.png");
        bgBehind->setContentSize(m_mainLayer->getContentSize() * std::clamp(tm->getBorderSize(), 0.F, 1.F));
        m_bgSprite->setColor(tm->getBorderColor().toCCColor3B());
        bgBehind->setID("bg-behind"_spr);
        m_mainLayer->addChildAtPosition(bgBehind, geode::Anchor::Center);

        // Background for the entire popup
        m_bgSprite = geode::NineSlice::create("square02b_001.png");
        m_bgSprite->setContentSize(m_mainLayer->getContentSize() - 3);
        m_bgSprite->setColor(tm->getTitleBackgroundColor().toCCColor3B());
        m_bgSprite->setID("main-bg"_spr);
        m_mainLayer->addChildAtPosition(m_bgSprite, geode::Anchor::Center);

        // Background for content
        m_contentBG = geode::NineSlice::create("square02b_001.png");
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
        m_tabs[0] = tab;
        tabsMenu->addChild(tab);

        tab = PopupTab::create("labels.text", 1, this);
        tab->setID("text-tab"_spr);
        m_tabs[1] = tab;
        tabsMenu->addChild(tab);

        tab = PopupTab::create("labels.events", 2, this);
        tab->setID("events-tab"_spr);
        m_tabs[2] = tab;
        tabsMenu->addChild(tab);

        tab = PopupTab::create("labels.preview", 3, this);
        tab->setID("preview-tab"_spr);
        m_tabs[3] = tab;
        tabsMenu->addChild(tab);

        tabsMenu->setLayout(geode::RowLayout::create()->setAxisAlignment(geode::AxisAlignment::Even));
        m_mainLayer->addChildAtPosition(tabsMenu, geode::Anchor::Top, { 0.f, -16.f });

        this->selectTab(0);

        // Register the popup with the engine for cleanup
        if (auto cocos = CocosRenderer::get()) {
            cocos->registerModal(this);
        }

        return true;
    }

    LabelSettingsPopup::~LabelSettingsPopup() {
        if (auto cocos = CocosRenderer::get()) {
            cocos->unregisterModal(this);
        }
    }

    static cocos2d::CCSprite* createButton(cocos2d::CCSprite* inner, float scale = 0.4f) {
        auto const tm = ThemeManager::get();
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

    static cocos2d::CCSprite* createToggle(char const* innerFrameName, float scale = 0.4f) {
        auto const tm = ThemeManager::get();
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

    static cocos2d::CCSprite* createButtonSprite(char const* frame, float scale = 0.4f, std::optional<float> innerScale = std::nullopt) {
        if (frame) {
            auto inner = cocos2d::CCSprite::createWithSpriteFrameName(frame);
            if (innerScale) { inner->setScale(*innerScale); }
            return createButton(inner, scale);
        }
        return createButton(nullptr, scale);
    }

    static cocos2d::CCSprite* createAlignButton(BMFontAlignment align) {
        char const* frame = nullptr;
        switch (align) {
            default: break;
            case BMFontAlignment::Left: frame = "falign_left.png"_spr; break;
            case BMFontAlignment::Center: frame = "falign_center.png"_spr; break;
            case BMFontAlignment::Right: frame = "falign_right.png"_spr; break;
        }
        return createButtonSprite(frame, 0.5f, 1.5f);
    }

    static cocos2d::CCSprite* createAlignButton(labels::LabelsContainer::Alignment align) {
        char const* frame = nullptr;
        switch (align) {
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
    static std::pair<cocos2d::CCMenu*, std::array<cocos2d::CCSprite*, sizeof...(Args)>> createCompactMenu(
        float width, LabelSettingsPopup* self,
        cocos2d::SEL_MenuHandler callback, Args... args
    ) {
        auto menu = cocos2d::CCMenu::create();
        menu->setContentSize({ width, 28.f });

        size_t index = 0;
        for (auto& button : { CCMenuItemSpriteExtra::create(args, self, callback)... }) {
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

    static TranslatedLabel* createLabel(char const* text) {
        auto label = TranslatedLabel::create(text);
        label->setAnchorPoint({0, 1});
        label->limitLabelWidth(110.f, 1.f, 0.1f);
        return label;
    }

    static CCMenuItemSpriteExtra* createLabelButton(
        char const* text, float width,
        LabelSettingsPopup* self, cocos2d::SEL_MenuHandler selector
    ) {
        auto const tm = ThemeManager::get();
        auto label = TranslatedLabel::create(text);
        label->limitLabelWidth(width * 0.8f, 1.f, 0.1f);
        label->setColor(tm->getButtonForegroundColor().toCCColor3B());
        auto bg = geode::NineSlice::create("square02b_001.png");
        bg->setContentSize({ width, 36.f });
        bg->setColor(tm->getButtonBackgroundColor().toCCColor3B());
        bg->addChildAtPosition(label, geode::Anchor::Center);
        bg->setScale(0.65f);
        return CCMenuItemSpriteExtra::create(bg, self, selector);
    }

    cocos2d::CCLayer* LabelSettingsPopup::createSettingsTab() {
        auto layer = CCLayer::create();
        auto* settings = m_component->getSettings();

        auto [alignMenu, alignSprites] = createCompactMenu(
            100.f, this,
            menu_selector(LabelSettingsPopup::onAlignButton),
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
        m_alignButtons = alignSprites;
        m_alignButtons[static_cast<size_t>(settings->alignment)]->setOpacity(255);
        alignMenu->setID("align-menu"_spr);
        layer->addChildAtPosition(alignMenu, geode::Anchor::Center, { 132.f, 42.f });

        auto [fontAlignMenu, fontAlignSprites] = createCompactMenu(
            100.f, this,
            menu_selector(LabelSettingsPopup::onFontAlignButton),
            createAlignButton(BMFontAlignment::Left),
            createAlignButton(BMFontAlignment::Center),
            createAlignButton(BMFontAlignment::Right)
        );
        m_fontAlignButtons = fontAlignSprites;
        m_fontAlignButtons[static_cast<size_t>(settings->fontAlignment)]->setOpacity(255);
        fontAlignMenu->setID("font-align-menu"_spr);
        layer->addChildAtPosition(fontAlignMenu, geode::Anchor::Center, { 132.f, -32.f });

        auto column1 = CCNode::create();

        column1->addChild(createLabel("labels.color"));
        column1->addChild(createLabel("labels.opacity"));
        column1->addChild(createLabel("labels.scale"));
        column1->addChild(createLabel("labels.font"));
        column1->addChild(createLabel("labels.absolute"));

        auto absoluteX = geode::TextInput::create(120.f, "0", "font_default.fnt"_spr);
        absoluteX->setCommonFilter(geode::CommonFilter::Float);
        absoluteX->setString(fmt::to_string(settings->offset.x));
        absoluteX->setCallback([this](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            m_component->getSettings()->offset.x = res.unwrap();
            m_component->triggerEditCallback();
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

        column2->addChild(ColorPicker::create(settings->color, false, [this](Color color) {
            m_component->getSettings()->color = { color.r, color.g, color.b, m_component->getSettings()->color.a };
            m_component->triggerEditCallback();
        }));

        auto opacityInput = geode::TextInput::create(120.f, "0.750000", "font_default.fnt"_spr);
        opacityInput->setCommonFilter(geode::CommonFilter::Float);
        opacityInput->setString(fmt::to_string(settings->color.a));
        opacityInput->setCallback([this](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            m_component->getSettings()->color.a = res.unwrap();
            m_component->triggerEditCallback();
        });
        column2->addChild(opacityInput);

        auto scaleInput = geode::TextInput::create(120.f, "0.300000", "font_default.fnt"_spr);
        scaleInput->setCommonFilter(geode::CommonFilter::Float);
        scaleInput->setString(fmt::to_string(settings->scale));
        scaleInput->setCallback([this](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            m_component->getSettings()->scale = res.unwrap();
            m_component->triggerEditCallback();
        });
        column2->addChild(scaleInput);

        auto picker = FontPicker::create(settings->font, [this](std::string const& font) {
            m_component->getSettings()->font = font;
            m_component->triggerEditCallback();
        });
        picker->setID("font-picker"_spr);
        picker->setContentSize({ 120.f, 28.f });
        picker->updateLayout();
        picker->updatePreview();
        column2->addChild(picker);

        auto absolutePosToggle = createToggler(
            createToggle("checkmark.png"_spr), createToggle(nullptr),
            [this](auto) {
                m_component->getSettings()->absolutePosition = !m_component->getSettings()->absolutePosition;
                m_component->triggerEditCallback();
            }
        );
        absolutePosToggle->toggle(settings->absolutePosition);
        column2->addChild(absolutePosToggle);

        auto absoluteY = geode::TextInput::create(120.f, "0", "font_default.fnt"_spr);
        absoluteY->setCommonFilter(geode::CommonFilter::Float);
        absoluteY->setString(fmt::to_string(settings->offset.y));
        absoluteY->setCallback([this](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            m_component->getSettings()->offset.y = res.unwrap();
            m_component->triggerEditCallback();
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

        auto exportBtn = createLabelButton(
            "labels.export", 120.f,
            this, menu_selector(LabelSettingsPopup::onExport)
        );

        exportBtn->setID("export-btn"_spr);
        auto menu = cocos2d::CCMenu::create();
        menu->addChildAtPosition(exportBtn, geode::Anchor::Center, { 132.f, -72.f });
        layer->addChild(menu);

        return layer;
    }

    cocos2d::CCLayer* LabelSettingsPopup::createTextTab() {
        auto layer = CCLayer::create();

        auto input = geode::TextInput::create(250.f, "FPS: {fps}", "font_default.fnt"_spr);
        input->setCommonFilter(geode::CommonFilter::Any);
        input->setString(m_component->getSettings()->text);
        input->setContentSize({ 375.f, 187.5f });
        input->getBGSprite()->setContentSize({ 750.f, 375.f });
        input->updateLayout();
        input->setCallback([this](std::string const& text) {
            m_component->getSettings()->text = text;
            m_component->triggerEditCallback();
        });

        layer->addChildAtPosition(input, geode::Anchor::Center);

        return layer;
    }

    constexpr auto CARD_WIDTH = 370.f;
    constexpr auto CARD_HEIGHT = 150.f;

    constexpr float getEventContainerHeight(size_t count) {
        return std::max((CARD_HEIGHT + 2.5f) * count + 42.f, 200.f);
    }

    static cocos2d::CCNode* createEventAddButton(LabelSettingsPopup* self) {
        auto addEventBtn = createLabelButton(
            "labels.events.add", 120.f,
            self, menu_selector(LabelSettingsPopup::onCreateEvent)
        );
        addEventBtn->setID("add-event-btn"_spr);

        auto menu = cocos2d::CCMenu::create();
        menu->setContentSize({ 385.f, 36.f });
        menu->addChildAtPosition(addEventBtn, geode::Anchor::Center);
        menu->setZOrder(5); // make sure the button is always last
        return menu;
    }

    cocos2d::CCNode* LabelSettingsPopup::createEventCard(labels::LabelEvent& event, size_t index) {
        auto menu = cocos2d::CCMenu::create();
        menu->setContentSize({ CARD_WIDTH, CARD_HEIGHT });
        menu->setID("event-card"_spr);

        auto cardBackground = geode::NineSlice::create("square02b_001.png");
        cardBackground->setContentSize({ CARD_WIDTH, CARD_HEIGHT });
        cardBackground->setColor(ThemeManager::get()->getBackgroundColor().lighten(0.25f).toCCColor3B());
        cardBackground->setOpacity(32);
        cardBackground->setID("event-card-bg"_spr);
        menu->addChildAtPosition(cardBackground, geode::Anchor::Center);

        // Pickers
        auto colorPicker = ColorPicker::create(
            event.color.value_or(Color{1.f, 1.f, 1.f}), false,
            [this, &event](Color color) {
                event.color = color;
                m_component->triggerEditCallback();
            }
        );
        colorPicker->setID("color-picker"_spr);
        colorPicker->setVisible(event.color.has_value());
        menu->addChildAtPosition(colorPicker, geode::Anchor::TopLeft, { 137.5f, -73.f });

        auto fontPicker = FontPicker::create(
            event.font.value_or("bigFont.fnt"),
            [this, &event](std::string const& font) {
                event.font = font;
                m_component->triggerEditCallback();
            }
        );
        fontPicker->setID("font-picker"_spr);
        fontPicker->setVisible(event.font.has_value());
        fontPicker->setScale(0.75f);
        fontPicker->setContentSize({ 120.f, 28.f });
        fontPicker->updateLayout();
        fontPicker->updatePreview();
        menu->addChildAtPosition(fontPicker, geode::Anchor::TopLeft, { 137.5f, -102.f });

        auto conditionPicker = OneOfPicker::create(
            labels::eventNames,
            [this, &event](int i) {
                event.type = static_cast<labels::LabelEvent::Type>(i);
                m_component->triggerEditCallback();
            },
            static_cast<size_t>(event.type)
        );
        conditionPicker->setID("condition-picker"_spr);
        conditionPicker->setScale(0.9f);
        conditionPicker->setContentSize({ 100.f, 28.f });
        conditionPicker->updateLayout();
        menu->addChildAtPosition(conditionPicker, geode::Anchor::TopLeft, { 137.5f, -16.f });

        auto visibilityPicker = OneOfPicker::create(
            labels::visibleNames,
            [&](int i) {
                if (i == 0) event.visible.reset();
                else event.visible = i == 1;
                m_component->triggerEditCallback();
            },
            event.visible.has_value() ? (event.visible.value() ? 1 : 2) : 0
        );
        visibilityPicker->setID("visibility-picker"_spr);
        visibilityPicker->setScale(0.9f);
        visibilityPicker->setContentSize({ 100.f, 28.f });
        visibilityPicker->updateLayout();
        menu->addChildAtPosition(visibilityPicker, geode::Anchor::TopLeft, { 137.5f, -44.f });

        // Inputs
        auto customConditionInput = geode::TextInput::create(165.f, "progress >= bestPercent", "font_default.fnt"_spr);
        customConditionInput->setCommonFilter(geode::CommonFilter::Any);
        customConditionInput->setString(event.condition);
        customConditionInput->setScale(0.9f);
        customConditionInput->setCallback([this, &event](std::string const& text) {
            event.condition = text;
            m_component->triggerEditCallback();
        });
        customConditionInput->setID("custom-condition-input"_spr);
        menu->addChildAtPosition(customConditionInput, geode::Anchor::TopLeft, { 260.f, -16.f });

        auto durationInput = geode::TextInput::create(100.f, "1.0", "font_default.fnt"_spr);
        durationInput->setCommonFilter(geode::CommonFilter::Float);
        durationInput->setString(fmt::to_string(event.duration));
        durationInput->setScale(0.9f);
        durationInput->setCallback([this, &event](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            event.duration = res.unwrap();
            m_component->triggerEditCallback();
        });
        durationInput->setID("duration-input"_spr);
        menu->addChildAtPosition(durationInput, geode::Anchor::TopLeft, { 137.5f, -132.f });

        auto modifyScaleInput = geode::TextInput::create(100.f, "1.0", "font_default.fnt"_spr);
        modifyScaleInput->setCommonFilter(geode::CommonFilter::Float);
        modifyScaleInput->setString(fmt::to_string(event.scale.value_or(1.f)));
        modifyScaleInput->setScale(0.9f);
        modifyScaleInput->setCallback([this, &event](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            event.scale = res.unwrap();
            m_component->triggerEditCallback();
        });
        modifyScaleInput->setID("modify-scale-input"_spr);
        modifyScaleInput->setVisible(event.scale.has_value());
        menu->addChildAtPosition(modifyScaleInput, geode::Anchor::TopLeft, { 322.5f, -44.f });

        auto modifyOpacityInput = geode::TextInput::create(100.f, "1.0", "font_default.fnt"_spr);
        modifyOpacityInput->setCommonFilter(geode::CommonFilter::Float);
        modifyOpacityInput->setString(fmt::to_string(event.opacity.value_or(1.f)));
        modifyOpacityInput->setScale(0.9f);
        modifyOpacityInput->setCallback([this, &event](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            event.opacity = res.unwrap();
            m_component->triggerEditCallback();
        });
        modifyOpacityInput->setID("modify-opacity-input"_spr);
        modifyOpacityInput->setVisible(event.opacity.has_value());
        menu->addChildAtPosition(modifyOpacityInput, geode::Anchor::TopLeft, { 322.5f, -73.f });

        auto delayInput = geode::TextInput::create(100.f, "0.0", "font_default.fnt"_spr);
        delayInput->setCommonFilter(geode::CommonFilter::Float);
        delayInput->setString(fmt::to_string(event.delay));
        delayInput->setScale(0.9f);
        delayInput->setCallback([this, &event](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            event.delay = res.unwrap();
            m_component->triggerEditCallback();
        });
        delayInput->setID("delay-input"_spr);
        menu->addChildAtPosition(delayInput, geode::Anchor::TopLeft, { 322.5f, -102.f });

        auto easingInput = geode::TextInput::create(100.f, "0.0", "font_default.fnt"_spr);
        easingInput->setCommonFilter(geode::CommonFilter::Float);
        easingInput->setString(fmt::to_string(event.easing));
        easingInput->setScale(0.9f);
        easingInput->setCallback([this, &event](std::string const& text) {
            auto res = geode::utils::numFromString<float>(text);
            if (!res) { return; }
            event.easing = res.unwrap();
            m_component->triggerEditCallback();
        });
        easingInput->setID("easing-input"_spr);
        menu->addChildAtPosition(easingInput, geode::Anchor::TopLeft, { 322.5f, -132.f });

        // Toggles
        auto enableToggle = createToggler(
            createToggle("checkmark.png"_spr),
            createToggle(nullptr),
            [this, &event](auto) {
                event.enabled = !event.enabled;
                m_component->triggerEditCallback();
            }
        );
        enableToggle->toggle(event.enabled);
        enableToggle->setID("enable-toggle"_spr);
        menu->addChildAtPosition(enableToggle, geode::Anchor::TopLeft, { 16.f, -16.f });

        auto modifyColorToggle = createToggler(
            createToggle("checkmark.png"_spr), createToggle(nullptr),
            [this, &event, colorPicker](auto) {
                if (!event.color.has_value()) event.color = {1.f, 1.f, 1.f};
                else event.color.reset();
                colorPicker->setVisible(event.color.has_value());
                colorPicker->setColor(event.color.value_or(gui::Color{1.f, 1.f, 1.f}));
                m_component->triggerEditCallback();
            }
        );
        modifyColorToggle->toggle(event.color.has_value());
        modifyColorToggle->setID("modify-color-toggle"_spr);
        menu->addChildAtPosition(modifyColorToggle, geode::Anchor::TopLeft, { 16.f, -73.f });

        auto modifyFontToggle = createToggler(
            createToggle("checkmark.png"_spr), createToggle(nullptr),
            [this, &event, fontPicker](auto) {
                if (!event.font.has_value()) event.font = "bigFont.fnt";
                else event.font.reset();
                fontPicker->setVisible(event.font.has_value());
                fontPicker->setFont(event.font.value_or("bigFont.fnt"));
                m_component->triggerEditCallback();
            }
        );
        modifyFontToggle->toggle(event.font.has_value());
        modifyFontToggle->setID("modify-font-toggle"_spr);
        menu->addChildAtPosition(modifyFontToggle, geode::Anchor::TopLeft, { 16.f, -102.f });

        auto modifyScaleToggle = createToggler(
            createToggle("checkmark.png"_spr),
            createToggle(nullptr),
            [this, &event, modifyScaleInput](auto) {
                if (!event.scale.has_value()) event.scale = 1.f;
                else event.scale.reset();
                modifyScaleInput->setString(fmt::to_string(event.scale.value_or(1.f)));
                modifyScaleInput->setVisible(event.scale.has_value());
                m_component->triggerEditCallback();
            }
        );
        modifyScaleToggle->toggle(event.scale.has_value());
        modifyScaleToggle->setID("modify-scale-toggle"_spr);
        menu->addChildAtPosition(modifyScaleToggle, geode::Anchor::TopLeft, { 198.5f, -44.f });

        auto modifyOpacityToggle = createToggler(
            createToggle("checkmark.png"_spr),
            createToggle(nullptr),
            [this, &event, modifyOpacityInput](auto) {
                if (!event.opacity.has_value()) event.opacity = 1.f;
                else event.opacity.reset();
                modifyOpacityInput->setString(fmt::to_string(event.opacity.value_or(1.f)));
                modifyOpacityInput->setVisible(event.opacity.has_value());
                m_component->triggerEditCallback();
            }
        );
        modifyOpacityToggle->toggle(event.opacity.has_value());
        modifyOpacityToggle->setID("modify-opacity-toggle"_spr);
        menu->addChildAtPosition(modifyOpacityToggle, geode::Anchor::TopLeft, { 198.5f, -73.f });

        // Labels
        auto const createLabel = [menu](char const* text, float width, float x, float y) {
            auto label = TranslatedLabel::create(text);
            label->setAnchorPoint({0, 0.5f});
            label->limitLabelWidth(width, 1.f, 0.1f);
            label->setColor(ThemeManager::get()->getCheckboxForegroundColor().toCCColor3B());
            menu->addChildAtPosition(label, geode::Anchor::TopLeft, { x, y });
            return label;
        };

        createLabel("labels.events.enabled", 55, 32, -16)->setID("enable-label"_spr);
        createLabel("labels.events.visible", 64, 6, -44)->setID("visibility-label"_spr);
        createLabel("labels.events.color", 55, 32, -73)->setID("color-label"_spr);
        createLabel("labels.events.font", 55, 32, -102)->setID("font-label"_spr);
        createLabel("labels.events.duration", 64, 6, -132)->setID("duration-label"_spr);

        createLabel("labels.events.scale", 55, 215, -44)->setID("scale-label"_spr);
        createLabel("labels.events.opacity", 55, 215, -73)->setID("opacity-label"_spr);
        createLabel("labels.events.delay", 64, 188, -102)->setID("delay-label"_spr);
        createLabel("labels.events.easing", 64, 188, -132)->setID("easing-label"_spr);

        // Delete button
        auto deleteBtn = createSpriteExtra(
            createButtonSprite("trashbin.png"_spr, 0.35f),
            [this, index](auto) {
                auto settings = m_component->getSettings();
                settings->events.erase(settings->events.begin() + index);
                m_component->triggerEditCallback();
                this->selectTab(2);
            }
        );
        deleteBtn->setID("delete-btn"_spr);
        menu->addChildAtPosition(deleteBtn, geode::Anchor::TopLeft, { 352.5f, -16.f });

        return menu;
    }

    cocos2d::CCLayer* LabelSettingsPopup::createEventsTab() {
        auto layer = CCLayer::create();

        auto scrollLayer = ScrollLayer::create({ 385.f, 200.f });
        scrollLayer->setID("events-scroll"_spr);
        layer->addChildAtPosition(scrollLayer, geode::Anchor::Center, { -385.f / 2, -200.f / 2 });

        auto settings = m_component->getSettings();

        auto contentLayer = scrollLayer->m_contentLayer;
        m_eventsContentLayer = contentLayer;

        for (auto& event : settings->events) {
            size_t index = &event - &settings->events.front();
            contentLayer->addChild(createEventCard(event, index));
        }

        contentLayer->addChild(createEventAddButton(this));

        contentLayer->setContentSize({ 385.f, getEventContainerHeight(settings->events.size()) });
        contentLayer->setLayout(
            geode::ColumnLayout::create()
                ->setAxisReverse(true)
                ->setAxisAlignment(geode::AxisAlignment::Center)
                ->setCrossAxisAlignment(geode::AxisAlignment::Center)
                ->setCrossAxisLineAlignment(geode::AxisAlignment::Center)
                ->setGap(2.5f)
                ->setAutoScale(false)
        );

        return layer;
    }

    cocos2d::CCLayer* LabelSettingsPopup::createPreviewTab() {
        auto layer = CCLayer::create();
        auto settings = m_component->getSettings();

        m_previewLabel = hacks::Labels::SmartLabel::create(settings->text, settings->font);
        m_previewLabel->setSettings(settings);

        m_previewLabel->setFont(settings->font);
        m_previewLabel->setScale(settings->scale);
        m_previewLabel->setColor(settings->color.toCCColor3B());
        m_previewLabel->setOpacity(settings->color.getAlphaByte());
        m_previewLabel->setAlignment(settings->fontAlignment);

        auto origVisible = settings->visible;
        settings->visible = true;
        m_previewLabel->update();
        settings->visible = origVisible;

        layer->addChildAtPosition(m_previewLabel, geode::Anchor::Center);
        this->schedule(schedule_selector(LabelSettingsPopup::updatePreview));

        return layer;
    }

    void LabelSettingsPopup::updatePreview(float) {
        auto settings = m_component->getSettings();
        auto origVisible = settings->visible;
        settings->visible = true;
        m_previewLabel->update();
        settings->visible = origVisible;
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

    void LabelSettingsPopup::onExport(CCObject*) {
        m_component->triggerExportCallback();
    }

    void LabelSettingsPopup::onCreateEvent(CCObject*) {
        auto settings = m_component->getSettings();
        settings->events.emplace_back();
        m_component->triggerEditCallback();

        auto index = settings->events.size() - 1;

        // Add a new card
        auto card = this->createEventCard(settings->events.back(), index);
        m_eventsContentLayer->addChild(card);
        m_eventsContentLayer->setContentSize({ 385.f, getEventContainerHeight(settings->events.size()) });
        m_eventsContentLayer->updateLayout();
    }

    void LabelSettingsPopup::onAlignButton(CCObject* sender) {
        auto tag = sender->getTag();
        for (size_t i = 0; i < m_alignButtons.size(); ++i) {
            m_alignButtons[i]->setOpacity(i == tag ? 255 : DISABLED_OPACITY);
        }
        m_component->getSettings()->alignment = static_cast<labels::LabelsContainer::Alignment>(tag);
        m_component->triggerEditCallback();
    }

    void LabelSettingsPopup::onFontAlignButton(CCObject* sender) {
        auto tag = sender->getTag();
        for (size_t i = 0; i < m_fontAlignButtons.size(); ++i) {
            m_fontAlignButtons[i]->setOpacity(i == tag ? 255 : DISABLED_OPACITY);
        }
        m_component->getSettings()->fontAlignment = static_cast<BMFontAlignment>(tag);
        m_component->triggerEditCallback();
    }

    LabelSettingsPopup* LabelSettingsPopup::create(LabelSettingsComponent* component) {
        auto ret = new LabelSettingsPopup;
        if (ret->init(component)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
}
