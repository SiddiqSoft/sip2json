#   SiddiqSoft CMakeLists - Benchmark Suite
#   Copyright 2024-2026 Abdulkareem Siddiq. All rights reserved.
#   See LICENSE file
#
cmake_minimum_required(VERSION 3.31)

# _________________
# Version detection and CPM cache setup
if(WIN32)
    if(DEFINED ENV{LOCALAPPDATA})
        set(CPM_SOURCE_CACHE "$ENV{LOCALAPPDATA}/CPM/.cpmcache")
    elseif(DEFINED ENV{USERPROFILE})
        set(CPM_SOURCE_CACHE "$ENV{USERPROFILE}/AppData/Local/CPM/.cpmcache")
    else()
        set(CPM_SOURCE_CACHE "${CMAKE_BINARY_DIR}/.cpmcache")
    endif()
else()
    set(CPM_SOURCE_CACHE "$ENV{HOME}/.cache/.cpmcache")
endif()
set(CPM_SOURCE_CACHE ${CPM_SOURCE_CACHE} CACHE PATH "CPM source cache")

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CPM.cmake)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../validation/CPM.cmake")
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../validation/CPM.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/CPM.cmake" COPYONLY)
    else()
        file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.1/CPM.cmake ${CMAKE_CURRENT_SOURCE_DIR}/CPM.cmake)
    endif()
endif()
include(${CMAKE_CURRENT_SOURCE_DIR}/CPM.cmake)

# ______________________
# Project Metadata block
project(sip2json_benchmark
        VERSION         "${CURRENT_PROJECT_VERSION}"
        LANGUAGES       CXX
        DESCRIPTION     "Performance Benchmark Suite for sip2json")

if(NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD STREQUAL "")
    set(CMAKE_CXX_STANDARD 23)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option to select sip2json version for benchmark: 'current' (default) or tag/version like 'master', 'v2.4.2'
set(SIP2JSON_VERSION "current" CACHE STRING "Version/tag of sip2json to benchmark (e.g. 'current', 'master', 'v2.4.2')")

if(NOT SIP2JSON_VERSION STREQUAL "current" AND NOT SIP2JSON_VERSION STREQUAL "")
    message(STATUS "tests/benchmark - Fetching sip2json dependency version '${SIP2JSON_VERSION}' via CPM...")
    CPMAddPackage(
        NAME sip2json
        GITHUB_REPOSITORY SiddiqSoft/sip2json
        VERSION ${SIP2JSON_VERSION}
        GIT_TAG ${SIP2JSON_VERSION}
        OPTIONS "sip2json_BUILD_TESTS OFF" "BUILD_TESTS OFF"
    )
else()
    # ____________________________________
    # Ensure target sip2json::sip2json exists
    if(NOT TARGET sip2json::sip2json)
        if(TARGET sip2json)
            add_library(sip2json::sip2json ALIAS sip2json)
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeLists.txt")
            add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../.." "${CMAKE_CURRENT_BINARY_DIR}/sip2json")
        endif()
    endif()
endif()

# Dependencies
if(NOT TARGET nlohmann_json::nlohmann_json)
    CPMAddPackage("gh:nlohmann/json#v3.12.0")
endif()
if(NOT TARGET ctre::ctre)
    CPMAddPackage("gh:hanickadot/compile-time-regular-expressions#v3.11.0")
endif()

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_${CMAKE_CXX_STANDARD})
target_link_libraries(${PROJECT_NAME} PRIVATE sip2json::sip2json)
