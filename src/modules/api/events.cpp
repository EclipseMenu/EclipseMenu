#include <eclipse.hpp>
#include <modules/gui/gui.hpp>
#include <modules/config/config.hpp>
#include <modules/labels/variables.hpp>

namespace eclipse::api {
using namespace geode::prelude;

template <config::SupportedType T>
void createGetConfigListener() {
    new EventListener<EventFilter<events::RequestConfigValueEvent<T>>>(+[](events::RequestConfigValueEvent<T>* e) {
        auto useInternal = e->getUseInternal();
        auto res = useInternal ? config::get<T>(e->getKey()) : config::getTemp<T>(e->getKey());
        if (!res) return ListenerResult::Stop;
        e->setValue(res.unwrap());
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

template <label::SupportedType T>
void createGetRiftVariableListener() {
    new EventListener<EventFilter<events::GetRiftVariableEvent<T>>>(+[](events::GetRiftVariableEvent<T>* e) {
        auto val = labels::VariableManager::get().getVariable(std::string(e->getName()));

#define HANDLE_CASE(type) \
    if (val.is##type()) { e->setResult(Ok(val.get##type())); }\
    else { e->setResult(Err("Value is not a " #type)); }

        if constexpr (std::same_as<T, std::string>) {
            HANDLE_CASE(String)
        } else if constexpr (std::same_as<T, bool>) {
            HANDLE_CASE(Boolean)
        } else if constexpr (std::same_as<T, int64_t>) {
            HANDLE_CASE(Integer)
        } else if constexpr (std::same_as<T, double>) {
            HANDLE_CASE(Float)
        } else if constexpr (std::same_as<T, label::null_t>) {
            if (val.isNull()) e->setResult(Ok(std::monostate{}));
            else e->setResult(Err("Value is not null"));
        } else {
            e->setResult(Err("Unsupported type"));
        }

#undef HANDLE_CASE

        return ListenerResult::Stop;
    });
}

template <label::SupportedType T>
void createSetRiftVariableListener() {
    new EventListener<EventFilter<events::SetRiftVariableEvent<T>>>(+[](events::SetRiftVariableEvent<T>* e) {
        if constexpr (std::same_as<T, label::null_t>) {
            labels::VariableManager::get().removeVariable(std::string(e->getName()));
        } else {
            labels::VariableManager::get().setVariable(std::string(e->getName()), rift::Value::from<T>(e->getValue()));
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
    new EventListener<EventFilter<events::AddButtonEvent>>(+[](events::AddButtonEvent* e) {
        auto tab = gui::MenuTab::find(e->getTabName());
        auto button = tab->addButton(e->getTitle());
        button->callback([callback = std::get<0>(e->getCallbacks())] {
            std::invoke(callback);
        });
        e->setUniqueID(button->getUID());
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

    /* RIFT */
    new EventListener<EventFilter<events::FormatRiftStringEvent>>(+[](events::FormatRiftStringEvent* e) {
        e->setResult(rift::format(e->getSource(), labels::VariableManager::get().getVariables()));
        return ListenerResult::Stop;
    });
    createGetRiftVariableListener<std::string>();
    createGetRiftVariableListener<bool>();
    createGetRiftVariableListener<int64_t>();
    createGetRiftVariableListener<double>();
    createGetRiftVariableListener<label::null_t>();
    createSetRiftVariableListener<std::string>();
    createSetRiftVariableListener<bool>();
    createSetRiftVariableListener<int64_t>();
    createSetRiftVariableListener<double>();
    createSetRiftVariableListener<label::null_t>();
}

}
