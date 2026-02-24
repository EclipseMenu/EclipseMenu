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
#include <ranges>
#include <modules/bot/bot.hpp>

#include "mods.hpp"

namespace eclipse::hacks {
    namespace Bot { bot::Bot& getBot(); }
    namespace Global { std::map<std::string_view, bool> const& getAttemptCheats(); }
}

namespace eclipse::api {
using namespace geode::prelude;

StringMap<Function<bool()>> g_cheats;
StringMap<Function<bool()>>& getCheats() { return g_cheats; }

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
    __internal__::FetchVTableEvent().listen([](__internal__::VTable& vtable, size_t version) {
        // version is unused for now
        (void) version;

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

        // modules
        vtable.RegisterCheat = +[](std::string name, Function<bool()> callback) { g_cheats[std::move(name)] = std::move(callback); };
        vtable.LoadReplay = +[](std::filesystem::path const& path) { return hacks::Bot::getBot().load(path); };
        vtable.LoadReplayFromData = +[](std::span<uint8_t> data) { return hacks::Bot::getBot().load(data); };
        vtable.CheckCheatsEnabled = +[] { return config::getTemp<"hasCheats", bool>(false); };
        vtable.CheckCheatedInAttempt = +[] { return config::getTemp<"trippedSafeMode", bool>(false); };
        vtable.GetHackingModules = +[] {
            return hacks::Global::getAttemptCheats() | std::views::transform([](auto& pair) {
                return HackingModule {
                    .name = pair.first,
                    .state = pair.second ? HackingModule::State::Enabled : HackingModule::State::Tripped
                };
            }) | std::ranges::to<std::vector>();
        };

        // components
        vtable.CreateMenuTab = +[](std::string_view name) { gui::MenuTab::find(name); };
        vtable.CreateLabel = +[](std::string_view tabName, std::string title) -> size_t {
            auto tab = gui::MenuTab::find(tabName);
            if (!tab) return 0;
            auto label = tab->addLabel(std::move(title));
            return label->getUID();
        };
        vtable.CreateToggle = +[](std::string_view tabName, std::string id, std::string title, Function<void(bool)> callback) -> size_t {
            auto tab = gui::MenuTab::find(tabName);
            if (!tab) return 0;
            return tab->addToggle(std::move(title), std::move(id))
                      ->callback(std::move(callback))
                      ->handleKeybinds()
                      ->disableSaving()
                      ->getUID();
        };
        vtable.CreateInputFloat = +[](std::string_view tabName, std::string title, std::string id, Function<void(float)> callback) -> size_t {
            auto tab = gui::MenuTab::find(tabName);
            if (!tab) return 0;
            return tab->addInputFloat(std::move(title), std::move(id))
                      ->callback(std::move(callback))
                      ->disableSaving()
                      ->getUID();
        };
        vtable.CreateButton = +[](std::string_view tabName, std::string title, Function<void()> callback) -> size_t {
            auto tab = gui::MenuTab::find(tabName);
            if (!tab) return 0;
            return tab->addButton(std::move(title))
                      ->callback(std::move(callback))
                      ->getUID();
        };
        vtable.SetComponentDescription = +[](size_t id, std::string description) {
            auto component = gui::Component::find(id);
            if (!component) return;
            component->setDescription(std::move(description));
        };
        vtable.SetLabelText = +[](size_t id, std::string text) {
            auto component = gui::Component::find(id);
            if (!component || component->getType() != gui::ComponentType::Label) return;
            static_cast<gui::LabelComponent*>(component)->setText(std::move(text));
        };
        vtable.SetInputFloatParams = +[](size_t id, std::optional<float> min, std::optional<float> max, std::optional<std::string> format) {
            auto component = gui::Component::find(id);
            if (!component || component->getType() != gui::ComponentType::InputFloat) return;
            auto inputFloat = static_cast<gui::InputFloatComponent*>(component);
            if (min) inputFloat->setMin(min.value());
            if (max) inputFloat->setMax(max.value());
            if (format) inputFloat->setFormat(std::move(format).value());
        };

        return ListenerResult::Stop;
    }).leak();
}

}
