#pragma once
#include <modules/gui/gui.hpp>
#include "popup/popup.hpp"

namespace eclipse::gui::cocos {

    class CocosRenderer : public Renderer {
    public:
        static std::shared_ptr<CocosRenderer> get() {
            auto engine = Engine::get();
            if (engine->getRendererType() != RendererType::Cocos2d) return nullptr;
            return std::static_pointer_cast<CocosRenderer>(engine->getRenderer());
        }

        void init() override;
        void toggle() override;
        [[nodiscard]] bool isToggled() const override;
        void shutdown() override;
        void shutdown(bool noCleanup);

        [[nodiscard]] Popup* getPopup() const { return m_popup; }
        bool isPartOfPopup(cocos2d::CCNode* node) const;

    private:
        Popup* m_popup = nullptr;
    };

}