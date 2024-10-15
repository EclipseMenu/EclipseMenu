#include "cocos.hpp"
#include <utils.hpp>

#include "popup/options-popup.hpp"

namespace eclipse::gui::cocos {

    void CocosRenderer::init() {}

    void CocosRenderer::toggle() {
        if (m_popup) return shutdown();

        m_popup = Popup::create(Engine::get()->getTabs());
        m_popup->show();
        utils::updateCursorState(isToggled());
    }

    bool CocosRenderer::isToggled() const {
        return m_popup != nullptr;
    }

    void CocosRenderer::shutdown() {
        shutdown(false);
    }

    void CocosRenderer::shutdown(bool noCleanup) {
        for (auto popup : m_optionsPopups)
            popup->removeFromParentAndCleanup(true);
        m_optionsPopups.clear();

        if (!m_popup) return;

        if (!noCleanup)
            m_popup->removeFromParentAndCleanup(true);
        m_popup = nullptr;
        utils::updateCursorState(isToggled());
    }

    bool CocosRenderer::isPartOfPopup(cocos2d::CCNode* node) const {
        if (!m_popup) return false;
        return m_popup->isAncestorOf(node);
    }

    //since cocos is retained mode, extra work might not be needed
    //might need to change in the future IDK
    void CocosRenderer::queueAfterDrawing(const std::function<void()>& func) {
        func();
    }

    void CocosRenderer::showPopup(const eclipse::Popup &popup) {
        if (popup.isPrompt()) return; // TODO: Implement prompt

        geode::createQuickPopup(
            popup.getTitle().c_str(), popup.getMessage().c_str(),
            popup.getButton1().c_str(),
            popup.getButton2().empty() ? nullptr : popup.getButton2().c_str(),
            [callback = popup.getCallback()](auto, bool result) { callback(result); }
        )->show();
    }
}
