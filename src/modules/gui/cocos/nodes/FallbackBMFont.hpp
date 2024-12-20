#pragma once
#include <modules/config/config.hpp>
#include <modules/i18n/translations.hpp>

namespace eclipse::gui::cocos {

    inline static uint16_t* copyUTF16StringN(const uint16_t* str) {
        auto len = str ? std::char_traits<uint16_t>::length(str) : 0;
        auto copy = new uint16_t[len + 1];
        std::char_traits<uint16_t>::copy(copy, str, len);
        copy[len] = 0;
        return copy;
    }

    inline static std::u16string UTF8ToUTF16(const std::string& utf8) {
#ifdef GEODE_IS_WINDOWS
        auto len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
        std::vector<uint16_t> utf16(len);
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, reinterpret_cast<wchar_t*>(utf16.data()), len);
        return std::u16string(reinterpret_cast<char16_t*>(utf16.data()));
#elif GEODE_IS_ANDROID
        auto str = cocos2d::cc_utf8_to_utf16(utf8.c_str());
        auto ret = std::u16string(reinterpret_cast<char16_t*>(str));
        delete[] str;
        return ret;
#else
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        return convert.from_bytes(utf8);
#endif
    }

    inline static void setBatchSpriteColor(cocos2d::CCSpriteBatchNode* batch, const cocos2d::ccColor3B& color) {
        if (auto children = batch->getChildren()) {
            for (int i = 0; i < children->count(); i++) {
                static_cast<cocos2d::CCSprite*>(children->objectAtIndex(i))->setColor(color);
            }
        }
    }

    inline static void setBatchSpriteOpacity(cocos2d::CCSpriteBatchNode* batch, GLubyte opacity) {
        if (auto children = batch->getChildren()) {
            for (int i = 0; i < children->count(); i++) {
                static_cast<cocos2d::CCSprite*>(children->objectAtIndex(i))->setOpacity(opacity);
            }
        }
    }

    inline static void setBatchSpriteColorAndOpacity(cocos2d::CCSpriteBatchNode* batch, const cocos2d::ccColor3B& color, GLubyte opacity) {
        if (auto children = batch->getChildren()) {
            for (int i = 0; i < children->count(); i++) {
                auto sprite = static_cast<cocos2d::CCSprite*>(children->objectAtIndex(i));
                sprite->setColor(color);
                sprite->setOpacity(opacity);
            }
        }
    }

    inline static std::unordered_map<uint64_t, std::string_view> g_emojis = {
        { 0xD83D'DC7D, "alien.png"_spr },
        { 0xD83D'DC76, "baby.png"_spr },
        { 0xD83D'DC94, "brokenHeart.png"_spr },
        { 0xD83D'DC31, "cat.png"_spr },
        { 0x2705,      "checkmark.png"_spr },
        { 0x2666'FE0F, "checkpoint.png"_spr },
        { 0x274C,      "cross.png"_spr },
        { 0xD83D'DC4E, "dislike.png"_spr },
        { 0xD83D'DC36, "dog.png"_spr },
        { 0x2757,      "exclamation.png"_spr },
        { 0xD83D'DD25, "fire.png"_spr },
        { 0xD83D'DC7B, "ghost.png"_spr },
        { 0x2764'FE0F, "heart.png"_spr },
        { 0xD83D'DC4D, "like.png"_spr },
        { 0xD83C'DF19, "moon.png"_spr },
        { 0xD83E'DDD6, "personInSteamyRoom.png"_spr },
        { 0x2753,      "question.png"_spr },
        { 0xD83E'DD16, "robot.png"_spr },
        { 0x2B55,      "secretCoin-empty.png"_spr },
        { 0xD83D'DFE1, "secretCoin-filled.png"_spr },
        { 0xD83D'DC80, "skull.png"_spr },
        { 0x2744'FE0F, "snowflake.png"_spr },
        { 0xD83D'DDE3'FE0F, "speakingHead.png"_spr },
        { 0x2B50,      "star.png"_spr },
        { 0xD83E'DDFF, "startpos.png"_spr },
        { 0xD83D'DEDE, "userCoin-empty.png"_spr },
        { 0xD83D'DD35, "userCoin-filled.png"_spr },
    };

    /// @brief Common base class for all specialized label nodes.
    template <typename T, typename P>
    class BaseLabel : public cocos2d::CCLabelBMFont {
    public:
        static T* create(const std::string& text, const std::string& font, P* parent) {
            auto ret = new T();
            if (ret->init(text, font, parent)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        bool init(const std::string& text, const std::string& font, P* parent) {
            m_parent = parent;
            m_spriteBatch = parent->getSecondBatch();

            if (!initWithString(text.c_str(), font.c_str()))
                return false;

            return true;
        }

        P* getParent() const { return m_parent; }

        void setString(unsigned short* newString, bool needUpdateLabel) override {
            auto copy = copyUTF16StringN(newString);
            if (needUpdateLabel) {
                delete[] m_sInitialString;
                m_sInitialString = copy;
            } else {
                delete[] m_sString;
                m_sString = copy;
            }

            if (m_pChildren) {
                for (int i = 0; i < m_pChildren->count(); i++) {
                    if (auto child = static_cast<CCNode*>(m_pChildren->objectAtIndex(i))) {
                        child->setVisible(false);
                    }
                }
            }
            if (auto children = m_spriteBatch->getChildren()) {
                for (int i = 0; i < children->count(); i++) {
                    if (auto child = static_cast<CCNode*>(children->objectAtIndex(i))) {
                        child->setVisible(false);
                    }
                }
            }

            // i love c++ so much
            static_cast<T*>(this)->T::createFontChars();
            if (needUpdateLabel) {
                this->updateLabel();
            }
        }

        void setString(const char* newString, bool needUpdateLabel) override {
            if (!newString) newString = "";
            if (needUpdateLabel) m_sInitialStringUTF8 = newString;
            this->BaseLabel::setString(reinterpret_cast<uint16_t*>(UTF8ToUTF16(newString).data()), needUpdateLabel);
        }

        int kerningAmountForFirst(uint16_t first, uint16_t second, const cocos2d::tCCKerningHashElement* dictionary) const {
            int ret = 0;
            unsigned int key = first << 16 | second & 0xffff;
            if( dictionary ) {
                cocos2d::tCCKerningHashElement* element = nullptr;
                HASH_FIND_INT(dictionary, &key, element);
                if(element) ret = element->amount;
            }
            return ret;
        }

        int kerningAmountForFirst(uint16_t first, uint16_t second) const {
            return kerningAmountForFirst(first, second, m_pConfiguration->m_pKerningDictionary);
        }

        void setContentSize(const cocos2d::CCSize &contentSize) override {
            CCLabelBMFont::setContentSize(contentSize);
            if (m_parent) m_parent->setContentSize(contentSize);
        }

    protected:
        P* m_parent = nullptr;
        CCSpriteBatchNode* m_spriteBatch = nullptr;
    };

    /// @brief Label node that supports two bitmap fonts, using the second one as a fallback.
    /// This is useful for languages that have characters not supported by the main font.
    /// (without having to use TTF or bitmaps with every single character)
    class FallbackBMFont : public cocos2d::CCNode {
    protected:
        class Label : public BaseLabel<Label, FallbackBMFont> {
        public:
            void createFontChars();
        };

    public:
        static FallbackBMFont* create(const std::string& text, const std::string& font, const std::string& fallbackFont);

        ~FallbackBMFont() override {
            m_fallbackConfiguration->release();
        }

        bool init(const std::string& text, const std::string& font, const std::string& fallbackFont);

        void setString(std::string_view text) const {
            m_label->Label::setString(text.data(), true);
        }

        void setFntFile(std::string_view fntFile) const {
            m_label->setFntFile(fntFile.data());
        }

        void setColor(const cocos2d::ccColor3B& color) const {
            m_label->setColor(color);
            setBatchSpriteColor(m_fallbackBatch, color);
        }

        void setOpacity(GLubyte opacity) const {
            m_label->setOpacity(opacity);
            setBatchSpriteOpacity(m_fallbackBatch, opacity);
        }

        void limitLabelWidth(float width, float defaultScale, float minScale) {
            auto originalWidth = m_label->getContentSize().width;
            auto scale = this->getScale();
            if (originalWidth > width && width > 0.0f) {
                scale = width / originalWidth;
            }
            if (defaultScale != 0.0f && defaultScale <= scale) {
                scale = defaultScale;
            }
            if (minScale != 0.0f && minScale >= scale) {
                scale = minScale;
            }
            this->setScale(scale);
        }

        cocos2d::CCSpriteBatchNode* getSecondBatch() const { return m_fallbackBatch; }

    protected:
        Label* m_label = nullptr;
        cocos2d::CCSpriteBatchNode* m_fallbackBatch = nullptr;
        cocos2d::CCBMFontConfiguration* m_fallbackConfiguration = nullptr;
    };

    /// @brief Label node that supports embedded sprites in the text.
    class EmojiLabel : public cocos2d::CCNode {
    protected:
        class Label : public BaseLabel<Label, EmojiLabel> {
        public:
            void createFontChars();
        };
    public:
        static EmojiLabel* create(const std::string& text, const std::string& font);

        virtual bool init(const std::string& text, const std::string& font);

        void setString(std::string_view text) const {
            m_label->Label::setString(text.data(), true);
        }

        void setFntFile(std::string_view fntFile) const {
            m_label->setFntFile(fntFile.data());
        }

        void setColor(const cocos2d::ccColor3B& color) const {
            m_label->setColor(color);
        }

        void setOpacity(GLubyte opacity) const {
            m_label->setOpacity(opacity);
            setBatchSpriteOpacity(m_emojiBatch, opacity);
        }

        cocos2d::CCSpriteBatchNode* getSecondBatch() const { return m_emojiBatch; }

    protected:
        Label* m_label = nullptr;
        cocos2d::CCSpriteBatchNode* m_emojiBatch = nullptr;
    };


    /// @brief Label node that supports translation.
    class TranslatedLabel : public FallbackBMFont {
    public:
        static TranslatedLabel* create(std::string_view key) {
            auto ret = new TranslatedLabel();
            ret->init(
                i18n::get_(key),
                fmt::format("font_{}.fnt"_spr, i18n::getRequiredGlyphRangesString()),
                "font_default.fnt"_spr
            );
            ret->autorelease();
            return ret;
        }
    };

}
