cmake_minimum_required(VERSION 3.21)
add_library(third_party INTERFACE)

# Include libraries
add_library(imgui INTERFACE)
CPMAddPackage("gh:EclipseMenu/imgui#8cf1bfc")
target_include_directories(imgui INTERFACE ${imgui_SOURCE_DIR})
target_sources(imgui INTERFACE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)
set(HAS_IMGUI ON)
set(IMGUI_HOOK_EARLY OFF)

if (NOT ANDROID AND NOT IOS)
    CPMAddPackage("gh:EclipseMenu/discord-presence#3b50b08")
endif()

CPMAddPackage("gh:prevter/gd-imgui-cocos#c35983c")
CPMAddPackage("gh:prevter/msgpack-for-matjson#12edd5a")
CPMAddPackage("gh:maxnut/GDR-converter#71104a9")
CPMAddPackage("gh:EclipseMenu/rift@2.0.2")
CPMAddPackage("gh:prevter/AdvancedLabel#17ed1c1")
CPMAddPackage("gh:prevter/sinaps#7d2f7b1")

if (WIN32)
    CPMAddPackage("gh:zyantific/zydis#ae12a09")
endif ()

# Fix debug build
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
    target_compile_definitions(discord-rpc PRIVATE _ITERATOR_DEBUG_LEVEL=0)
    target_compile_definitions(rift PRIVATE _HAS_ITERATOR_DEBUGGING=0)
endif()

# Link them to the project
target_link_libraries(third_party INTERFACE
    GDRconverter
    libGDR
    imgui
    imgui-cocos
    rift
    advanced_label
    sinaps
)

if (NOT ANDROID AND NOT IOS)
    target_link_libraries(third_party INTERFACE discord-rpc)
endif()

if (WIN32)
    target_link_libraries(third_party INTERFACE Zydis Zycore)
endif ()
