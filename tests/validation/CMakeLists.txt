#   SiddiqSoft CMakeLists 
#   Copyright 2024 Abdulkareem Siddiq. All rights reserved.
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
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.1/CPM.cmake ${CMAKE_CURRENT_SOURCE_DIR}/CPM.cmake)
endif()
include(${CMAKE_CURRENT_SOURCE_DIR}/CPM.cmake)

# ______________________
# Project Metadata block
project(sip2json_client_test
        VERSION         "${CURRENT_PROJECT_VERSION}"
        LANGUAGES       CXX
        HOMEPAGE_URL    "%%myurl%%"
        DESCRIPTION     "Sample client sub-project that uses the sip2json library")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option to select sip2json version for testing/benchmarking: 'current' (default) or tag/version like '1.17.1', '2.4.2', '2.5.7', '2.5.8'
set(SIP2JSON_VERSION "current" CACHE STRING "Version/tag of sip2json to benchmark (e.g. 'current', '1.17.1', '2.4.2', '2.5.7', '2.5.8')")

if(NOT SIP2JSON_VERSION STREQUAL "current" AND NOT SIP2JSON_VERSION STREQUAL "")
    message(STATUS "tests/validation - Fetching sip2json dependency version '${SIP2JSON_VERSION}' via CPM...")
    CPMAddPackage(
        NAME sip2json
        GITHUB_REPOSITORY SiddiqSoft/sip2json
        VERSION ${SIP2JSON_VERSION}
        GIT_TAG ${SIP2JSON_VERSION}
        OPTIONS "sip2json_BUILD_TESTS OFF" "BUILD_TESTS OFF"
    )
else()
    # ____________________________________
    # Ensure target sip2json::sip2json exists (use parent project target if available, or load parent project if building standalone)
    if(NOT TARGET sip2json::sip2json)
        if(TARGET sip2json)
            add_library(sip2json::sip2json ALIAS sip2json)
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeLists.txt")
            add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../.." "${CMAKE_CURRENT_BINARY_DIR}/sip2json")
        endif()
    endif()
endif()

# Executable targets
add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_${CMAKE_CXX_STANDARD})
target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp)

# Benchmark executable target
add_executable(sip2json_benchmark ${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp)
target_compile_features(sip2json_benchmark PRIVATE cxx_std_${CMAKE_CXX_STANDARD})

# ____________________________________
# Dependencies
if(NOT TARGET GTest::gtest_main)
    CPMAddPackage("gh:google/googletest#v1.17.0")
endif()
if(NOT TARGET nlohmann_json::nlohmann_json)
    CPMAddPackage("gh:nlohmann/json#v3.12.0")
endif()
if(NOT TARGET ctre::ctre)
    CPMAddPackage("gh:hanickadot/compile-time-regular-expressions#v3.11.0")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest_main
                                                sip2json::sip2json)
target_link_libraries(sip2json_benchmark PRIVATE sip2json::sip2json)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME} TEST_PREFIX "${PROJECT_NAME}."
                    PROPERTIES ENVIRONMENT "SAMPLES_DIR=${CMAKE_CURRENT_SOURCE_DIR}/samples")
