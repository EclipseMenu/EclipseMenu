cmake_minimum_required(VERSION 3.21)
add_library(third_party INTERFACE)

# Include libraries
add_library(imgui INTERFACE)
CPMAddPackage("gh:EclipseMenu/imgui#6cd1915")
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
    CPMAddPackage("gh:EclipseMenu/discord-presence#01b3ebc")
endif()

CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.12.0
)

CPMAddPackage("gh:matcool/gd-imgui-cocos#9764333")
CPMAddPackage("gh:maxnut/GDR-converter#55c5321")
CPMAddPackage("gh:EclipseMenu/rift@2.0.2")
CPMAddPackage("gh:prevter/AdvancedLabel#a800983")
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
    nlohmann_json::nlohmann_json
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
