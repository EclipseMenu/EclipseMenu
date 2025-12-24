#include "hack.hpp"

#include <ranges>
#include <modules/config/config.hpp>

namespace eclipse::hack {
    static bool s_lateInit = false;

    bool isLateInit() {
        return s_lateInit;
    }

    std::vector<HackPtr>& getHacks() {
        static std::vector<HackPtr> hacks;
        return hacks;
    }

    std::vector<HackPtr>& getUpdatedHacks() {
        static std::vector<HackPtr> hacks;
        return hacks;
    }

    std::vector<HackPtr>& getCheatingHacks() {
        static std::vector<HackPtr> hacks;
        return hacks;
    }

    void registerHack(HackPtr&& hack) {
        getHacks().push_back(hack);
        if (s_lateInit)
            hack->init();
    }

    WeakHackPtr find(std::string_view id) {
        for (auto const& hack : getHacks()) {
            if (hack->getId() == id) {
                return hack;
            }
        }
        return {};
    }

    void initializeHacks() {
        auto& hacks = getHacks();

        // Sort hacks by priority (and then ID)
        std::ranges::sort(hacks, [](auto& a, auto& b) {
            if (a->getPriority() == b->getPriority()) {
                std::string_view aId = a->getId();
                std::string_view bId = b->getId();
                return aId < bId;
            }

            return a->getPriority() < b->getPriority();
        });

        for (auto const& hack : hacks)
            hack->init();

        s_lateInit = true;
    }

    void lateInitializeHacks() {
        for (auto const hack : getHacks())
            hack->lateInit();
    }

    void safeHooksAll(std::map<std::string, std::shared_ptr<geode::Hook>>& hooks) {
        for (auto& hookPtr : hooks | std::views::values) {
            hookPtr->setPriority(SAFE_HOOK_PRIORITY);
        }
    }

    void safeHooks(
        std::map<std::string, std::shared_ptr<geode::Hook>>& hooks, std::string_view className,
        std::initializer_list<std::string_view> funcs
    ) {
        for (auto hook : funcs) {
            auto name = fmt::format("{}::{}", className, hook);
            auto it = hooks.find(name);
            if (it != hooks.end()) {
                it->second->setPriority(SAFE_HOOK_PRIORITY);
            } else {
                geode::log::warn("Hook '{}' not found in class '{}'", hook, className);
            }
        }
    }

    void firstHooksAll(std::map<std::string, std::shared_ptr<geode::Hook>>& hooks) {
        for (auto& hookPtr : hooks | std::views::values) {
            hookPtr->setPriority(FIRST_HOOK_PRIORITY);
        }
    }

    void firstHooks(
        std::map<std::string, std::shared_ptr<geode::Hook>>& hooks, std::string_view className,
        std::initializer_list<std::string_view> funcs
    ) {
        for (auto hook : funcs) {
            auto name = fmt::format("{}::{}", className, hook);
            auto it = hooks.find(name);
            if (it != hooks.end()) {
                it->second->setPriority(FIRST_HOOK_PRIORITY);
            } else {
                geode::log::warn("Hook '{}' not found in class '{}'", hook, className);
            }
        }
    }

    void setupTogglesAll(std::string_view id, std::map<std::string, std::shared_ptr<geode::Hook>>& hooks) {
        auto value = config::get(id, false);
        std::vector<geode::Hook*> hookPtrs;
        for (auto& hook : hooks | std::views::values) {
            hook->setAutoEnable(value);
            hookPtrs.push_back(hook.get());
        }
        config::addDelegate(id, [hookPtrs = std::move(hookPtrs), id] {
            auto value = config::get(id, false);
            for (auto hook : hookPtrs) {
                (void) hook->toggle(value);
            }
        });
    }

    void setupToggles(
        std::string_view id, std::map<std::string, std::shared_ptr<geode::Hook>>& hooks, std::string_view className,
        std::initializer_list<std::string_view> funcs
    ) {
        auto value = config::get(id, false);
        std::vector<geode::Hook*> hookPtrs;
        for (auto hook : funcs) {
            auto name = fmt::format("{}::{}", className, hook);
            auto it = hooks.find(name);
            if (it != hooks.end()) {
                it->second->setAutoEnable(value);
                hookPtrs.push_back(it->second.get());
            } else {
                geode::log::warn("Hook '{}' not found in class '{}'", hook, className);
            }
        }
        config::addDelegate(id, [hookPtrs = std::move(hookPtrs), id] {
            auto value = config::get(id, false);
            for (auto hook : hookPtrs) {
                (void) hook->toggle(value);
            }
        });
    }
}
