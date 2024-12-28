#include "FallbackBMFont.hpp"
#include <modules/utils/SingletonCache.hpp>

namespace eclipse::gui::cocos {

    void FallbackBMFont::Label::createFontChars() {
        int nextFontPositionX = 0;
        int nextFontPositionY = 0;
        uint16_t prev = -1;
        int kerningAmount = 0;

        cocos2d::CCSize tmpSize = {0, 0};

        int longestLine = 0;
        uint32_t totalHeight = 0;

        uint32_t quantityOfLines = 1;
        uint32_t stringLen = m_string.length();
        if (stringLen == 0) {
            return this->setContentSize({0, 0});
        }

        auto charset = m_pConfiguration->getCharacterSet();
        auto fallbackCharset = m_parent->m_fallbackConfiguration->getCharacterSet();

        for (uint32_t i = 0; i < stringLen - 1; ++i) {
            if (m_string[i] == '\n') {
                quantityOfLines++;
            }
        }

        totalHeight = m_pConfiguration->m_nCommonHeight * quantityOfLines;
        nextFontPositionY = -(m_pConfiguration->m_nCommonHeight - m_pConfiguration->m_nCommonHeight * quantityOfLines);

        cocos2d::CCRect rect;
        cocos2d::ccBMFontDef fontDef;

        auto scaleFactor = utils::get<cocos2d::CCDirector>()->getContentScaleFactor();

        uint32_t normalIndex = 0;
        uint32_t fallbackIndex = 0;

        for (uint32_t i = 0; i < stringLen; ++i) {
            auto c = m_string[i];
            if (c == '\n') {
                nextFontPositionX = 0;
                nextFontPositionY -= m_pConfiguration->m_nCommonHeight;
                continue;
            }

            bool usingFallback = false;

            // check if charset has the character
            if (!charset->contains(c)) {
                // check if it has uppercase version
                auto upper = std::toupper(c);
                if (!charset->contains(c)) {
                    // check if it has fallback font
                    if (fallbackCharset->contains(c)) {
                        usingFallback = true;
                    } else {
                        continue;
                    }
                } else {
                    c = upper;
                }
            }

            kerningAmount = this->kerningAmountForFirst(
                prev, c,
                usingFallback ? m_parent->m_fallbackConfiguration->m_pKerningDictionary : m_pConfiguration->m_pKerningDictionary
            );
            cocos2d::tCCFontDefHashElement* element = nullptr;

            uint32_t key = c;
            cocos2d::tCCFontDefHashElement* root = usingFallback ? m_parent->m_fallbackConfiguration->m_pFontDefDictionary : m_pConfiguration->m_pFontDefDictionary;
            HASH_FIND_INT(root, &key, element);
            if (!element) {
                continue;
            }

            fontDef = element->fontDef;
            rect = {
                fontDef.rect.origin.x / scaleFactor, fontDef.rect.origin.y / scaleFactor,
                fontDef.rect.size.width / scaleFactor, fontDef.rect.size.height / scaleFactor
            };

            rect.origin.x += m_tImageOffset.x;
            rect.origin.y += m_tImageOffset.y;

            cocos2d::CCSprite* fontChar;

            if (usingFallback) {
                fontChar = static_cast<cocos2d::CCSprite*>(m_spriteBatch->getChildByTag(fallbackIndex));
            } else {
                fontChar = static_cast<cocos2d::CCSprite*>(this->getChildByTag(normalIndex));
            }

            if (fontChar) {
                // Reusing previous Sprite
                fontChar->setVisible(true);

                // updating previous sprite
                fontChar->setTextureRect(rect, false, rect.size);
            }

            if (!fontChar) {
                fontChar = new cocos2d::CCSprite();
                fontChar->initWithTexture(
                    usingFallback ? m_spriteBatch->getTexture() : m_pobTextureAtlas->getTexture(),
                    rect
                );
                if (m_bIsBatched && !usingFallback) {
                    m_pTargetArray->addObject(fontChar);
                } else if (usingFallback) {
                    m_spriteBatch->addChild(fontChar, fallbackIndex, fallbackIndex);
                } else {
                    this->addChild(fontChar, normalIndex, normalIndex);
                }
                fontChar->release();

                // Apply label properties
                fontChar->setOpacityModifyRGB(m_bIsOpacityModifyRGB);

                // Color MUST be set before opacity, since opacity might change color if OpacityModifyRGB is on
                fontChar->updateDisplayedColor(m_tDisplayedColor);
                fontChar->updateDisplayedOpacity(m_cDisplayedOpacity);
            }

            if (usingFallback) fallbackIndex++;
            else normalIndex++;

            int yOffset = m_pConfiguration->m_nCommonHeight - fontDef.yOffset;
            cocos2d::CCPoint fontPos {
                static_cast<float>(nextFontPositionX) + fontDef.xOffset + fontDef.rect.size.width*0.5f + kerningAmount,
                static_cast<float>(nextFontPositionY) + yOffset - rect.size.height*0.5f * scaleFactor
            };
            fontChar->setPosition({
                fontPos.x / scaleFactor,
                fontPos.y / scaleFactor
            });

            // update kerning
            nextFontPositionX += m_nExtraKerning + fontDef.xAdvance + kerningAmount;
            prev = c;

            if (longestLine < nextFontPositionX) {
                longestLine = nextFontPositionX;
            }
        }

        if (fontDef.xAdvance < fontDef.rect.size.width) {
            tmpSize.width = longestLine + fontDef.rect.size.width - fontDef.xAdvance;
        } else {
            tmpSize.width = longestLine;
        }
        tmpSize.height = totalHeight;

        this->setContentSize({
            tmpSize.width / scaleFactor,
            tmpSize.height / scaleFactor
        });
    }

