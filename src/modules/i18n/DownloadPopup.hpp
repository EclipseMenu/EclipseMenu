#pragma once
#include <Geode/utils/web.hpp>

namespace eclipse::gui::cocos {
    class TranslatedLabel;
}

namespace eclipse::i18n {
    class DownloadPopup : public geode::Popup {
    protected:
        bool init(std::string charset);
        void keyBackClicked() override {}
        void keyDown(cocos2d::enumKeyCodes key, double) override {}

        void setProgress(float progress);
        void startDownloadFile(std::filesystem::path path, std::string url);
        void handleFileDownloaded();
        void handleError(int code);

        std::string m_charset;
        int m_filesDownloaded = 0, m_totalFiles = 6;

        gui::cocos::TranslatedLabel* m_progressLabel = nullptr;
        cocos2d::CCSprite* m_progressBar = nullptr;
        cocos2d::CCSprite* m_progressBarBG = nullptr;

        geode::async::TaskHolder<geode::utils::web::WebResponse> m_listener;

    public:
        static DownloadPopup* create(std::string charset);
    };
}
