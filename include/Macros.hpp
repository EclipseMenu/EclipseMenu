#pragma once
#include <Geode/platform/cplatform.h>

#ifndef ECLIPSE_DLL
    #ifdef Eclipse_EXPORTS
        #ifdef GEODE_IS_WINDOWS
            #define ECLIPSE_DLL __declspec(dllexport)
        #else
            #define ECLIPSE_DLL __attribute__((visibility("default")))
        #endif
    #else
        #ifdef GEODE_IS_WINDOWS
            #define ECLIPSE_DLL __declspec(dllimport)
        #else
            #define ECLIPSE_DLL
        #endif
    #endif
#endif

#ifndef ECLIPSE_EVENT_METHOD
    #define ECLIPSE_EVENT_METHOD(name, ...)
#endif