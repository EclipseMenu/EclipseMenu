#pragma once
#include <string_view>

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define TRACE_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define TRACE_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define TRACE_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define TRACE_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define TRACE_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define TRACE_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define TRACE_FUNC_SIG __func__
#else
#define TRACE_FUNC_SIG "TRACE_FUNC_SIG unknown!"
#endif

namespace eclipse::debug {

    template <size_t N>
    struct StaticString {
        char data[N];
        constexpr operator std::string_view() const { return std::string_view(data, N); }
    };

    template <size_t N, size_t K>
    constexpr auto removeFromString(const char(&expr)[N], const char(&remove)[K]) {
        StaticString<N> result = {};
        size_t srcIndex = 0;
        size_t dstIndex = 0;
        while (srcIndex < N) {
            size_t matchIndex = 0;
            while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
                matchIndex++;
            if (matchIndex == K - 1)
                srcIndex += matchIndex;
            result.data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
            srcIndex++;
        }
        return result;
    }

    class Trace {
        std::string_view m_name;

    public:
        explicit Trace(std::string_view name);
        ~Trace();
    };


#define TRACE_FUNC_LINE2(name, line) \
    constexpr auto newName_##line = eclipse::debug::removeFromString(name, "__cdecl ");\
    eclipse::debug::Trace trace_##line(newName_##line)
#define TRACE_FUNC_LINE1(name, line) TRACE_FUNC_LINE2(name, line)
#define TRACE_FUNC_LINE0(name) TRACE_FUNC_LINE1(name, __LINE__)
#define TRACE_FUNCTION() TRACE_FUNC_LINE0(TRACE_FUNC_SIG)

#define TRACE_SCOPE_LINE2(name, line) \
    eclipse::debug::Trace trace_##line(name)
#define TRACE_SCOPE_LINE1(name, line) TRACE_SCOPE_LINE2(name, line)
#define TRACE_SCOPE_LINE0(name, line) TRACE_SCOPE_LINE1(name, line)
#define TRACE_SCOPE(name) TRACE_SCOPE_LINE1(name, __LINE__)

}