    FallbackBMFont* FallbackBMFont::create(const std::string& text, const std::string& font, const std::string& fallbackFont) {
        auto ret = new FallbackBMFont;
        ret->init(text, font, fallbackFont);
        ret->autorelease();
        return ret;
    }

    bool FallbackBMFont::init(const std::string& text, const std::string& font, const std::string& fallbackFont) {
        if (!CCNode::init()) return false;

        m_fallbackConfiguration = cocos2d::FNTConfigLoadFile(fallbackFont.c_str());
        m_fallbackConfiguration->retain();

        m_fallbackBatch = cocos2d::CCSpriteBatchNode::create(m_fallbackConfiguration->getAtlasName());
        m_fallbackBatch->setID("fallback-batch");
        this->addChild(m_fallbackBatch);

        m_label = Label::create(text, font, this);
        m_label->setID("label");
        m_label->setAnchorPoint({0, 0});
        this->addChild(m_label);

        this->setAnchorPoint({ 0.5, 0.5 });

        return true;
    }

    constexpr bool isEmojiHeader(uint16_t c) { return c >= 0xD83C && c <= 0xD83E; }
    constexpr bool isEmoji(uint16_t c) { return c >= 0x2600 && c <= 0x2BFF; }
    constexpr bool isEmojiModifier(uint16_t c) { return c == 0xFE0F; }

    static uint64_t getEmoji(const std::u16string& str, uint32_t& index, uint32_t len) {
        uint64_t emoji = str[index];
        if (isEmojiHeader(str[index])) {
            if (index + 1 >= len) return 0;
            emoji = (emoji << 16) | str[index + 1];
            index++;

            if (index + 1 < len && isEmojiModifier(str[index + 1])) {
                index++;
                emoji = (emoji << 16) | str[index];
            }

            return emoji;
        }

        if (isEmoji(emoji)) {
            if (index + 1 < len && isEmojiModifier(str[index + 1])) {
                index++;
                emoji = (emoji << 16) | str[index];
            }
            return emoji;
        }

        return 0;
    }

