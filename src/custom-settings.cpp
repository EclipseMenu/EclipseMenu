#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/ui/Popup.hpp>
#include <modules/gui/theming/manager.hpp>

using namespace geode::prelude;

class CustomButton : public SettingV3 {
protected:
    std::string m_buttonCaption = "OK";

public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<CustomButton>();
        auto root = checkJson(json, "CustomButton");

        res->init(key, modID, root);
        res->parseNameAndDescription(root);
        res->parseEnableIf(root);
        res->parseButtonCaption(json, "default");

        root.checkUnknownKeys();
        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }

    void parseButtonCaption(matjson::Value const& json, std::string const& key) {
        m_buttonCaption = json[key].asString().unwrap();
    }

    bool load(matjson::Value const& json) override { return true; }
    bool save(matjson::Value& json) const override { return true; }
    bool isDefaultValue() const override { return true; }
    std::string const& getButtonCaption() const { return m_buttonCaption; }

    void reset() override {}

    SettingNodeV3* createNode(float width) override;
};

class CustomButtonNode : public SettingNodeV3 {
protected:
    ButtonSprite* m_buttonSprite{};
    CCMenuItemSpriteExtra* m_button{};

    bool init(const std::shared_ptr<CustomButton>& setting, float width) {
        if (!SettingNodeV3::init(setting, width))
            return false;

        m_buttonSprite = ButtonSprite::create(this->getSetting()->getButtonCaption().c_str(), "goldFont.fnt", "GJ_button_01.png", .8f);
        m_buttonSprite->setScale(.5f);
        m_button = CCMenuItemSpriteExtra::create(
            m_buttonSprite, this, menu_selector(CustomButtonNode::onButton)
        );
        this->getButtonMenu()->addChildAtPosition(m_button, Anchor::Center);
        this->getButtonMenu()->setContentWidth(60);
        this->getButtonMenu()->updateLayout();

        this->updateState(nullptr);

        return true;
    }

    void onButton(CCObject*) {
        static std::unordered_map<std::string, std::function<void()>> buttonCallbacks = {
            {"reset-theme", [] {
                createQuickPopup("Reset Theme", "Are you sure you want to reset the theme to default?",
                    "No", "Yes", [](auto, bool btn2) {
                        if (btn2) eclipse::gui::ThemeManager::get()->reloadTheme();
                    });
            }},
        };

        auto buttonID = this->getSetting()->getKey();
        if (auto it = buttonCallbacks.find(buttonID); it != buttonCallbacks.end())
            it->second();
    }

    void onCommit() override {}

    void onResetToDefault() override {}

public:
    static CustomButtonNode* create(const std::shared_ptr<CustomButton>& setting, float width) {
        auto ret = new CustomButtonNode;
        if (ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool hasUncommittedChanges() const override { return false; }
    bool hasNonDefaultValue() const override { return false; }

    std::shared_ptr<CustomButton> getSetting() const {
        return std::static_pointer_cast<CustomButton>(SettingNodeV3::getSetting());
    }
};

SettingNodeV3* CustomButton::createNode(float width) {
    return CustomButtonNode::create(
        std::static_pointer_cast<CustomButton>(shared_from_this()),
        width
    );
}

$execute { (void) Mod::get()->registerCustomSettingType("custom-btn", &CustomButton::parse); }
