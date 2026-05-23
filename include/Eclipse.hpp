#pragma once

#ifdef ECLIPSE_USING_EVENT_API
    #warning "Already using Event API, either remove \"Events.hpp\" include to use linked API, or don't include \"Eclipse.hpp\""
#else
    #ifndef ECLIPSE_USING_LINK_API
        #define ECLIPSE_USING_LINK_API
    #endif
#endif

#include "Macros.hpp"