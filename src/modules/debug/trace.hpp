#pragma once
#include <string_view>
#include "utils.hpp"

namespace eclipse::debug {
    class Trace {
        std::string_view m_name;

    public:
        explicit Trace(std::string_view name);
        ~Trace();
    };
}

#ifdef ECLIPSE_DEBUG_BUILD
#define TRACE_FUNC_LINE2(line) GET_FUNC_NAME(newName_##line); eclipse::debug::Trace trace_##line(newName_##line)
#define TRACE_FUNC_LINE1(line) TRACE_FUNC_LINE2(line)
#define TRACE_FUNCTION() TRACE_FUNC_LINE1(__LINE__)

#define TRACE_SCOPE_LINE2(name, line) eclipse::debug::Trace trace_##line(name)
#define TRACE_SCOPE_LINE1(name, line) TRACE_SCOPE_LINE2(name, line)
#define TRACE_SCOPE_LINE0(name, line) TRACE_SCOPE_LINE1(name, line)
#define TRACE_SCOPE(name) TRACE_SCOPE_LINE1(name, __LINE__)
#else
#define TRACE_FUNCTION()
#define TRACE_SCOPE(name)
#endif
