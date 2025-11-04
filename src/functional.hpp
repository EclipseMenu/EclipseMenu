#pragma once

#if defined(__cpp_lib_move_only_function) || defined(__cpp_lib_function_ref)
#include <functional>
#endif

#if !defined(__cpp_lib_move_only_function)
#include <std23/move_only_function.h>
#endif

#if !defined(__cpp_lib_function_ref)
#include <std23/function_ref.h>
#endif

namespace eclipse {
#ifdef __cpp_lib_move_only_function
    template <typename... Args>
    using Function = std::move_only_function<Args...>;
#else
    template <typename... Args>
    using Function = std23::move_only_function<Args...>;
#endif

#ifdef __cpp_lib_function_ref
    template <typename... Args>
    using FunctionRef = std::function_ref<Args...>;
#else
    template <typename... Args>
    using FunctionRef = std23::function_ref<Args...>;
#endif

    template <typename... Args>
    using StdFunction = std::function<Args...>;
}
