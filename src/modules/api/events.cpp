#define ECLIPSE_DONT_DEFINE_IMPLS
#include <eclipse.hpp>
#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/labels/variables.hpp>

#include <modules/gui/components/button.hpp>
#include <modules/gui/components/input-float.hpp>
#include <modules/gui/components/label.hpp>
#include <modules/gui/components/toggle.hpp>

#include <memory>

#include "mods.hpp"

namespace eclipse::api {
using namespace geode::prelude;

StringMap<Function<bool()>> g_cheats;
StringMap<Function<bool()>>& getCheats() { return g_cheats; }

// TODO: geode v5
// template <config::SupportedType T>
// void createGetConfigListener() {
//     new EventListener<EventFilter<events::RequestConfigValueEvent<T>>>(+[](events::RequestConfigValueEvent<T>* e) {
//         auto useInternal = e->getUseInternal();
//         auto res = useInternal ? config::get<T>(e->getKey()) : config::getTemp<T>(e->getKey());
//         if (!res) return ListenerResult::Stop;
//         e->setValue(res.unwrap());
//         return ListenerResult::Stop;
//     });
// }
//
// template <config::SupportedType T>
// void createSetConfigListener() {
//     new EventListener<EventFilter<events::SetConfigValueEvent<T>>>(+[](events::SetConfigValueEvent<T>* e) {
//         if (e->getUseInternal()) {
//             config::set<T>(e->getKey(), e->getValue());
//         } else {
//             config::setTemp<T>(e->getKey(), e->getValue());
//         }
//         return ListenerResult::Stop;
//     });
// }
//
// template <label::SupportedType T>
// void createGetRiftVariableListener() {
//     new EventListener<EventFilter<events::GetRiftVariableEvent<T>>>(+[](events::GetRiftVariableEvent<T>* e) {
//         auto val = labels::VariableManager::get().getVariable(std::string(e->getName()));
//

//

//
// #undef HANDLE_CASE
//
//         return ListenerResult::Stop;
//     });
// }
//
// template <label::SupportedType T>
// void createSetRiftVariableListener() {
//     new EventListener<EventFilter<events::SetRiftVariableEvent<T>>>(+[](events::SetRiftVariableEvent<T>* e) {
//         if constexpr (std::same_as<T, label::null_t>) {
//             labels::VariableManager::get().removeVariable(std::string(e->getName()));
//         } else {
//             labels::VariableManager::get().setVariable(std::string(e->getName()), rift::Value::from<T>(e->getValue()));
//         }
//         return ListenerResult::Stop;
//     });
// }
//
// $execute {
//     new EventListener<EventFilter<events::CreateMenuTabEvent>>(+[](events::CreateMenuTabEvent* e) {
//         gui::MenuTab::find(e->getName());
//         return ListenerResult::Stop;
//     });
//
//     /* Components */
//     new EventListener<EventFilter<events::AddLabelEvent>>(+[](events::AddLabelEvent* e) {
//         auto tab = gui::MenuTab::find(e->getTabName());
//         auto label = tab->addLabel(e->getTitle());
//         e->setUniqueID(label->getUID());
//         return ListenerResult::Stop;
//     });
//     new EventListener<EventFilter<events::AddToggleEvent>>(+[](events::AddToggleEvent* e) {
//         auto tab = gui::MenuTab::find(e->getTabName());
//         auto toggle = tab->addToggle(e->getTitle(), e->getID());
//         toggle->callback([callback = std::get<0>(e->getCallbacks())](bool value) {
//             std::invoke(callback, value);
//         })->handleKeybinds()->disableSaving();
//         e->setUniqueID(toggle->getUID());
//         return ListenerResult::Stop;
//     });
//     new EventListener<EventFilter<events::AddButtonEvent>>(+[](events::AddButtonEvent* e) {
//         auto tab = gui::MenuTab::find(e->getTabName());
//         auto button = tab->addButton(e->getTitle());
//         button->callback([callback = std::get<0>(e->getCallbacks())] {
//             std::invoke(callback);
//         });
//         e->setUniqueID(button->getUID());
//         return ListenerResult::Stop;
//     });
//     new EventListener<EventFilter<events::AddInputFloatEvent>>(+[](events::AddInputFloatEvent* e) {
//         auto tab = gui::MenuTab::find(e->getTabName());
//         auto input = tab->addInputFloat(e->getTitle(), e->getID());
//         input->callback([callback = std::get<0>(e->getCallbacks())](float value) {
//             std::invoke(callback, value);
//         })->disableSaving();
//         e->setUniqueID(input->getUID());
//         return ListenerResult::Stop;
//     });
//
//     /* Special Components */
//     new EventListener<EventFilter<events::SetLabelTextEvent>>(+[](events::SetLabelTextEvent* e) {
//         auto label = gui::Component::find(e->getID());
//         if (!label || label->getType() != gui::ComponentType::Label) return ListenerResult::Stop;
//         static_cast<gui::LabelComponent*>(label)->setText(e->getText());
//         return ListenerResult::Stop;
//     });
//     new EventListener<EventFilter<events::SetInputFloatParamsEvent>>(+[](events::SetInputFloatParamsEvent* e) {
//         auto input = gui::Component::find(e->getID());
//         if (!input || input->getType() != gui::ComponentType::InputFloat) return ListenerResult::Stop;
//         auto inputFloat = static_cast<gui::InputFloatComponent*>(input);
//         if (e->getMin()) inputFloat->setMin(e->getMin().value());
//         if (e->getMax()) inputFloat->setMax(e->getMax().value());
//         if (e->getFormat()) inputFloat->setFormat(e->getFormat().value());
//         return ListenerResult::Stop;
//     });
//
//     /* Component Descriptions */
//     new EventListener<EventFilter<events::SetComponentDescriptionEvent>>(+[](events::SetComponentDescriptionEvent* e) {
//         auto component = gui::Component::find(e->getID());
//         if (!component) return ListenerResult::Stop;
//         component->setDescription(e->getDescription());
//         return ListenerResult::Stop;
//     });
//
//     /* Config */
//     createGetConfigListener<bool>();
//     createGetConfigListener<int>();
//     createGetConfigListener<float>();
//     createGetConfigListener<std::string>();
//     createSetConfigListener<bool>();
//     createSetConfigListener<int>();
//     createSetConfigListener<float>();
//     createSetConfigListener<std::string>();
//
//     /* RIFT */
//     new EventListener<EventFilter<events::FormatRiftStringEvent>>(+[](events::FormatRiftStringEvent* e) {
//         auto res = rift::format(e->getSource(), labels::VariableManager::get().getVariables());
//         if (res.isErr()) {
//             e->setResult(res.unwrapErr().message());
//         } else {
//             e->setResult(res.unwrap());
//         }
//         return ListenerResult::Stop;
//     });
//     createGetRiftVariableListener<std::string>();
//     createGetRiftVariableListener<bool>();
//     createGetRiftVariableListener<int64_t>();
//     createGetRiftVariableListener<double>();
//     createGetRiftVariableListener<label::null_t>();
//     createGetRiftVariableListener<matjson::Value>();
//     createSetRiftVariableListener<std::string>();
//     createSetRiftVariableListener<bool>();
//     createSetRiftVariableListener<int64_t>();
//     createSetRiftVariableListener<double>();
//     createSetRiftVariableListener<label::null_t>();
//     createSetRiftVariableListener<matjson::Value>();
//
//     /* Modules */
//     new EventListener<EventFilter<events::RegisterCheatEvent>>(+[](events::RegisterCheatEvent* e) {
//         g_cheats[e->getName()] = e->getCallback();
//         return ListenerResult::Stop;
//     });
//
//     new EventListener<EventFilter<events::CheckCheatsEnabledEvent>>(+[](events::CheckCheatsEnabledEvent* e) {
//         e->setResult(config::getTemp<"hasCheats", bool>(false));
//         return ListenerResult::Stop;
//     });
//
//     new EventListener<EventFilter<events::CheckCheatedInAttemptEvent>>(+[](events::CheckCheatedInAttemptEvent* e) {
//         e->setResult(config::getTemp<"trippedSafeMode", bool>(false));
//         return ListenerResult::Stop;
//     });
// }

std::string formatRiftString(std::string_view source) {
    auto res = rift::format(source, labels::VariableManager::get().getVariables());
    if (res.isErr()) {
        return std::string(res.unwrapErr().message());
    }
    return std::move(res).unwrap();
}

template <config::SupportedType T>
void setRiftVariable(std::string name, T value) {
    if constexpr (std::same_as<T, label::null_t>) {
         labels::VariableManager::get().removeVariable(std::move(name));
    } else {
        labels::VariableManager::get().setVariable(std::move(name), std::move(value));
    }
}

template <config::SupportedType T>
geode::Result<T> getRiftVariable(std::string_view name) {
    auto val = labels::VariableManager::get().getVariable(std::string(name));

    #define HANDLE_CASE(type) \
    if (val.is##type()) { return geode::Ok(val.get##type()); } \
    else { return geode::Err("Value is not of type " #type); }

    if constexpr (std::same_as<T, std::string>) {
        HANDLE_CASE(String)
    } else if constexpr (std::same_as<T, bool>) {
        HANDLE_CASE(Boolean)
    } else if constexpr (std::same_as<T, int64_t>) {
        HANDLE_CASE(Integer)
    } else if constexpr (std::same_as<T, double>) {
        HANDLE_CASE(Float)
    } else if constexpr (std::same_as<T, label::null_t>) {
        if (val.isNull()) return Ok(std::nullptr_t{});
        return Err("Value is not null");
    } else if constexpr (std::same_as<T, matjson::Value>) {
        return Ok(val.toJson());
    } else {
        return Err("Unsupported type");
    }

    #undef HANDLE_CASE
}

$execute {
    __internal__::FetchVTableEvent().listen([](__internal__::VTable& vtable) {
        // config
        vtable.Config_getBool = &config::getTemp<bool>;
        vtable.Config_getInt = &config::getTemp<int>;
        vtable.Config_getDouble = &config::getTemp<double>;
        vtable.Config_getString = &config::getTemp<std::string>;
        vtable.Config_getStringView = &config::getTemp<std::string_view>;
        vtable.Config_getBoolInternal = &config::get<bool>;
        vtable.Config_getIntInternal = &config::get<int>;
        vtable.Config_getDoubleInternal = &config::get<double>;
        vtable.Config_getStringInternal = &config::get<std::string>;
        vtable.Config_getStringViewInternal = &config::get<std::string_view>;
        vtable.Config_setBool = &config::setTemp<bool>;
        vtable.Config_setInt = &config::setTemp<int>;
        vtable.Config_setDouble = &config::setTemp<double>;
        vtable.Config_setString = &config::setTemp<std::string>;
        vtable.Config_setStringView = &config::setTemp<std::string_view>;
        vtable.Config_setBoolInternal = &config::set<bool>;
        vtable.Config_setIntInternal = &config::set<int>;
        vtable.Config_setDoubleInternal = &config::set<double>;
        vtable.Config_setStringInternal = &config::set<std::string>;
        vtable.Config_setStringViewInternal = &config::set<std::string_view>;

        // label
        vtable.FormatRiftString = &formatRiftString;
        vtable.GetRiftVariableNull = &getRiftVariable<label::null_t>;
        vtable.GetRiftVariableBool = &getRiftVariable<bool>;
        vtable.GetRiftVariableInt = &getRiftVariable<int64_t>;
        vtable.GetRiftVariableDouble = &getRiftVariable<double>;
        vtable.GetRiftVariableString = &getRiftVariable<std::string>;
        vtable.GetRiftVariableObject = &getRiftVariable<matjson::Value>;
        vtable.SetRiftVariableNull = &setRiftVariable<label::null_t>;
        vtable.SetRiftVariableBool = &setRiftVariable<bool>;
        vtable.SetRiftVariableInt = &setRiftVariable<int64_t>;
        vtable.SetRiftVariableDouble = &setRiftVariable<double>;
        vtable.SetRiftVariableString = &setRiftVariable<std::string>;
        vtable.SetRiftVariableObject = &setRiftVariable<matjson::Value>;


        return ListenerResult::Stop;
    }).leak();
}

}
