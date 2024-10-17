#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/SliderTouchLogic.hpp>
#include <Geode/modify/GJScaleControl.hpp>

namespace eclipse::hacks::Creator {

    // TODO: bypass certain sliders in editor
    class SliderLimit : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Creator");

            tab->addToggle("Slider Limit", "creator.sliderlimit")
                    ->handleKeybinds()
                    ->setDescription("Removes the min/max limit on sliders.");
        }

        [[nodiscard]] const char* getId() const override { return "Slider Limit"; }
    };

    REGISTER_HACK(SliderLimit)

    class $modify(SliderLimitSTLHook, SliderTouchLogic) {
        ALL_DELEGATES_AND_SAFE_PRIO("creator.sliderlimit")

        void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override {
            // We reimplement the function with this one :fire:
            auto touchPos = this->convertTouchToNodeSpace(touch);
            auto position = touchPos - this->m_position;
            // auto delta = this->m_length / 2.f;
            if (this->m_rotated) {
                // auto clamped = std::clamp(position.y, -delta, delta);
                auto clamped = position.y; // Remove the clamping
                this->m_thumb->setPosition({ 0.f, clamped });
            } else {
                // auto clamped = std::clamp(position.x, -delta, delta);
                auto clamped = position.x; // Remove the clamping
                this->m_thumb->setPosition({ clamped, 0.f });
            }

            if (this->m_activateThumb)
                this->m_thumb->activate();

            if (auto* slider = this->m_slider)
                slider->updateBar();
        }
    };

    class $modify (SliderLimitGJSHook, GJScaleControl) {
        ADD_HOOKS_DELEGATE("creator.sliderlimit")

        void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
            GJScaleControl::ccTouchMoved(touch, event);

            if (m_sliderXY && m_sliderXY->m_touchLogic->m_activateThumb) {
                m_sliderXY->getThumb()->setPositionX(this->convertToNodeSpace(touch->getLocation()).x);
                m_sliderXY->updateBar();

                float value = m_sliderXY->getThumb()->getValue();

                updateLabelXY(value);
                this->sliderChanged(m_sliderXY->getThumb());

                if (EditorUI::get()) EditorUI::get()->scaleXYChanged(value, value, m_scaleLocked);
            }
        }
    };

}
