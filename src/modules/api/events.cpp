#include <eclipse.hpp>
#include <modules/gui/gui.hpp>
#include <modules/config/config.hpp>

namespace eclipse::api {
using namespace geode::prelude;

template <config::SupportedType T>
void createGetConfigListener() {
    new EventListener<EventFilter<events::RequestConfigValueEvent<T>>>(+[](events::RequestConfigValueEvent<T>* e) {
        auto useInternal = e->getUseInternal();
        auto exists = useInternal ? config::has(e->getKey()) : config::hasTemp(e->getKey());
        if (!exists) return ListenerResult::Stop;
        e->setValue(useInternal ? config::get<T>(e->getKey()) : config::getTemp<T>(e->getKey()));
        return ListenerResult::Stop;
    });
}

template <config::SupportedType T>
void createSetConfigListener() {
    new EventListener<EventFilter<events::SetConfigValueEvent<T>>>(+[](events::SetConfigValueEvent<T>* e) {
        if (e->getUseInternal()) {
            config::set<T>(e->getKey(), e->getValue());
        } else {
            config::setTemp<T>(e->getKey(), e->getValue());
        }
        return ListenerResult::Stop;
    });
}

$execute {
    new EventListener<EventFilter<events::CreateMenuTabEvent>>(+[](events::CreateMenuTabEvent* e) {
        gui::MenuTab::find(e->getName());
        return ListenerResult::Stop;
    });

    /* Components */
    new EventListener<EventFilter<events::AddLabelEvent>>(+[](events::AddLabelEvent* e) {
        auto tab = gui::MenuTab::find(e->getTabName());
        auto label = tab->addLabel(e->getTitle());
        e->setUniqueID(label->getUID());
        return ListenerResult::Stop;
    });
    new EventListener<EventFilter<events::AddToggleEvent>>(+[](events::AddToggleEvent* e) {
        auto tab = gui::MenuTab::find(e->getTabName());
        auto toggle = tab->addToggle(e->getTitle(), e->getID());
        toggle->callback([callback = std::get<0>(e->getCallbacks())](bool value) {
            std::invoke(callback, value);
        })->handleKeybinds()->disableSaving();
        e->setUniqueID(toggle->getUID());
        return ListenerResult::Stop;
    });

    /* Component Descriptions */
    new EventListener<EventFilter<events::SetComponentDescriptionEvent>>(+[](events::SetComponentDescriptionEvent* e) {
        auto component = gui::Component::find(e->getID());
        if (!component) return ListenerResult::Stop;
        component->setDescription(e->getDescription());
        return ListenerResult::Stop;
    });

    /* Config */
    createGetConfigListener<bool>();
    createGetConfigListener<int>();
    createGetConfigListener<float>();
    createGetConfigListener<std::string>();
    createSetConfigListener<bool>();
    createSetConfigListener<int>();
    createSetConfigListener<float>();
    createSetConfigListener<std::string>();
}

}
