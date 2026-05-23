cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

project(Eclipse LANGUAGES CXX)

include(CheckIPOSupported)
check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT LTO_ERROR)

if(LTO_SUPPORTED)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "LTO not supported: ${LTO_ERROR}")
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src include)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

CPMAddPackage("gh:prevter/HeliosUI#bf49a83")
target_compile_definitions(HeliosUI PUBLIC HELIOS_OPENGL_LOADER_INCLUDE="Geode/cocos/platform/CCGL.h")
target_link_libraries(HeliosUI PRIVATE geode-sdk)
target_link_libraries(${PROJECT_NAME} Helios::UI)

setup_geode_mod(${PROJECT_NAME})

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/mod.json" MOD_JSON)
string(JSON MOD_VERSION GET "${MOD_JSON}" "version")
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${MOD_VERSION} SOVERSION 2)
target_compile_definitions(${PROJECT_NAME} PRIVATE ECLIPSE_VERSION="${MOD_VERSION}")