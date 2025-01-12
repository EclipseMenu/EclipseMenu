#pragma once
#include "label.hpp"

namespace eclipse::gui::cocos {

    /// @brief Label node that supports translation.
    class TranslatedLabel : public Label {
    public:
        static TranslatedLabel* create(std::string_view key) {
            auto ret = new TranslatedLabel();
            if (ret->init(key)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        static TranslatedLabel* createRaw(std::string_view text) {
            auto ret = new TranslatedLabel();
            if (ret->initRaw(text)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        static TranslatedLabel* createWrapped(std::string_view key, float wrapWidth, float scale = 1.f) {
            auto ret = new TranslatedLabel();
            if (ret->initWrapped(key, wrapWidth, scale)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        static TranslatedLabel* createWrappedRaw(std::string_view text, float wrapWidth, float scale = 1.f) {
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

        bool init(std::string_view key);
        bool initWrapped(std::string_view key, float wrapWidth, float scale);
        bool initRaw(std::string_view text);
        bool initWrappedRaw(std::string_view text, float wrapWidth, float scale);
    };

    class EmojiLabel : public Label {
    public:
        static EmojiLabel* create(std::string_view text, std::string_view font) {
            auto ret = new EmojiLabel();
            if (ret->init(text, font)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

    protected:
        bool init(std::string_view text, std::string_view font);
    };

}
