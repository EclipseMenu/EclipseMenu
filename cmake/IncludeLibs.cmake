cmake_minimum_required(VERSION 3.21)
add_library(third_party INTERFACE)

# Include libraries
add_library(imgui INTERFACE)
CPMAddPackage("gh:EclipseMenu/imgui#dac32cb")
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
CPMAddPackage("gh:matcool/gd-imgui-cocos#8cf1d37")
CPMAddPackage("gh:maxnut/GDReplayFormat#4950cc2")
CPMAddPackage("gh:EclipseMenu/rift#b8b31d6")
CPMAddPackage("gh:SpaghettDev/subprocess#e12740b")
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.11.3
)

# Fix debug build
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
    target_compile_definitions(discord-rpc PRIVATE _ITERATOR_DEBUG_LEVEL=0)
    target_compile_definitions(rift PRIVATE _HAS_ITERATOR_DEBUGGING=0)
endif()

# Link them to the project
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