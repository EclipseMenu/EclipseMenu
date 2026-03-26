#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GJScaleControl.hpp>
#include <Geode/modify/SliderTouchLogic.hpp>

namespace eclipse::hacks::Creator {
    // TODO: bypass certain sliders in editor
    class $hack(SliderLimit) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.creator");
            tab->addToggle("creator.sliderlimit")->handleKeybinds()->setDescription();
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
                this->m_thumb->setPosition({0.f, clamped});
            } else {
                // auto clamped = std::clamp(position.x, -delta, delta);
                auto clamped = position.x; // Remove the clamping
                this->m_thumb->setPosition({clamped, 0.f});
            }

            if (this->m_activateThumb)
                this->m_thumb->activate();

            if (auto* slider = this->m_slider)
                slider->updateBar();
        }
    };

    class $modify(SliderLimitGJSHook, GJScaleControl) {
        ADD_HOOKS_DELEGATE("creator.sliderlimit")

        void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override {
            if (touch->m_nId != m_touchID) return;

            Slider* slider = m_sliderXY;
            auto labelFunc = &GJScaleControl::updateLabelXY;
            auto delegateFunc = +[](GJScaleControlDelegate* delegate, float x, float y, bool locked) {
                delegate->scaleXYChanged(x, y, locked);
            };
            switch (m_scaleButtonType) {
                case ScaleButtonType::X:
                    slider = m_sliderX;
                    labelFunc = &GJScaleControl::updateLabelX;
                    delegateFunc = +[](GJScaleControlDelegate* delegate, float x, float y, bool locked) {
                        delegate->scaleXChanged(x, locked);
                    };
                    break;
                case ScaleButtonType::Y:
                    slider = m_sliderY;
                    labelFunc = &GJScaleControl::updateLabelY;
                    delegateFunc = +[](GJScaleControlDelegate* delegate, float x, float y, bool locked) {
                        delegate->scaleYChanged(y, locked);
                    };
                    break;
                default:
                    break;
            };

            if (slider && slider->m_touchLogic->m_activateThumb) {
                slider->getThumb()->setPositionX(this->convertToNodeSpace(touch->getLocation()).x);
                slider->updateBar();

                auto const thumbValue = slider->getThumb()->getValue();
                auto const scaleValue = m_lowerBound + thumbValue * (m_upperBound - m_lowerBound);
                auto const roundedValue = std::round(scaleValue * 100.f) / 100.f;
                auto xValue = roundedValue;
                auto yValue = roundedValue;

                // this is the ratio preserving code that i stole from ai :D
                if (m_scaleButtonType == ScaleButtonType::XY) {
                    if (m_valueX != m_valueY) {
                        if (m_valueX <= m_valueY) {
                            xValue = (roundedValue / m_valueY) * m_valueX;
                        } else {
                            yValue = (roundedValue / m_valueX) * m_valueY;
                        }
                    }
                }

                m_changedValueX = xValue;
                m_changedValueY = yValue;
                std::invoke(labelFunc, this, roundedValue);
                this->sliderChanged(slider->getThumb());

                if (m_delegate) {
                    std::invoke(delegateFunc, m_delegate, xValue, yValue, m_scaleLocked);
                }
            }
        }
    };

    class $modify(SliderLimitEUIHook, EditorUI) {
        ADD_HOOKS_DELEGATE("creator.sliderlimit")

        void scaleObjects(cocos2d::CCArray* objects, float scaleX, float scaleY, cocos2d::CCPoint pivotPoint, ObjectScaleType type, bool lockMove) {
            if (pivotPoint == cocos2d::CCPoint{0.f, 0.f}) {
                pivotPoint = this->getGroupCenter(objects, false);
            }

            for (auto obj : geode::cocos::CCArrayExt<GameObject*>(objects)) {
                // i hope you like the variable names this is how i handle math (scary)
                auto const& savedValues = m_objectEditorStates[obj->m_uniqueID];
                auto const newX = std::max(savedValues.m_scaleX * scaleX * obj->m_pixelScaleX, 0.01f);
                auto const newY = std::max(savedValues.m_scaleY * scaleY * obj->m_pixelScaleY, 0.01f);
                auto const pos = obj->getPosition();

                auto const currentOffset = (pos - pivotPoint);
                auto const normalizedOffset = currentOffset / cocos2d::CCPoint{obj->m_scaleX, obj->m_scaleY};
                auto const newOffset = normalizedOffset * cocos2d::CCPoint{newX, newY};

                auto moveOffset = cocos2d::CCPoint{0.f, 0.f};

                if (type != ObjectScaleType::Y) {
                    obj->updateCustomScaleX(newX);
                    moveOffset.x = (newOffset - currentOffset).x;
                }
                if (type != ObjectScaleType::X) {
                    obj->updateCustomScaleY(newY);
                    moveOffset.y = (newOffset - currentOffset).y;
                }
                if (!lockMove) {
                    this->moveObject(obj, moveOffset);
                }
            }
        }
    };
}
