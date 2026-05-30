#pragma once
#include <ranges>
#include <string_view>
#include "Config.hpp"
#include "../Prelude.hpp"

namespace eclipse {
    enum class MenuTab : uint8_t {
        Global,
        Player,
        Level,
        Creator,
        Bypass,
        Shortcuts
    };

    enum class ModuleType : uint8_t {
        Toggle,
        Button,
        IntToggle,
        FloatToggle,
        InputFloat,
        Color,
    };

    struct OptionDef {
        std::string id;
        ModuleType type = ModuleType::Toggle;
    };

    struct ModuleDef {
        MenuTab tab;
        ModuleType type = ModuleType::Toggle;
        bool cheat = false;
        std::vector<OptionDef> options;
    };

    class ModuleRegistry {
    public:
        static ModuleRegistry* get() {
            static ModuleRegistry instance;
            return &instance;
        }

        void emplace(std::string_view id, ModuleDef&& def) {
            // TODO: actually save it lol
        }
    };

    struct HackRegistar {
        std::string_view id;
        bool operator<<(ModuleDef&& def) const {
            ModuleRegistry::get()->emplace(id, std::move(def));
            return true;
        }
    };

    #define $hack(Name, Id) \
        namespace { struct Name { static constexpr std::string_view id = Id; }; } \
        static bool const Name##_registered = HackRegistar{Id} << ModuleDef

    inline void RegisterHooksHighPrio(
        StringMap<std::shared_ptr<geode::Hook>>& hooks,
        Setting<bool>& setting
    ) {
        auto value = setting.value();

        asp::SmallVec<geode::Hook*, 4> hookPtrs;
        for (auto& hook : hooks | std::views::values) {
            hook->setAutoEnable(value);
            hookPtrs.push_back(hook.get());
        }

        setting.listen([hooks = std::move(hookPtrs)](bool enabled) {
            for (auto hook : hooks) {
                (void) hook->toggle(enabled);
            }
        });
    }

    #define BIND_MODULE_HIGHPRIO(Module) \
        static void onModify(auto& self) { \
            ::eclipse::RegisterHooksHighPrio( \
                self.m_hooks,\
                ::eclipse::config::ref<bool, ::eclipse::FixedString<Module::id.size()>{Module::id.data()}>() \
            ); \
        }

}
