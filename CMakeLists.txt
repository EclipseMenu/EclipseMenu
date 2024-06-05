cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(EclipseMenu VERSION 1.0.0)

# Sources
file(GLOB_RECURSE SOURCES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Make sure we can include headers from src
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Add Geode
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Libraries
include(${CMAKE_SOURCE_DIR}/cmake/IncludeLibs.cmake)
target_link_libraries(${PROJECT_NAME} third_party)

# Setup Geode
setup_geode_mod(${PROJECT_NAME})
