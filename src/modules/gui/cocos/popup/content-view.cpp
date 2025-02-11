#include "content-view.hpp"

#include <modules/gui/gui.hpp>

#include <modules/gui/cocos/components/ButtonComponent.hpp>
#include <modules/gui/cocos/components/ColorComponent.hpp>
#include <modules/gui/cocos/components/ComboComponent.hpp>
#include <modules/gui/cocos/components/DirectoryComboComponent.hpp>
#include <modules/gui/cocos/components/FloatToggleComponent.hpp>
#include <modules/gui/cocos/components/InputFloatComponent.hpp>
#include <modules/gui/cocos/components/InputIntComponent.hpp>
#include <modules/gui/cocos/components/InputTextComponent.hpp>
#include <modules/gui/cocos/components/KeybindComponent.hpp>
#include <modules/gui/cocos/components/LabelSettingsComponent.hpp>
#include <modules/gui/cocos/components/RadioButtonMenuComponent.hpp>
#include <modules/gui/cocos/components/ToggleComponent.hpp>

#include <modules/gui/cocos/nodes/FallbackBMFont.hpp>
#include "scroll-layer.hpp"

namespace eclipse::gui::cocos {
    ContentView* ContentView::create(cocos2d::CCSize const& size, const std::shared_ptr<MenuTab>& tab) {
        auto ret = new ContentView();
        if (ret->init(size, tab)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void ContentView::setContent(const std::shared_ptr<MenuTab>& tab, bool resetScroll) const {
        this->loadContent(tab);
        if (resetScroll) m_contentLayer->scrollToTop();
    }

    bool ContentView::init(cocos2d::CCSize const& size, const std::shared_ptr<MenuTab>& tab) {
        if (!CCNode::init()) return false;
        //m_contentLayer = geode::ScrollLayer::create(size);
        m_contentLayer = ScrollLayer::create(size);

        this->setContentSize(size);
        this->setID("content-menu"_spr);

        this->addChild(m_contentLayer);
        this->setContent(tab);

        return true;
    }

    template <ComponentType Type>
    std::optional<std::shared_ptr<Component>> peekComponent(
        const std::vector<std::shared_ptr<Component>>& components, size_t index
    ) {
        if (index >= components.size()) return std::nullopt;
        auto& component = components[index];
        if (component->getType() != Type) return std::nullopt;
        return component;
    }

    void ContentView::loadContent(const std::shared_ptr<MenuTab>& tab) const {
        auto layer = m_contentLayer->m_contentLayer;
        layer->removeAllChildrenWithCleanup(true);

        auto size = this->getContentSize();

        // Add the tab components
        for (size_t i = 0; i < tab->getComponents().size(); ++i) {
            auto& component = tab->getComponents()[i];
            if (component->getFlags() & ComponentFlags::DisableCocos) continue;

            switch (component->getType()) {
                case ComponentType::Label: {
                    auto label = TranslatedLabel::create(component->getTitle());
                    label->setAnchorPoint({0, 1});
                    label->limitLabelWidth(size.width * 0.75f, 0.75f, 0.1f);
                    layer->addChild(label);
                } break;
                case ComponentType::Toggle: {
                    // Group two toggles into one row
                    if (auto component2 = peekComponent<ComponentType::Toggle>(tab->getComponents(), i + 1);
                        component2) {
                        auto row = CCNode::create();
                        row->setContentWidth(this->getContentWidth());
                        row->addChild(ToggleComponentNode::create(component, size.width / 2));
                        row->addChild(ToggleComponentNode::create(*component2, size.width / 2));
                        row->setLayout(
                            geode::RowLayout::create()
                                ->setAxisAlignment(geode::AxisAlignment::End)
                                ->setGap(0)
                        );
                        layer->addChild(row);
                        ++i;
                    } else {
                        layer->addChild(ToggleComponentNode::create(component, size.width));
                    }
                } break;
                case ComponentType::Button: {
                    // Group buttons into one menu with a column layout
                    auto menu = cocos2d::CCMenu::create();
                    int count = 1;
                    menu->addChild(ButtonComponentNode::create(component, size.width));
                    auto btn2 = peekComponent<ComponentType::Button>(tab->getComponents(), i + 1);
                    while (btn2) {
                        menu->addChild(ButtonComponentNode::create(*btn2, size.width));
                        btn2 = peekComponent<ComponentType::Button>(tab->getComponents(), ++i + 1);
                        ++count;
                    }
                    menu->setContentSize({ size.width, count * 30.5f });
                    menu->setLayout(
                        geode::ColumnLayout::create()
                            ->setAxisReverse(true)
                            ->setAutoScale(false)
                            ->setAutoGrowAxis(false)
                            ->setGrowCrossAxis(false)
                            ->setCrossAxisOverflow(false)
                            ->setAxisAlignment(geode::AxisAlignment::Center)
                            ->setGap(2.5f)
                    );
                    layer->addChild(menu);
                } break;
                case ComponentType::RadioButton: {
                    std::vector<std::shared_ptr<RadioButtonComponent>> radioComponents;
                    radioComponents.push_back(std::static_pointer_cast<RadioButtonComponent>(component));
                    auto radioComponent = peekComponent<ComponentType::RadioButton>(tab->getComponents(), i + 1);
                    while (radioComponent) {
                        radioComponents.push_back(std::static_pointer_cast<RadioButtonComponent>(*radioComponent));
                        radioComponent = peekComponent<ComponentType::RadioButton>(tab->getComponents(), ++i + 1);
                    }
                    layer->addChild(RadioButtonsMenuNode::create(radioComponents, size.width));
                } break;
                case ComponentType::LabelSettings: {
                    auto list = cocos2d::CCNode::create();
                    list->addChild(LabelSettingsComponentNode::create(component, size.width));
                    auto label2 = peekComponent<ComponentType::LabelSettings>(tab->getComponents(), i + 1);
                    size_t count = 1;
                    while (label2) {
                        list->addChild(LabelSettingsComponentNode::create(*label2, size.width));
                        label2 = peekComponent<ComponentType::LabelSettings>(tab->getComponents(), ++i + 1);
                        ++count;
                    }
                    list->setContentHeight(count * 37.5f);
                    list->setLayout(
                        geode::ColumnLayout::create()
                            ->setAutoScale(false)
                            ->setAxisReverse(true)
                            ->setAutoGrowAxis(std::nullopt)
                            ->setAxisAlignment(geode::AxisAlignment::Start)
                            ->setGap(1.5f)
                    );
                    layer->addChild(list);
                } break;
                case ComponentType::Color: {
                    layer->addChild(ColorComponentNode::create(component, size.width));
                } break;
                case ComponentType::FloatToggle: {
                    layer->addChild(FloatToggleComponentNode::create(component, size.width));
                } break;
                case ComponentType::InputFloat: {
                    layer->addChild(InputFloatComponentNode::create(component, size.width));
                } break;
                case ComponentType::InputInt: {
                    layer->addChild(InputIntComponentNode::create(component, size.width));
                } break;
                case ComponentType::InputText: {
                    layer->addChild(InputTextComponentNode::create(component, size.width));
                } break;
                case ComponentType::Keybind: {
                    layer->addChild(KeybindComponentNode::create(component, size.width));
                } break;
                case ComponentType::Combo: {
                    layer->addChild(ComboComponentNode::create(component, size.width));
                } break;
                case ComponentType::FilesystemCombo: {
                    layer->addChild(FilesystemComboComponentNode::create(component, size.width));
                } break;
                default: break;
            }
        }

        layer->setLayout(
            geode::ColumnLayout::create()
                ->setAutoScale(false)
                ->setAxisReverse(true)
                ->setAutoGrowAxis(this->getContentHeight())
                ->setAxisAlignment(geode::AxisAlignment::End)
                ->setGap(0),
            false
        );
        layer->updateLayout(false);
    }
}
