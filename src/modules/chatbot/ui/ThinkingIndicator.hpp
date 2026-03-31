#pragma once

namespace eclipse::ai {
    class ThinkingIndicator : public cocos2d::CCNode {
    public:
        static ThinkingIndicator* create();

        cocos2d::CCSize const& getContentSize() const override;

        bool isEnabled() const { return m_enabled; }
        void setEnabled(bool enabled);

    protected:
        bool init() override;

    private:
        cocos2d::CCLabelBMFont* m_label = nullptr;
        bool m_enabled = false;
    };
}