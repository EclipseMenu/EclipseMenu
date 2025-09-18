#include "DownloadPopup.hpp"

#include <modules/gui/cocos/nodes/FallbackBMFont.hpp>
#include "translations.hpp"

namespace eclipse::i18n {
    bool DownloadPopup::setup(std::string const& charset) {
        this->setID("download-popup"_spr);
        m_closeBtn->setVisible(false);

        m_charset = charset;

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

    void DownloadPopup::startDownloadFile(std::filesystem::path const& path, std::string const& url) {
        using namespace geode::utils;

        m_listener.bind([this, path](web::WebTask::Event* e) {
            if (web::WebResponse* value = e->getValue()) {
                if (!value->ok()) {
                    this->handleError(value->code());
                    return;
                }

                auto data = value->data();
                auto res = file::writeBinary(path, data);
                if (res.isErr()) {
                    this->handleError(-1);
                    return;
                }

                this->m_filesDownloaded++;
                this->handleFileDownloaded(); // begin next download or close
            } else if (web::WebProgress* progress = e->getProgress()) {
                auto p = progress->downloadProgress().value_or(0.f);
                auto totalProgress = (m_filesDownloaded * 100.f + p) / m_totalFiles;
                this->setProgress(totalProgress / 100.f);
            }
        });

        m_listener.setFilter(web::WebRequest().get(url));
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
        this->startDownloadFile(path, url);
    }

    void DownloadPopup::handleError(int code) {
        m_progressLabel->setString(i18n::format("interface.font-download-fail", code));
        m_progressLabel->limitLabelWidth(200.f, 1.0f, 0.2f);
        m_progressLabel->setPositionY(m_progressLabel->getPositionY() + 5.f);
        m_progressBarBG->setVisible(false);

        auto btn = geode::cocos::CCMenuItemExt::createSpriteExtra(
            ButtonSprite::create("OK", 342, 0, 1.0f, false),
            [this](auto) {
                this->onClose(nullptr);
            }
        );
        btn->setID("button"_spr);

        auto menu = cocos2d::CCMenu::create();
        menu->setID("menu"_spr);
        menu->addChild(btn);
        menu->alignItemsHorizontallyWithPadding(10.f);
        m_mainLayer->addChildAtPosition(menu, geode::Anchor::Bottom, {0, 25});
    }

    DownloadPopup* DownloadPopup::create(std::string const& charset) {
        auto ret = new DownloadPopup();
        if (ret->initAnchored(240.f, 120.f, charset)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
}
