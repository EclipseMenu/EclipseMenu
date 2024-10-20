#include "content-view.hpp"

#include <modules/gui/gui.hpp>

#include <modules/gui/cocos/components/ToggleComponent.hpp>
#include <modules/gui/cocos/components/ButtonComponent.hpp>

namespace eclipse::gui::cocos {

    ContentView* ContentView::create(cocos2d::CCSize const &size, const std::shared_ptr<MenuTab> &tab) {
        auto ret = new ContentView();
        if (ret->init(size, tab)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void ContentView::setContent(const std::shared_ptr<MenuTab>& tab) {
        this->loadContent(tab);
        m_contentLayer->scrollToTop();
    }

    bool ContentView::init(cocos2d::CCSize const &size, const std::shared_ptr<MenuTab> &tab) {
        if (!CCNode::init()) return false;
        //m_contentLayer = geode::ScrollLayer::create(size);
        m_contentLayer = ScrollLayer::create(size);

        this->setContentSize(size);
        this->setID("content-menu"_spr);

        this->addChild(m_contentLayer);
        this->setContent(tab);

        return true;
    }

    template<ComponentType Type>
    std::optional<std::shared_ptr<Component>> peekComponent(const std::vector<std::shared_ptr<Component>>& components, size_t index) {
        if (index >= components.size()) return std::nullopt;
        auto& component = components[index];
        if (component->getType() != Type) return std::nullopt;
        return component;
    }

    void ContentView::loadContent(const std::shared_ptr<MenuTab> &tab) const {
        auto layer = m_contentLayer->m_contentLayer;
        layer->removeAllChildrenWithCleanup(true);
        auto size = this->getContentSize();

        // Add the tab components
        for (size_t i = 0; i < tab->getComponents().size(); ++i) {
            auto& component = tab->getComponents()[i];
            switch (component->getType()) {
                case ComponentType::Label: {
                    // replace
                    auto label = cocos2d::CCLabelBMFont::create(component->getTitle().c_str(), "bigFont.fnt");
                    label->setAnchorPoint({ 0, 1 });
                    label->limitLabelWidth(size.width * 0.75f, 0.75f, 0.1f);
                    layer->addChild(label);
                } break;
                case ComponentType::Toggle: {
                    // Group two toggles into one row
                    if (auto component2 = peekComponent<ComponentType::Toggle>(tab->getComponents(), i + 1); component2) {
                        auto row = CCNode::create();
                        row->setContentWidth(this->getContentWidth());
                        row->addChild(ToggleComponentNode::create(component, size.width / 2));
                        row->addChild(ToggleComponentNode::create(*component2, size.width / 2));
                        row->setLayout(
                            cocos2d::RowLayout::create()
                                ->setAxisAlignment(cocos2d::AxisAlignment::End)
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
                    menu->setContentHeight(count * 30.5f);
                    menu->setLayout(
                        cocos2d::ColumnLayout::create()
                            ->setAxisReverse(true)
                            ->setAxisAlignment(cocos2d::AxisAlignment::End)
                            ->setGap(2.5f)
                    );
                    layer->addChild(menu);
                } break;
                default: break;
            }
        }

        layer->setLayout(
            cocos2d::ColumnLayout::create()
                ->setAxisReverse(true)
                ->setAutoGrowAxis(this->getContentHeight())
                ->setAxisAlignment(cocos2d::AxisAlignment::End)
                ->setGap(0)
        );
        m_contentLayer->fixTouchPrio();
        geode::Loader::get()->queueInMainThread([this, layer]() { // because i apparently have to do this to prevent crashes!?
            m_contentLayer->setTouchEnabled(layer->getContentHeight() > 260.0F);
        });
    }
}
