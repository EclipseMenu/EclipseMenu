#pragma once
#include <modules/gui/gui.hpp>
#include "popup/popup.hpp"

namespace eclipse::gui::cocos {

    class OptionsPopup;
    class ModalPopup;

    class CocosRenderer : public Renderer {
    public:
        static std::shared_ptr<CocosRenderer> get() {
            if (Engine::getRendererType() != RendererType::Cocos2d) return nullptr;
            return std::static_pointer_cast<CocosRenderer>(Engine::get()->getRenderer());
        }

        void init() override;
        void toggle() override;
        [[nodiscard]] bool isToggled() const override;
        void shutdown() override;
        void shutdown(bool noCleanup);
        [[nodiscard]] RendererType getType() const override { return RendererType::Cocos2d; }

        [[nodiscard]] Popup* getPopup() const { return m_popup; }
        bool isPartOfPopup(cocos2d::CCNode* node) const;
        void queueAfterDrawing(const std::function<void()>& func) override;
        void showPopup(const eclipse::Popup& popup) override;

        void registerOptionsPopup(OptionsPopup* popup) { m_optionsPopups.push_back(popup); }
        void unregisterOptionsPopup(OptionsPopup* popup) { std::erase(m_optionsPopups, popup); }
        void registerModal(ModalPopup* modal) { m_modals.push_back(modal); }
        void unregisterModal(ModalPopup* modal) { std::erase(m_modals, modal); }

    private:
        Popup* m_popup = nullptr;
        std::vector<OptionsPopup*> m_optionsPopups;
        std::vector<ModalPopup*> m_modals;
    };

}