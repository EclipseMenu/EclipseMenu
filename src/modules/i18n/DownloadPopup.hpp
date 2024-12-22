#pragma once
#include <Geode/utils/web.hpp>

namespace eclipse::gui::cocos {
    class FallbackBMFont;
}

namespace eclipse::i18n {

    class DownloadPopup : public geode::Popup<std::string const&> {
    protected:
        bool setup(std::string const& charset) override;
        void keyBackClicked() override {}
        void keyDown(cocos2d::enumKeyCodes key) override {}

        void setProgress(float progress);
        void startDownloadFile(std::filesystem::path const& path, std::string const& url);
        void handleFileDownloaded();
        void handleError(int code);

        std::string m_charset;
        int m_filesDownloaded = 0, m_totalFiles = 6;

        gui::cocos::FallbackBMFont* m_progressLabel = nullptr;
        cocos2d::CCSprite* m_progressBar = nullptr;
        cocos2d::CCSprite* m_progressBarBG = nullptr;

        geode::EventListener<geode::utils::web::WebTask> m_listener;

    public:
        static DownloadPopup* create(std::string const& charset);

    };


}
