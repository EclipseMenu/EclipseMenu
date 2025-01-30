#pragma once

namespace eclipse::gui::cocos {
    class TranslatedLabel;

    class OneOfPicker : public cocos2d::CCMenu {
    public:
        static OneOfPicker* create(
            std::span<std::string> options,
            const std::function<void(int)>& callback,
            size_t index = 0
        );
        static OneOfPicker* create(
            std::span<const char*> options,
            const std::function<void(int)>& callback,
            size_t index = 0
        );

        void setSelected(size_t index);
        void setItems(std::vector<std::string>&& items);

    protected:
        bool init(
            std::span<std::string> options,
            const std::function<void(int)>& callback,
            size_t index
        );
        void onArrowClick(CCObject* sender);
        void updateValueLabel();

    protected:
        TranslatedLabel* m_valueLabel = nullptr;
        cocos2d::extension::CCScale9Sprite* m_background = nullptr;
        std::vector<std::string> m_options;
        std::function<void(int)> m_callback;
        size_t m_selected = 0;
    };
}