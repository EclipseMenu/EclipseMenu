#include "DownloadPopup.hpp"

#include <modules/gui/cocos/nodes/FallbackBMFont.hpp>
#include "translations.hpp"

namespace eclipse::i18n {
    bool DownloadPopup::init(std::string charset) {
        if (!Popup::init(240.f, 120.f)) {
            return false;
        }

        this->setID("download-popup"_spr);
        m_closeBtn->setVisible(false);

        m_charset = std::move(charset);

        auto label = gui::cocos::TranslatedLabel::create("interface.font-download");
        label->setID("title"_spr);
        m_mainLayer->addChildAtPosition(label, geode::Anchor::Top, {0, -20});

        m_progressLabel = gui::cocos::TranslatedLabel::createRaw("0% (0/6)");
        m_progressLabel->setID("progress-label"_spr);
        m_mainLayer->addChildAtPosition(m_progressLabel, geode::Anchor::Center);

        m_progressBarBG = cocos2d::CCSprite::create("slidergroove_02.png");
        m_progressBarBG->setID("progress-bar-bg"_spr);

        m_progressBar = cocos2d::CCSprite::create("sliderBar.png");
        m_progressBar->setID("progress-bar"_spr);
        cocos2d::ccTexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE};
        m_progressBar->getTexture()->setTexParameters(&params);
        m_progressBar->setAnchorPoint({0, 0});
        m_progressBar->setPosition({2.f, 1.5f});
        m_progressBarBG->addChild(m_progressBar, -1);
        m_mainLayer->addChildAtPosition(m_progressBarBG, geode::Anchor::Bottom, {0, 20});

        m_totalFiles = BitmapFontsExtensions.size();
        this->handleFileDownloaded();

        return true;
    }

    void DownloadPopup::setProgress(float progress) {
        m_progressLabel->setString(fmt::format("{:.2f}% ({}/{})", progress * 100, m_filesDownloaded, m_totalFiles));
        m_progressBar->setTextureRect({
            0, 0,
            (m_progressBarBG->getContentWidth() - 4.f) * progress,
            m_progressBar->getContentHeight()
        });
    }

    void DownloadPopup::startDownloadFile(std::filesystem::path path, std::string url) {
        using namespace geode::utils;

        m_listener.spawn(
            web::WebRequest()
                .onProgress([this](web::WebProgress const& progress) {
                    auto p = progress.downloadProgress().value_or(0.f);
                    auto totalProgress = (m_filesDownloaded * 100.f + p) / m_totalFiles;
                    this->setProgress(totalProgress / 100.f);
                })
                .get(std::move(url)),
            [this, path = std::move(path)](web::WebResponse response) {
                if (!response.ok()) {
                    this->handleError(response.code());
                    return;
                }

                auto res = response.into(path);
                if (res.isErr()) {
                    this->handleError(-1);
                    return;
                }

                this->m_filesDownloaded++;
                this->handleFileDownloaded(); // begin next download or close
            }
        );
    }


    void DownloadPopup::handleFileDownloaded() {
        if (m_filesDownloaded >= m_totalFiles) {
            return this->onClose(nullptr);
        }

        this->setProgress(static_cast<float>(m_filesDownloaded) / m_totalFiles);

        auto ext = BitmapFontsExtensions[m_filesDownloaded];
        auto path = geode::Mod::get()->getConfigDir() / "bmfonts" / GEODE_MOD_ID / fmt::format("font_{}{}", m_charset, ext);

        auto url = fmt::format(
            "https://raw.githubusercontent.com/EclipseMenu/EclipseMenu/refs/heads/main/resources/BitmapFonts/{}",
            path.filename()
        );
        this->startDownloadFile(std::move(path), std::move(url));
    }

    void DownloadPopup::handleError(int code) {
        m_progressLabel->setString(i18n::format("interface.font-download-fail", code));
        m_progressLabel->limitLabelWidth(200.f, 1.0f, 0.2f);
        m_progressLabel->setPositionY(m_progressLabel->getPositionY() + 5.f);
        m_progressBarBG->setVisible(false);

        auto btnSprite = ButtonSprite::create("OK", 120.f, 0, 1.0f, false);
        auto btn = CCMenuItemSpriteExtra::create(btnSprite, this, menu_selector(DownloadPopup::onClose));
        btn->setID("button"_spr);

        auto menu = cocos2d::CCMenu::create();
        menu->setID("menu"_spr);
        menu->addChild(btn);
        menu->alignItemsHorizontallyWithPadding(10.f);
        m_mainLayer->addChildAtPosition(menu, geode::Anchor::Bottom, {0, 25});
    }

    DownloadPopup* DownloadPopup::create(std::string charset) {
        auto ret = new DownloadPopup();
        if (ret->init(std::move(charset))) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
}
