#pragma once
#include <AdvancedLabel.hpp>

namespace eclipse::gui::cocos {

    /// @brief Label node that supports translation.
    class TranslatedLabel : public Label {
    public:
        static TranslatedLabel* create(geode::ZStringView key) {
            auto ret = new TranslatedLabel();
            if (ret->init(key)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        static TranslatedLabel* createRaw(geode::ZStringView text) {
            auto ret = new TranslatedLabel();
            if (ret->initRaw(text)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        static TranslatedLabel* createWrapped(geode::ZStringView key, float wrapWidth, float scale = 1.f) {
            auto ret = new TranslatedLabel();
            if (ret->initWrapped(key, wrapWidth, scale)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        static TranslatedLabel* createWrappedRaw(geode::ZStringView text, float wrapWidth, float scale = 1.f) {
            auto ret = new TranslatedLabel();
            if (ret->initWrappedRaw(text, wrapWidth, scale)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

    protected:
        static std::string getCurrentAtlas();
        static bool isDefaultAtlas();

        bool init(geode::ZStringView key);
        bool initWrapped(geode::ZStringView key, float wrapWidth, float scale);
        bool initRaw(geode::ZStringView text);
        bool initWrappedRaw(geode::ZStringView text, float wrapWidth, float scale);
    };

    class EmojiLabel : public Label {
    public:
        static EmojiLabel* create(geode::ZStringView text, std::string const& font) {
            auto ret = new EmojiLabel();
            if (ret->init(text, font)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

    protected:
        bool init(geode::ZStringView text, std::string const& font);
    };

}
