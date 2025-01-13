#pragma once
#include <string>
#include <unordered_map>
#include <vector>

struct BMKerningPair {
    uint32_t first = 0;
    uint32_t second = 0;

    bool operator==(const BMKerningPair& other) const = default;

    uint64_t toInt() const {
        return static_cast<uint64_t>(first) << 32 | second;
    }
};

struct BMFontDef {
    uint32_t charID = 0;
    cocos2d::CCRect rect = {0, 0, 0, 0};
    float xOffset = 0;
    float yOffset = 0;
    float xAdvance = 0;
};

struct BMFontPadding {
    int left = 0, top = 0, right = 0, bottom = 0;
};

template <>
struct std::hash<BMKerningPair> {
    size_t operator()(BMKerningPair const& pair) const noexcept {
        return std::hash<uint64_t>()(pair.toInt());
    }
};

/// @brief Reimplementation of the CCBMFontConfiguration class, with a few modifications to make it more modern.
class BMFontConfiguration {
public:
    static BMFontConfiguration* create(std::string_view fntFile);
    BMFontConfiguration() = default;

protected:
    bool initWithFNTfile(std::string_view fntFile);
    bool initWithContents(std::string const& contents, std::string const& fntFile);

private:
    bool parseInfoArguments(std::istringstream& line);
    bool parseImageFileName(std::istringstream& line, std::string const& fntFile);
    bool parseCommonArguments(std::istringstream& line);
    bool parseCharacterDefinition(std::istringstream& line);
    bool parseKerningEntry(std::istringstream& line);

public:
    std::unordered_map<uint32_t, BMFontDef> const& getFontDefDictionary() const { return m_fontDefDictionary; }
    std::unordered_map<BMKerningPair, int> const& getKerningDictionary() const { return m_kerningDictionary; }
    int getCommonHeight() const { return m_commonHeight; }
    BMFontPadding const& getPadding() const { return m_padding; }
    std::string const& getAtlasName() const { return m_atlasName; }

protected:
    std::unordered_map<uint32_t, BMFontDef> m_fontDefDictionary;
    std::unordered_map<BMKerningPair, int> m_kerningDictionary;
    int m_commonHeight = 0;
    BMFontPadding m_padding;
    std::string m_atlasName;
};

std::u32string utf8_to_utf32(std::string_view text);
std::string utf32_to_utf8(std::u32string_view text);

enum class BMFontAlignment {
    Left,
    Center,
    Right,
    Justify // TODO: implement justify
};

/// @brief Multifunctional label node, that is more optimized and feature complete than the available CCLabelBMFont/TextArea ones.
/// Supports features like line wrapping, multiple fonts, batched emojis and more.
class Label : public cocos2d::CCNode, public cocos2d::CCRGBAProtocol, public cocos2d::CCLabelProtocol {
public:
    /// @brief Create a label with text and bitmap font file.
    static Label* create(std::string_view text, std::string_view font);

    /// @brief Create a label with text, bitmap font file, and scale.
    static Label* create(std::string_view text, std::string_view font, float scale);

    /// @brief Create a label with text, bitmap font file, and alignment.
    static Label* create(std::string_view text, std::string_view font, BMFontAlignment alignment);

    /// @brief Create a label with text, bitmap font file, alignment, and scale.
    static Label* create(std::string_view text, std::string_view font, BMFontAlignment alignment, float scale);

    /// @brief Create a wrapped label with text, bitmap font file, scale and wrap width.
    static Label* createWrapped(std::string_view text, std::string_view font, float wrapWidth);

    /// @brief Create a wrapped label with text, bitmap font file, alignment, scale and wrap width.
    static Label* createWrapped(std::string_view text, std::string_view font, float scale, float wrapWidth);

    /// @brief Create a wrapped label with text, bitmap font file, alignment, and wrap width.
    static Label* createWrapped(std::string_view text, std::string_view font, BMFontAlignment alignment, float wrapWidth);

    /// @brief Create a wrapped label with text, bitmap font file, alignment, scale and wrap width.
    static Label* createWrapped(std::string_view text, std::string_view font, BMFontAlignment alignment, float scale, float wrapWidth);

public:
    using EmojiMap = std::unordered_map<std::u32string_view, const char*>;

    /// @brief Set the contents of the label.
    void setString(std::string_view text);
    /// @brief Get the contents of the label.
    [[nodiscard]] std::string const& getString() const { return m_text; }
    /// @brief Set the primary font of the label.
    void setFont(std::string_view font);
    /// @brief Add additional font to the label. (for multi-font labels)
    void addFont(std::string_view font, std::optional<float> scale = std::nullopt);
    /// @brief Activate support for emojis in the label.
    void enableEmojis(std::string_view sheetFileName, const EmojiMap* frameNames);
    /// @brief Enable or disable line wrapping.
    void setWrapEnabled(bool enabled);
    /// @brief Set the wrap width of the label.
    void setWrapWidth(float width);
    /// @brief Enable or disable line wrapping and set the wrap width.
    void setWrap(bool enabled, float width);
    /// @brief Whether emojis should inherit the color of the label.
    void enableEmojiColors(bool enabled);
    /// @brief Get the primary font atlas name.
    [[nodiscard]] std::string const& getFont() const { return m_font; }
    /// @brief Get the alignment of the label.
    [[nodiscard]] BMFontAlignment getAlignment() const { return m_alignment; }
    /// @brief Set the alignment of the label.
    void setAlignment(BMFontAlignment alignment);
    /// @brief Resize the label to fit the width.
    void limitLabelWidth(float width, float defaultScale, float minScale);

protected:
    static int kerningAmountForChars(uint32_t first, uint32_t second, const BMFontConfiguration* config);

