#pragma once

#ifdef GEODE_IS_ANDROID
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace eclipse::utils {
    namespace traits
    {
        template <typename T>
        struct FunctionTraits;

        template <typename Ret, typename... Args>
        struct FunctionTraits<Ret(*)(Args...)> {
            using ReturnType = Ret;
            using ClassType = void;
            using ArgumentTypes = std::tuple<Args...>;
            static constexpr size_t Arity = sizeof...(Args);
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionTraits<Ret(Class::*)(Args...)> {
            using FunctionType = Ret(Class::*)(Args...);
            using FunctionPointerType = Ret(*)(Class*, Args...);
            using ReturnType = Ret;
            using ClassType = Class;
            using ArgumentTypes = std::tuple<Args...>;
            static constexpr size_t Arity = sizeof...(Args);

            template <typename H>
            using TrampolineType = Ret(*)(H&, typename H::TargetType*, Args...);

            template <typename H>
            static Ret trampoline(Class* self, Args... args);

            static Ret Call(Ret(Class::*method)(Args...), Class* obj, Args... args) {
                return (obj->*method)(args...);
            }
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionTraits<Ret(Class::*)(Args...) const> {
            using FunctionType = Ret(Class::*)(Args...);
            using FunctionPointerType = Ret(*)(const Class*, Args...);
            using ReturnType = Ret;
            using ClassType = Class;
            using ArgumentTypes = std::tuple<Args...>;
            static constexpr size_t Arity = sizeof...(Args);

            template <typename H>
            using TrampolineType = Ret(*)(H&, typename H::TargetType*, Args...);

            template <typename H>
            static Ret trampoline(Class* self, Args... args);

            static Ret Call(Ret(Class::*method)(Args...) const, const Class* obj, Args... args) {
                return (obj->*method)(args...);
            }
        };
    }

    /// @brief Utility class for replacing virtual table entries with custom functions.
    /// Supports chaining multiple hooks.
    template <auto F, typename T = traits::FunctionTraits<decltype(F)>::ClassType>
    class VMTHooker {
    public:
        using TargetType = T;
        using Traits = traits::FunctionTraits<decltype(F)>;
        static_assert(!std::is_same_v<typename Traits::ClassType, void>, "Function cannot be a free floating function");
        using HookFunc = Traits::template TrampolineType<VMTHooker>;

    private:
        friend struct traits::FunctionTraits<decltype(F)>;

        Traits::FunctionPointerType m_original = nullptr;
        std::vector<HookFunc> m_hooks;
        size_t m_callCount = 0;
        #ifdef GEODE_IS_ANDROID
        uintptr_t* m_vmt_address = 0;
        #else
        geode::Patch* m_patch = nullptr;
        #endif

    private:
        VMTHooker() {
            auto vmtOffset = geode::addresser::getVirtualOffset(F);
            uintptr_t vmtAddress = 0; {
                auto tmpInstance = TargetType();
                vmtAddress = *reinterpret_cast<uintptr_t*>(&tmpInstance);
            }

            auto vmtEntry = reinterpret_cast<uintptr_t*>(vmtAddress + vmtOffset);
            m_original = reinterpret_cast<Traits::FunctionPointerType>(*vmtEntry);
            #ifdef GEODE_IS_ANDROID
            static size_t pageSize = sysconf(_SC_PAGESIZE);
            static void* pageStart = reinterpret_cast<void*>(
                reinterpret_cast<uintptr_t>(vmtEntry) & ~(pageSize - 1)
            );
            m_vmt_address = vmtEntry;

            if (mprotect(pageStart, pageSize, PROT_READ | PROT_WRITE) == -1) {
                geode::log::error(
                    "mprotect failed while trying to make VMT entry at {} writable: {}",
                    static_cast<void*>(m_vmt_address),
                    strerror(errno)
                );
                return;
            }

            const auto bytes = geode::toBytes(&Traits::template trampoline<VMTHooker>);
            std::memcpy(m_vmt_address, bytes.data(), bytes.size());

            mprotect(pageStart, pageSize, PROT_READ);
            #else
            auto res = geode::Mod::get()->patch(
                vmtEntry, geode::toBytes(&Traits::template trampoline<VMTHooker>)
            );

            if (!res) {
                geode::log::error("Failed to hook VMT entry: {}", res.unwrapErr());
            } else {
                m_patch = res.unwrap();
            }

            updatePatch();
            #endif
        }

        template <typename ...Args>
        Traits::ReturnType callOriginal(Traits::ClassType* instance, Args... args) {
            return m_original(instance, args...);
        }

        void updatePatch() {
            #ifdef GEODE_IS_ANDROID
            static size_t pageSize = sysconf(_SC_PAGESIZE);
            static void* pageStart = reinterpret_cast<void*>(
                reinterpret_cast<uintptr_t>(m_vmt_address) & ~(pageSize - 1)
            );
            if (mprotect(pageStart, pageSize, PROT_READ | PROT_WRITE) == -1) {
                geode::log::error(
                    "mprotect failed while trying to make VMT entry at {} writable: {}",
                    static_cast<void*>(m_vmt_address),
                    strerror(errno)
                );
                return;
            }

            if (m_hooks.empty()) {
                *m_vmt_address = reinterpret_cast<uintptr_t>(m_original);
            } else {
                const auto bytes = geode::toBytes(&Traits::template trampoline<VMTHooker>);
                std::memcpy(m_vmt_address, bytes.data(), bytes.size());
            }

            mprotect(pageStart, pageSize, PROT_READ);
            #else
            if (!m_patch) return;
            if (m_hooks.empty()) {
                (void) m_patch->disable();
            } else {
                (void) m_patch->enable();
            }
            #endif
        }

    public:
        static VMTHooker& get() {
            static VMTHooker hooker;
            return hooker;
        }

        /// @brief Adds a hook to the chain.
        void addHook(HookFunc hook) {
            if (std::find(m_hooks.begin(), m_hooks.end(), hook) != m_hooks.end()) return;
            m_hooks.push_back(hook);
            updatePatch();
        }

        /// @brief Removes a hook from the chain.
        void removeHook(HookFunc hook) {
            m_hooks.erase(std::remove(m_hooks.begin(), m_hooks.end(), hook), m_hooks.end());
            updatePatch();
        }

        /// @brief Toggles a hook in the chain.
        void toggleHook(HookFunc hook, bool enable) {
            if (enable) {
                addHook(hook);
            } else {
                removeHook(hook);
            }
        }

        /// @brief Invokes the hook chain. If no hooks are present, the original function is called.
        template <typename ...Args>
        Traits::ReturnType invoke(TargetType* instance, Args... args) {
            if (m_callCount >= m_hooks.size()) {
                return callOriginal(reinterpret_cast<Traits::ClassType*>(instance), args...);
            }
            auto hook = m_hooks[m_callCount];
            m_callCount++;
            return hook(*this, instance, args...);
        }

        template <typename ...Args>
        Traits::ReturnType operator()(TargetType* instance, Args... args) {
            return invoke(instance, args...);
        }
    };

    template <typename R, typename C, typename ...Args>
    template <typename H>
    R traits::FunctionTraits<R(C::*)(Args...)>::trampoline(C* self, Args... args) {
        auto& hook = H::get();
        hook.m_callCount = 0;
        return hook.invoke(reinterpret_cast<typename H::TargetType*>(self), args...);
    }
}
