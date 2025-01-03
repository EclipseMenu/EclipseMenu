cmake_minimum_required(VERSION 3.21)

add_library(third_party INTERFACE)

add_library(imgui INTERFACE)
CPMAddPackage("gh:ocornut/imgui#1d4c752")
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

if (NOT ANDROID)
    set(RAPIDJSONTEST OFF)
    CPMAddPackage("gh:EclipseMenu/discord-rpc#1259d3a")
endif()

CPMAddPackage("gh:matcool/gd-imgui-cocos#0207a36")
CPMAddPackage("gh:maxnut/GDReplayFormat#5b8aeab")
set(RIFT_INCLUDE_MATJSON ON)
CPMAddPackage("gh:EclipseMenu/rift#60c9a83")
CPMAddPackage("gh:SpaghettDev/subprocess#e12740b")
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.11.3
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
    target_compile_definitions(discord-rpc PRIVATE _ITERATOR_DEBUG_LEVEL=0)
    target_compile_definitions(rift PRIVATE _HAS_ITERATOR_DEBUGGING=0)
endif()

target_link_libraries(third_party INTERFACE
    libGDR
    subprocess
    imgui
    imgui-cocos
    nlohmann_json::nlohmann_json
    rift
)

if (NOT ANDROID)
    target_link_libraries(third_party INTERFACE discord-rpc)
endif()
