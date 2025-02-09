cmake_minimum_required(VERSION 3.21)
add_library(third_party INTERFACE)

# Include libraries
add_library(imgui INTERFACE)
CPMAddPackage("gh:EclipseMenu/imgui#1d4c752")
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
CPMAddPackage("gh:matcool/gd-imgui-cocos#d6fa518")
CPMAddPackage("gh:maxnut/GDR-converter#c37ed1c")
set(RIFT_INCLUDE_MATJSON ON)
CPMAddPackage("gh:EclipseMenu/rift#d6dd2bd")
CPMAddPackage("gh:SpaghettDev/subprocess#e12740b")
CPMAddPackage("gh:simdutf/simdutf@6.1.0")
CPMAddPackage("gh:prevter/sinaps#1663e86")
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.11.3
)

# Fix debug build
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
    target_compile_definitions(discord-rpc PRIVATE _ITERATOR_DEBUG_LEVEL=0)
    target_compile_definitions(simdutf PRIVATE _ITERATOR_DEBUG_LEVEL=0)
    target_compile_definitions(rift PRIVATE _HAS_ITERATOR_DEBUGGING=0)
endif()

# Link them to the project
target_link_libraries(third_party INTERFACE
    GDRconverter
    libGDR
    subprocess
    imgui
    imgui-cocos
    nlohmann_json::nlohmann_json
    rift
    simdutf
    sinaps
)

if (NOT ANDROID)
    target_link_libraries(third_party INTERFACE discord-rpc)
endif()