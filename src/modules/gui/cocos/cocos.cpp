#include "cocos.hpp"
#include <utils.hpp>

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
        if (!m_popup) return;

        if (!noCleanup)
            m_popup->removeFromParent();
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
}