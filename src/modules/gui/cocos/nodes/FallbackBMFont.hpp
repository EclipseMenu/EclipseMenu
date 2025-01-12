#pragma once

#include <modules/utils/SingletonCache.hpp>
#include <codecvt>

#ifdef GEODE_IS_ANDROID
#define GD_STRING_SET_WITH_SW(str) std::string(str)
#else
#define GD_STRING_SET_WITH_SW(str) str
#endif

namespace eclipse::gui::cocos {

    static std::u16string utf8ToUtf16(std::string_view utf8) noexcept;

    static void setBatchSpriteColor(cocos2d::CCSpriteBatchNode* batch, const cocos2d::ccColor3B& color);

    static void setBatchSpriteOpacity(cocos2d::CCSpriteBatchNode* batch, GLubyte opacity);

    static void setBatchSpriteColorAndOpacity(cocos2d::CCSpriteBatchNode* batch, const cocos2d::ccColor3B& color, GLubyte opacity);

    inline static const std::unordered_map<uint64_t, std::string_view> g_emojis = {
        { 0xD83D'DC7D, "alien.png"_spr },
        { 0xD83D'DC76, "baby.png"_spr },
        { 0xD83D'DC94, "brokenHeart.png"_spr },
        { 0xD83D'DC31, "cat.png"_spr },
        { 0x2705,      "checkmark.png"_spr },
        { 0x2666'FE0F, "checkpoint.png"_spr },
        { 0x274C,      "cross.png"_spr },
        { 0xD83D'DC8E, "diamond.png"_spr },
        { 0xD83D'DCA0, "diamondShards.png"_spr },
        { 0xD83D'DC4E, "dislike.png"_spr },
        { 0xD83D'DC36, "dog.png"_spr },
        { 0xD83C'DF17, "eclipse.png"_spr },
        { 0x2757,      "exclamation.png"_spr },
        { 0xD83E'DD2F, "explodingHead.png"_spr },
        { 0xD83D'DD25, "fire.png"_spr },
        { 0xD83E'DEA9, "geode.png"_spr },
        { 0xD83D'DC7B, "ghost.png"_spr },
        { 0x2764'FE0F, "heart.png"_spr },
        { 0xD83D'DC4D, "like.png"_spr },
        { 0xD83C'DF19, "moon.png"_spr },
        { 0xD83E'DD13, "nerd.png"_spr },
        { 0xD83D'DD39, "orbs.png"_spr },
        { 0xD83E'DDD6, "personInSteamyRoom.png"_spr },
        { 0x2753,      "question.png"_spr },
        { 0xD83E'DD16, "robot.png"_spr },
        { 0x2B55,      "secretCoin-empty.png"_spr },
        { 0xD83D'DFE1, "secretCoin-filled.png"_spr },
        { 0xD83D'DC80, "skull.png"_spr },
        { 0x2744'FE0F, "snowflake.png"_spr },
        { 0xD83D'DE2D, "sob.png"_spr },
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
        }

        void setString(const char* newString, bool needUpdateLabel) override {
            if (!newString) newString = "";
            if (needUpdateLabel) {
                if (m_sInitialStringUTF8 == newString) return;
                m_sInitialStringUTF8 = newString;
            }
            m_string = std::move(utf8ToUtf16(newString));
            this->BaseLabel::setString(reinterpret_cast<uint16_t*>(m_string.data()), needUpdateLabel);
        }

        void setFntFile(std::string_view fntFile) {
            if (m_sFntFile == fntFile) return;

            auto* newConf = cocos2d::FNTConfigLoadFile(fntFile.data());
            if (!newConf) return;

            m_sFntFile = GD_STRING_SET_WITH_SW(fntFile);
            CC_SAFE_RELEASE(m_pConfiguration);
            m_pConfiguration = newConf;
            m_pConfiguration->retain();

            this->setTexture(utils::get<cocos2d::CCTextureCache>()->addImage(m_pConfiguration->getAtlasName(), false));
            static_cast<T*>(this)->T::createFontChars();
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
        std::u16string m_string;
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

        /// @brief Create a FallbackBMFont with the default font and fallback font.
        static FallbackBMFont* create(const std::string& text);

        ~FallbackBMFont() override;

        bool init(const std::string& text, const std::string& font, const std::string& fallbackFont);

        void setString(std::string_view text) const;

        void setFntFile(std::string_view fntFile) const;

        void setColor(const cocos2d::ccColor3B& color) const;

        void setOpacity(GLubyte opacity) const;

        void limitLabelWidth(float width, float defaultScale, float minScale);

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

        void setString(std::string_view text) const;

        void setFntFile(std::string_view fntFile) const;

        void setColor(const cocos2d::ccColor3B& color) const;

        void setOpacity(GLubyte opacity) const;

        cocos2d::CCSpriteBatchNode* getSecondBatch() const { return m_emojiBatch; }

    protected:
        Label* m_label = nullptr;
        cocos2d::CCSpriteBatchNode* m_emojiBatch = nullptr;
    };


    /// @brief Label node that supports translation.
    class TranslatedLabel : public FallbackBMFont {
    public:
        static TranslatedLabel* create(std::string_view key);
    };

}
