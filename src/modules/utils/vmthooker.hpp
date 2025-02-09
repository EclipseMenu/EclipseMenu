#pragma once

namespace eclipse::utils {
    // patching existing virtual table entries is only supported on desktop due to memory protection
    #ifdef GEODE_IS_DESKTOP
    /// @brief Trampoline function which executes the hook chain.
    template <typename T, typename R, typename C, typename... Args>
    R vmtTrampoline(C* self, Args... args);

    /// @brief Utility class for replacing virtual table entries with custom functions.
    /// Supports chaining multiple hooks.
    template <typename T, typename R, typename C, typename... Args>
    class VMTHooker {
    public:
        using TargetType = T;
        using ReturnType = R;
        using ClassType = C;
        using MethodType = R(C::*)(Args...);

    private:
        ReturnType (ClassType::*m_func)(Args...);
        std::vector<ReturnType(*)(VMTHooker&, TargetType*, Args...)> m_hooks;
        geode::Patch* m_patch = nullptr;
        size_t m_callCount = 0;

        R callOriginal(ClassType* instance, Args... args) {
            return (instance->*m_func)(args...);
        }

        friend R vmtTrampoline<T, R, C, Args...>(C*, Args...);

        explicit VMTHooker(ReturnType (ClassType::*method)(Args...)) {
            assert(method);

            auto vmtOffset = geode::addresser::getVirtualOffset(method);
            uintptr_t vmtAddress = 0; {
                auto tmpInstance = TargetType();
                vmtAddress = *reinterpret_cast<uintptr_t*>(&tmpInstance);
            }

            auto vmtEntry = reinterpret_cast<uintptr_t*>(vmtAddress + vmtOffset);
            m_func = *reinterpret_cast<R(C::**)(Args...)>(vmtEntry);
            auto res = geode::Mod::get()->patch(
                vmtEntry, geode::toBytes(&vmtTrampoline<TargetType, ReturnType, ClassType, Args...>)
            );
            if (!res) {
                geode::log::error("Failed to hook vmt entry: {}", res.unwrapErr());
            } else {
                m_patch = res.unwrap();
            }

            updatePatch();
        }

        void updatePatch() {
            if (!m_patch) return;
            if (m_hooks.empty()) {
                (void) m_patch->disable();
            } else {
                (void) m_patch->enable();
            }
        }

    public:
        static VMTHooker& get(ReturnType (ClassType::*method)(Args...) = nullptr) {
            static VMTHooker hooker(method);
            return hooker;
        }

        /// @brief Adds a hook to the chain.
        void addHook(ReturnType (*hook)(VMTHooker&, TargetType*, Args...)) {
            if (std::find(m_hooks.begin(), m_hooks.end(), hook) != m_hooks.end()) return;
            m_hooks.push_back(hook);
            updatePatch();
        }

        /// @brief Removes a hook from the chain.
        void removeHook(ReturnType (*hook)(VMTHooker&, TargetType*, Args...)) {
            m_hooks.erase(std::remove(m_hooks.begin(), m_hooks.end(), hook), m_hooks.end());
            updatePatch();
        }

        /// @brief Toggles a hook in the chain.
        void toggleHook(ReturnType (*hook)(VMTHooker&, TargetType*, Args...), bool enable) {
            if (enable) {
                addHook(hook);
            } else {
                removeHook(hook);
            }
        }

        /// @brief Invokes the hook chain. If no hooks are present, the original function is called.
        ReturnType invoke(TargetType* instance, Args... args) {
            if (m_callCount >= m_hooks.size()) {
                return callOriginal(reinterpret_cast<ClassType*>(instance), args...);
            }
            auto hook = m_hooks[m_callCount];
            m_callCount++;
            return hook(*this, instance, args...);
        }

        ReturnType operator()(TargetType* instance, Args... args) {
            return invoke(instance, args...);
        }
    };

    template <typename T, typename R, typename C, typename... Args>
    R vmtTrampoline(C* self, Args... args) {
        auto& hooker = VMTHooker<T, R, C, Args...>::get();
        hooker.m_callCount = 0; // initiate chain
        return hooker.invoke(reinterpret_cast<T*>(self), args...);
    }
    #endif
}
