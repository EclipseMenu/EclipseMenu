#pragma once

#include <memory>
#include <utils.hpp>

constexpr int32_t SAFE_HOOK_PRIORITY = 0x500000;
constexpr int32_t FIRST_HOOK_PRIORITY = -0x500000;

#define SAFE_SET_PRIO(name, prio) do { \
    if (!self.setHookPriority(name, prio)) { \
        geode::log::warn("Failed to set {} hook priority!", name); \
    } \
} while (0)

#define SAFE_PRIORITY(name) SAFE_SET_PRIO(name, SAFE_HOOK_PRIORITY)
#define FIRST_PRIORITY(name) SAFE_SET_PRIO(name, FIRST_HOOK_PRIORITY)

// Sets specified hooks priority to SAFE_HOOK_PRIORITY
#define SAFE_HOOKS(class, ...)\
do {\
    std::array funcs = { __VA_ARGS__ };\
    for (auto& func : funcs) {\
        auto name = fmt::format(#class "::{}", func);\
        SAFE_PRIORITY(name);\
    }\
} while (0)

// Sets all hooks priority to SAFE_HOOK_PRIORITY
#define SAFE_HOOKS_ALL()\
do {\
    for (auto& [name, hook] : self.m_hooks) {\
        hook->setPriority(SAFE_HOOK_PRIORITY);\
    }\
} while (0)

// Sets specified hooks priority to FIRST_HOOK_PRIORITY
#define FIRST_HOOKS(class, ...)\
do {\
    std::array funcs = { __VA_ARGS__ };\
    for (auto& func : funcs) {\
        auto name = fmt::format(#class "::{}", func);\
        FIRST_PRIORITY(name);\
    }\
} while (0)

// Sets all hooks priority to FIRST_HOOK_PRIORITY
#define FIRST_HOOKS_ALL()\
do {\
    for (auto& [name, hook] : self.m_hooks) {\
        hook->setPriority(FIRST_HOOK_PRIORITY);\
    }\
} while (0)

// Adds a delegate toggle for specified methods in the modify class
#define HOOKS_TOGGLE(id, class, ...)\
do {\
    std::array funcs = { __VA_ARGS__ };\
    std::vector<geode::Hook*> hooks;\
    for (auto& f : funcs) {\
        auto res = self.getHook(fmt::format(#class "::{}", f));\
        if (res.isErr()) {\
            geode::log::warn("Hook '{}' not found in " #class, f);\
            continue;\
        }\
        auto hook = res.unwrap();\
        hooks.push_back(hook);\
    }\
    auto value = config::get(id, false);\
    for (auto h : hooks) {\
        h->setAutoEnable(value);\
    }\
    config::addDelegate(id, [hooks] {\
        auto value = config::get(id, false);\
        for (auto h : hooks)\
            (void)(value ? h->enable() : h->disable());\
    });\
} while (0)

// Adds a delegate toggle for all hooks in the modify class
#define HOOKS_TOGGLE_ALL(id)\
do {\
    std::vector<geode::Hook*> hooks;\
    for (auto& [name, hook] : self.m_hooks) {\
        hooks.push_back(hook.get());\
    }\
    auto value = config::get(id, false);\
    for (auto h : hooks) {\
        h->setAutoEnable(value);\
    }\
    config::addDelegate(id, [hooks] {\
        auto value = config::get(id, false);\
        for (auto h : hooks) {\
            (void)(value ? h->enable() : h->disable());\
        }\
    });\
} while (0)

// Creates an onModify method with hooks set to SAFE_PRIORITY
#define ENABLE_SAFE_HOOKS(class, ...)\
static void onModify(auto& self) {\
    SAFE_HOOKS(class, __VA_ARGS__);\
}

// Creates an onModify method with hooks set to FIRST_PRIORITY
#define ENABLE_FIRST_HOOKS(class, ...)\
static void onModify(auto& self) {\
    FIRST_HOOKS(class, __VA_ARGS__);\
}

// Creates an onModify method which sets all hooks to SAFE_PRIORITY
#define ENABLE_SAFE_HOOKS_ALL()\
static void onModify(auto& self) {\
    SAFE_HOOKS_ALL();\
}

// Creates an onModify method which sets all hooks to FIRST_PRIORITY
#define ENABLE_FIRST_HOOKS_ALL()\
static void onModify(auto& self) {\
    FIRST_HOOKS_ALL();\
}

// Creates an onModify method with all hooks added to delegate listener
#define ADD_HOOKS_DELEGATE(id)\
static void onModify(auto& self) {\
    HOOKS_TOGGLE_ALL(id);\
}

// Creates an onModify method with all hooks added to delegate listener and specific hooks added to SAFE_PRIORITY
#define DELEGATES_AND_SAFE_PRIO(id, class, ...)\
static void onModify(auto& self) {\
    SAFE_HOOKS(class, __VA_ARGS__);\
    HOOKS_TOGGLE_ALL(id);\
}

// Creates an onModify method with all hooks added to delegate listener and added to SAFE_PRIORITY
#define ALL_DELEGATES_AND_SAFE_PRIO(id)\
static void onModify(auto& self) {\
    SAFE_HOOKS_ALL();\
    HOOKS_TOGGLE_ALL(id);\
}

// Makes a fast static variable that is updated when the config changes
#define CACHE_CONFIG(type, name, cfg, default) \
    static type name = (config::addDelegate(cfg, [] { \
        name = config::get<type>(cfg, default); \
    }), config::get<type>(cfg, default))

// Makes a fast static bool variable that is updated when the config changes
#define CACHE_CONFIG_BOOL(name, cfg) CACHE_CONFIG(bool, name, cfg, false)

// Uses the cached bool and returns it in one macro
#define RETURN_CACHED_BOOL(cfg) CACHE_CONFIG_BOOL(cached, cfg); return cached

#ifdef GEODE_IS_WINDOWS
    #define ECLIPSE_DLL __declspec(dllexport)
#else
    #define ECLIPSE_DLL __attribute__((visibility("default")))
#endif

namespace eclipse::hack {
    /// @brief Base class for all hacks.
    /// @tparam V Hack class type. Used for some preprocessor magic.
    template <typename V>
    class Hack {
    public:
        virtual ~Hack() = default;

        /// @brief Initializes the hack. Used to add GUI elements.
        virtual void init() = 0;

        /// @brief Late initialization. Used to run logic after the game has loaded.
        virtual void lateInit() {}

        /// @brief Callback for CCSchedule::update, called every frame.
        virtual void update() {}

        /// @brief Check whether the hack should trip the safe mode.
        [[nodiscard]] virtual bool isCheating() const { return false; }

        /// @brief Get the hack's ID. (unique identifier)
        [[nodiscard]] virtual const char* getId() const = 0;

        /// @brief Get hack's position priority (used for sorting)
        [[nodiscard]] virtual int32_t getPriority() const { return 0; }

        static constexpr bool HAS_IS_CHEATING = !std::is_same_v<decltype(&V::isCheating), decltype(&Hack::isCheating)>;
        static constexpr bool HAS_UPDATE = !std::is_same_v<decltype(&V::update), decltype(&Hack::update)>;
    };

    using BaseHack = Hack<void>;
    using HackPtr = std::shared_ptr<BaseHack>;
    using WeakHackPtr = std::weak_ptr<BaseHack>;

    /// @brief Finds a hack by its ID.
    [[nodiscard]] WeakHackPtr find(std::string_view id);

    /// @brief Get all registered hacks. (non-const)
    [[nodiscard]] std::vector<HackPtr>& getHacks();

    /// @brief Get hacks that have an update method.
    [[nodiscard]] std::vector<HackPtr>& getUpdatedHacks();

    /// @brief Get hacks that have an isCheating method.
    [[nodiscard]] std::vector<HackPtr>& getCheatingHacks();

    /// @brief Check if all hacks have already been initialized.
    [[nodiscard]] bool isLateInit();

    /// @brief Initializes all hacks.
    void initializeHacks();

    /// @brief Late initializes all hacks.
    void lateInitializeHacks();

    template <typename T>
    void registerHack() {
        auto& hacks = getHacks();
        auto hack = std::make_shared<T>();
        auto ptr = std::reinterpret_pointer_cast<BaseHack>(hack);
        hacks.push_back(ptr);
        if (isLateInit()) {
            hack->init();
        }

        if constexpr (T::HAS_UPDATE) {
            getUpdatedHacks().push_back(ptr);
        }
        if constexpr (T::HAS_IS_CHEATING) {
            getCheatingHacks().push_back(ptr);
        }
    }

    #define REGISTER_HACK(hackClass) $execute { eclipse::hack::registerHack<hackClass>(); }

    #define $hack(name) dummy_##name##_hack; struct ECLIPSE_DLL name : eclipse::hack::Hack<name>
}