    /// @brief Hide all characters of the label.
    void hideAllChars() const;

    /// @brief Update the characters of the label when it is not left-aligned.
    void updateAlignment() const;

    static float getWordWidth(std::vector<cocos2d::CCSprite*> const& word);

    /// @brief Update the characters of the label when it is wrapped.
    /// Will calculate the line breaks and update the characters accordingly.
    void updateCharsWrapped();

    /// @brief Find the font definition for the specified character. [Internal]
    const BMFontDef* getFontDefForChar(
        char32_t c, const BMFontConfiguration* config,
        float& outScale, size_t& outIndex,
        cocos2d::CCSpriteBatchNode*& outBatch,
        BMFontConfiguration*& outConfig
    ) const;

    std::u32string_view parseEmoji(std::u32string_view text, uint32_t& index) const;

    /// @brief Check for an emoji character and add it to the label if found. [Internal]
    void checkForEmoji(
        std::u32string_view text, uint32_t& index,
        float scaleFactor, int& nextX, int nextY, int commonHeight,
        int& longestLine, std::vector<cocos2d::CCSprite*>& currentLine,
        size_t& emojiIndex
    ) const;

    /// @brief Fetches or creates a sprite with the provided rect. [Internal]
    cocos2d::CCSprite* getSpriteForChar(
        cocos2d::CCSpriteBatchNode* batch, size_t index,
        float scale, cocos2d::CCRect const& rect
    ) const;

    /// @brief Update the characters of the label when it is not wrapped.
    /// Automatically calculates the line breaks and updates the characters accordingly.
    void updateChars();

    /// @brief Update the colors of all characters.
    void updateColors() const;

    /// @brief Update the opacity of all characters.
    void updateOpacity() const;

public:
    /// === CCRGBAProtocol ===

    void setColor(cocos2d::ccColor3B const& color) override {
        m_color = color;
        updateColors();
    }

    void setOpacity(GLubyte opacity) override {
        m_opacity = opacity;
        updateOpacity();
    }

    cocos2d::ccColor3B const& getColor() override { return m_color; }
    cocos2d::ccColor3B const& getDisplayedColor() override { return m_color; }
    GLubyte getDisplayedOpacity() override { return m_opacity; }
    GLubyte getOpacity() override { return m_opacity; }
    void setOpacityModifyRGB(bool bValue) override { m_isOpacityModifyRGB = bValue; }
    bool isOpacityModifyRGB() override { return m_isOpacityModifyRGB; }
    bool isCascadeColorEnabled() override { return false; }
    void setCascadeColorEnabled(bool cascadeColorEnabled) override {}
    void updateDisplayedColor(cocos2d::ccColor3B const& color) override {}
    bool isCascadeOpacityEnabled() override { return false; }
    void setCascadeOpacityEnabled(bool cascadeOpacityEnabled) override {}
    void updateDisplayedOpacity(GLubyte opacity) override {}

    /// === CCLabelProtocol ===

    void setString(const char* label) override { this->setString(std::string_view(label)); }
    const char* getString() override { return m_text.c_str(); }

protected:
    bool init(std::string_view text, std::string_view font, BMFontAlignment alignment, float scale);
    bool initWrapped(std::string_view text, std::string_view font, BMFontAlignment alignment, float scale, float wrapWidth);

protected:
    // Protocol properties
    cocos2d::ccColor3B m_color = cocos2d::ccc3(255, 255, 255);
    GLubyte m_opacity = 255;
    bool m_isOpacityModifyRGB = true;

    // Label properties
    std::string m_text;                                  // UTF-8 encoded text
    std::string m_font;                                  // primary font atlas name
    std::u32string m_unicodeText;                        // UTF-32 encoded text
    BMFontAlignment m_alignment = BMFontAlignment::Left; // text alignment
    BMFontConfiguration* m_fontConfig = nullptr;         // primary font configuration
    bool m_useWrap = false;                              // enable line wrapping
    bool m_useEmojiColors = false;                       // enable emoji colorization
    float m_wrapWidth = 0.f;                             // maximum scaled content width before wrapping
    int m_extraKerning = 0.f;                            // additional kerning between characters

    // Children
    struct FontCfg {
        BMFontConfiguration* config;       // font configuration
        cocos2d::CCSpriteBatchNode* batch; // corresponding batch node
        std::optional<float> scale;        // auto scale by default
    };

    cocos2d::CCSpriteBatchNode* m_mainBatch = nullptr;        // Primary font batch
    cocos2d::CCSpriteBatchNode* m_spriteSheetBatch = nullptr; // Sprite sheet batch for emoji characters
    std::vector<FontCfg> m_fontBatches;                       // Font batches for alternate fonts
    //  std::vector<cocos2d::CCSprite*> m_sprites;            // Classic sprites

    // Internal properties
    //  struct Chunk {
    //      std::u32string text;
    //      cocos2d::ccColor3B color = cocos2d::ccc3(255, 255, 255);
    //      float scale = 1.f;
    //      size_t fontIndex = 0;
    //      cocos2d::CCSprite* sprite = nullptr;
    //  };

    const EmojiMap* m_emojiMap = nullptr;                 // emoji map (MAP SHOULD BE GLOBAL AND NEVER DESTROYED)
    std::vector<std::vector<cocos2d::CCSprite*>> m_lines; // lines of characters
    //  std::vector<Chunk> m_chunks;                      // chunks containing metadata
    //  bool m_useChunks = false;                         // whether to use chunks instead of raw text
};