    void EmojiLabel::Label::createFontChars() {
        int nextFontPositionX = 0;
        int nextFontPositionY = 0;
        uint16_t prev = -1;
        int kerningAmount = 0;

        cocos2d::CCSize tmpSize = {0, 0};

        int longestLine = 0;
        uint32_t totalHeight = 0;

        uint32_t quantityOfLines = 1;
        uint32_t stringLen = m_string.length();
        if (stringLen == 0) {
            return this->setContentSize({0, 0});
        }

        auto charset = m_pConfiguration->getCharacterSet();

        for (uint32_t i = 0; i < stringLen - 1; ++i) {
            if (m_string[i] == '\n') {
                quantityOfLines++;
            }
        }

        totalHeight = m_pConfiguration->m_nCommonHeight * quantityOfLines;
        nextFontPositionY = -(m_pConfiguration->m_nCommonHeight - m_pConfiguration->m_nCommonHeight * quantityOfLines);

        cocos2d::CCRect rect;
        cocos2d::ccBMFontDef fontDef;

        auto scaleFactor = utils::get<cocos2d::CCDirector>()->getContentScaleFactor();

        uint32_t emojiIndex = 0;
        for (uint32_t i = 0; i < stringLen; ++i) {
            auto c = m_string[i];
            if (c == '\n') {
                nextFontPositionX = 0;
                nextFontPositionY -= m_pConfiguration->m_nCommonHeight;
                continue;
            }

            // check if charset has the character
            if (!charset->contains(c)) {
                auto emoji = getEmoji(m_string, i, stringLen);
                if (emoji != 0) {
                    auto it = g_emojis.find(emoji);
                    if (it != g_emojis.end()) {
                        auto frame = it->second;

                        auto sprite = static_cast<cocos2d::CCSprite*>(m_spriteBatch->getChildByTag(emojiIndex));
                        if (!sprite) {
                            // create sprite
                            sprite = cocos2d::CCSprite::createWithSpriteFrameName(frame.data());
                            m_spriteBatch->addChild(sprite, i, emojiIndex);
                            sprite->setOpacity(getOpacity());
                        } else {
                            // update sprite frame
                            auto spriteFrame = cocos2d::CCSpriteFrameCache::get()->spriteFrameByName(frame.data());
                            sprite->setTextureRect(spriteFrame->getRect(), spriteFrame->isRotated(), spriteFrame->getOriginalSize());
                            sprite->setVisible(true);
                        }

                        // calculate size
                        auto size = sprite->getContentSize();
                        auto sizeInPixels = cocos2d::CCSize {
                            size.width * scaleFactor,
                            size.height * scaleFactor
                        };

                        // rescale to fit font height
                        auto scale = m_pConfiguration->m_nCommonHeight / sizeInPixels.height;
                        sprite->setScale(scale);
                        sizeInPixels.width *= scale;

                        // update position
                        sprite->setPosition({
                            (nextFontPositionX + sizeInPixels.width / 2) / scaleFactor,
                            (nextFontPositionY + m_pConfiguration->m_nCommonHeight / 2) / scaleFactor
                        });
                        nextFontPositionX += sizeInPixels.width + m_nExtraKerning;

                        // update longest line
                        if (longestLine < nextFontPositionX) {
                            longestLine = nextFontPositionX;
                        }

                        emojiIndex++;
                    }

                    continue;
                }

                // check if it has uppercase version
                c = std::toupper(c);
                if (!charset->contains(c)) {
                    continue;
                }
            }

            kerningAmount = this->kerningAmountForFirst(prev, c);
            cocos2d::tCCFontDefHashElement* element = nullptr;

            uint32_t key = c;
            HASH_FIND_INT(m_pConfiguration->m_pFontDefDictionary, &key, element);
            if (!element) {
                continue;
            }

            fontDef = element->fontDef;
            rect = {
                fontDef.rect.origin.x / scaleFactor, fontDef.rect.origin.y / scaleFactor,
                fontDef.rect.size.width / scaleFactor, fontDef.rect.size.height / scaleFactor
            };

            rect.origin.x += m_tImageOffset.x;
            rect.origin.y += m_tImageOffset.y;

            cocos2d::CCSprite* fontChar;

            fontChar = static_cast<cocos2d::CCSprite*>(this->getChildByTag(i));
            if (fontChar) {
                // Reusing previous Sprite
                fontChar->setVisible(true);

                // updating previous sprite
                fontChar->setTextureRect(rect, false, rect.size);
            } else {
                fontChar = new cocos2d::CCSprite();
                fontChar->initWithTexture(m_pobTextureAtlas->getTexture(), rect);
                if (m_bIsBatched) {
                    m_pTargetArray->addObject(fontChar);
                } else {
                    addChild(fontChar, i, i);
                }
                fontChar->release();

                // Apply label properties
                fontChar->setOpacityModifyRGB(m_bIsOpacityModifyRGB);

                // Color MUST be set before opacity, since opacity might change color if OpacityModifyRGB is on
                fontChar->updateDisplayedColor(m_tDisplayedColor);
                fontChar->updateDisplayedOpacity(m_cDisplayedOpacity);
            }

            // See issue 1343. cast( signed short + unsigned integer ) == unsigned integer (sign is lost!)
            int yOffset = m_pConfiguration->m_nCommonHeight - fontDef.yOffset;
            cocos2d::CCPoint fontPos {
                static_cast<float>(nextFontPositionX) + fontDef.xOffset + fontDef.rect.size.width*0.5f + kerningAmount,
                static_cast<float>(nextFontPositionY) + yOffset - rect.size.height*0.5f * scaleFactor
            };
            fontChar->setPosition({
                fontPos.x / scaleFactor,
                fontPos.y / scaleFactor
            });

            // update kerning
            nextFontPositionX += m_nExtraKerning + fontDef.xAdvance + kerningAmount;
            prev = c;

            if (longestLine < nextFontPositionX) {
                longestLine = nextFontPositionX;
            }
        }

        if (fontDef.xAdvance < fontDef.rect.size.width) {
            tmpSize.width = longestLine + fontDef.rect.size.width - fontDef.xAdvance;
        } else {
            tmpSize.width = longestLine;
        }
        tmpSize.height = totalHeight;

        this->setContentSize({
            tmpSize.width / scaleFactor,
            tmpSize.height / scaleFactor
        });
    }

    EmojiLabel* EmojiLabel::create(const std::string& text, const std::string& font) {
        auto ret = new EmojiLabel;
        ret->init(text, font);
        ret->autorelease();
        return ret;
    }

    bool EmojiLabel::init(const std::string& text, const std::string& font) {
        CCNode::init();

        m_emojiBatch = cocos2d::CCSpriteBatchNode::create("emojis.png"_spr);
        m_emojiBatch->setID("emoji-batch");
        this->addChild(m_emojiBatch);

        m_label = Label::create(text, font, this);
        m_label->setAnchorPoint({0, 0});
        m_label->setPosition({0, 0});
        m_label->setID("label");
        this->addChild(m_label);

        return true;
    }
}
