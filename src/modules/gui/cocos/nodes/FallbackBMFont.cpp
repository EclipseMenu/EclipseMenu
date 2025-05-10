#include "FallbackBMFont.hpp"
#include <modules/i18n/translations.hpp>

namespace eclipse::gui::cocos {
    template <size_t N>
    struct EmojiToHexConverter {
        static constexpr size_t length = N - 1;
        static constexpr size_t modIdSize = sizeof(GEODE_MOD_ID) - 1;

        char32_t value[length]{};
        char filename[modIdSize + (length * 6 + 4)]{};

        constexpr EmojiToHexConverter(char32_t const (&str)[N]) {
            std::copy_n(str, N - 1, value);

            constexpr char hex[] = "0123456789abcdef";
            size_t index = 0;
            // add the mod id
            for (size_t i = 0; i < modIdSize; ++i) {
                filename[index++] = GEODE_MOD_ID[i];
            }
            filename[index++] = '/';

            // expand the emoji to hex
            for (size_t i = 0; i < length; ++i) {
                int c = str[i];

                // if last char is 0xfe0f, skip it
                if (c == 0xfe0f) {
                    // if this was a last char, remove the dash
                    if (filename[index - 1] == '-' && i == length - 1) {
                        index--;
                    }
                    continue;
                }

                if (c >= 0x10000) filename[index++] = hex[(c >> 16) & 0xF];
                if (c >= 0x1000) filename[index++] = hex[(c >> 12) & 0xF];
                if (c >= 0x100) filename[index++] = hex[(c >> 8) & 0xF];
                if (c >= 0x10) filename[index++] = hex[(c >> 4) & 0xF];
                filename[index++] = hex[c & 0xF];
                if (i < length - 1) filename[index++] = '-';
            }

            // add the extension
            filename[index++] = '.';
            filename[index++] = 'p';
            filename[index++] = 'n';
            filename[index] = 'g';
        }
    };

    template <EmojiToHexConverter S>
    constexpr std::pair<std::u32string_view, const char*> operator""_emoji() {
        return { std::u32string_view(S.value, S.length), S.filename };
    }

    static const Label::EmojiMap g_emojis = {
        // Twemoji emojis
        U"💯"_emoji, U"🤫"_emoji, U"🐻"_emoji, U"💔"_emoji, U"🐱"_emoji,
        U"🐶"_emoji, U"🦊"_emoji, U"🔫"_emoji, U"🤨"_emoji, U"🗿"_emoji,
        U"🔥"_emoji, U"🤓"_emoji, U"😱"_emoji, U"💀"_emoji, U"😈"_emoji,
        U"🗣️"_emoji, U"😭"_emoji, U"🤡"_emoji, U"👽"_emoji, U"👶"_emoji,
        U"🤖"_emoji, U"❤️"_emoji, U"👻"_emoji, U"🤯"_emoji, U"❄️"_emoji,
        U"🛌"_emoji, U"🧖"_emoji, U"🐟"_emoji, U"✨"_emoji, U"🎉"_emoji,
        U"🐷"_emoji, U"🌚"_emoji, U"🌞"_emoji, U"🌝"_emoji, U"🐵"_emoji,
        U"🙈"_emoji, U"🙉"_emoji, U"🙊"_emoji, U"👑"_emoji, U"🙏"_emoji,
        U"❓"_emoji, U"😎"_emoji, U"😍"_emoji,
        U"🇦"_emoji, U"🇧"_emoji, U"🇨"_emoji, U"🇩"_emoji,
        U"🇪"_emoji, U"🇫"_emoji, U"🇬"_emoji, U"🇭"_emoji,
        U"🇮"_emoji, U"🇯"_emoji, U"🇰"_emoji, U"🇱"_emoji,
        U"🇲"_emoji, U"🇳"_emoji, U"🇴"_emoji, U"🇵"_emoji,
        U"🇶"_emoji, U"🇷"_emoji, U"🇸"_emoji, U"🇹"_emoji,
        U"🇺"_emoji, U"🇻"_emoji, U"🇼"_emoji, U"🇽"_emoji,
        U"🇾"_emoji, U"🇿"_emoji,
        U"0️⃣"_emoji, U"1️⃣"_emoji, U"2️⃣"_emoji, U"3️⃣"_emoji,
        U"4️⃣"_emoji, U"5️⃣"_emoji, U"6️⃣"_emoji, U"7️⃣"_emoji,
        U"8️⃣"_emoji, U"9️⃣"_emoji,

        // Geometry Dash emojis
        U"✅"_emoji, U"❌"_emoji, U"❗"_emoji,
        U"👎"_emoji, U"👍"_emoji,
        U"⭐"_emoji, U"🌙"_emoji,
        U"⭕"_emoji, U"🟡"_emoji, // Gold Coin (empty/full)
        U"🛞"_emoji, U"🔵"_emoji, // User Coin (empty/full)
        U"♦️"_emoji, // Practice Mode Checkpoint
        U"🧿"_emoji, // StartPos
        U"💎"_emoji, // Diamond
        U"💠"_emoji, // Diamond Shard
        U"🔹"_emoji, // Orbs

        // Custom emojis
        U"🌗"_emoji, // Eclipse Logo
        U"🪩"_emoji, // Geode Logo
    };

    std::string TranslatedLabel::getCurrentAtlas() {
        return fmt::format("font_{}.fnt"_spr, i18n::getRequiredGlyphRangesString());
    }

    bool TranslatedLabel::isDefaultAtlas() {
        return i18n::getRequiredGlyphRangesString() == "default";
    }

    bool TranslatedLabel::init(std::string_view key) {
        return initRaw(i18n::get(key));
    }

    bool TranslatedLabel::initWrapped(std::string_view key, float wrapWidth, float scale) {
        return initWrappedRaw(i18n::get(key), wrapWidth, scale);
    }

    bool TranslatedLabel::initRaw(std::string_view text) {
        if (!isDefaultAtlas()) {
            this->addFont("font_default.fnt"_spr);
        }
        return Label::init(text, getCurrentAtlas(), BMFontAlignment::Left, 1.f);
    }

    bool TranslatedLabel::initWrappedRaw(std::string_view text, float wrapWidth, float scale) {
        if (!isDefaultAtlas()) {
            this->addFont("font_default.fnt"_spr);
        }
        return Label::initWrapped(text, getCurrentAtlas(), BMFontAlignment::Left, scale, wrapWidth);
    }

    bool EmojiLabel::init(std::string_view text, std::string const& font) {
        this->enableEmojis("emojis.png"_spr, &g_emojis);
        return Label::init(text, font, BMFontAlignment::Left, 1.f);
    }
}
