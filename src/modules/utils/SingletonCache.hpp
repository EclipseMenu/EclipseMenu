#pragma once

namespace eclipse::utils {
    template <typename T>
    using base_type = std::remove_pointer_t<std::remove_cv_t<T>>;

    template <typename T, typename... Args>
    struct not_one_of {
        constexpr static bool value = (!std::is_same_v<base_type<T>, Args> && ...);
    };

    template <typename T, typename... Args>
    constexpr bool not_one_of_v = not_one_of<T, Args...>::value;

    template <typename T, typename... Args>
    constexpr bool one_of_v = !not_one_of_v<T, Args...>;

    template <typename T>
    concept HasSharedState = requires { T::get(); } || requires { T::sharedDispatcher(); };

    template <typename T>
    concept NestedInstance = one_of_v<base_type<T>, EditorUI, UILayer>;

    template <typename T>
    concept SupportedSingleton = HasSharedState<base_type<T>> || NestedInstance<T>;

    template <SupportedSingleton T>
    base_type<T>* get(); // forward declaration

    template <typename T>
    base_type<T>* getNested() {
        if constexpr (std::is_same_v<base_type<T>, EditorUI>) {
            if (auto editor = utils::get<LevelEditorLayer>()) {
                return editor->m_editorUI;
            }
        } else if constexpr (std::is_same_v<base_type<T>, UILayer>) {
            if (auto gjbgl = utils::get<GJBaseGameLayer>()) {
                return gjbgl->m_uiLayer;
            }
        } else {
            static_assert(!std::is_same_v<base_type<T>, base_type<T>>, "Unsupported nested singleton type");
        }
        return nullptr;
    }

    /// @brief More optimized way to get singletons. Caches the pointer after first call.
    /// Regular sharedState() function calls have an overhead of 10-20% compared to this.
    template <SupportedSingleton T>
    base_type<T>* get() {
        using type = base_type<T>;

        if constexpr (NestedInstance<type>) {
            return getNested<T>();
        } else {
            constexpr bool isStatic = not_one_of_v<
                type,
                PlayLayer, LevelEditorLayer,
                GJBaseGameLayer, MenuLayer,
                cocos2d::CCScene
            >;
            using refType = std::conditional_t<isStatic, type, type*>;

            static refType* ref = nullptr;
            if (ref) {
                if constexpr (isStatic) return ref;
                else return *ref;
            }

            if constexpr (std::is_same_v<type, PlayLayer>) {
                ref = &utils::get<GameManager>()->m_playLayer;
            } else if constexpr (std::is_same_v<type, LevelEditorLayer>) {
                ref = &utils::get<GameManager>()->m_levelEditorLayer;
            } else if constexpr (std::is_same_v<type, GJBaseGameLayer>) {
                ref = &utils::get<GameManager>()->m_gameLayer;
            } else if constexpr (std::is_same_v<type, MenuLayer>) {
                ref = &utils::get<GameManager>()->m_menuLayer;
            } else if constexpr (std::is_same_v<type, cocos2d::CCScene>) {
                ref = &cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
            } else if constexpr (isStatic) {
                if constexpr (std::is_same_v<type, cocos2d::CCIMEDispatcher>) {
                    ref = cocos2d::CCIMEDispatcher::sharedDispatcher();
                } else {
                    ref = type::get();
                }
            } else {
                static_assert(!std::is_same_v<type, type>, "Unsupported singleton type");
            }

            if constexpr (isStatic) return ref;
            else return *ref;
        }
    }
}
