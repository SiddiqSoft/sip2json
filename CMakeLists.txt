#   SiddiqSoft CMakeLists 
#   Copyright 2024 Abdulkareem Siddiq. All rights reserved.
#   See LICENSE file
#
#   When building from our CI server, the following options must be set
#   ${PROJECT_NAME}_BUILD_TESTS ON
#   CI_BUILDID must be set to the gitversion
# 
cmake_minimum_required(VERSION 3.25)

# _________________
# Version detection; must preceed the declaration of the actual project()
# Figures out the value ${CURRENT_PROJECT_VERSION} and therefore the ${PROJECT_VERSION}
# Brings in the CPM module into pack/CPM.make
include(pack/CMakeCommonHelpers.cmake)

# ______________________
# Project Metadata block
project(sip2json
        VERSION         "${CURRENT_PROJECT_VERSION}"
        LANGUAGES       CXX
        HOMEPAGE_URL    "%%myurl%%"
        DESCRIPTION     "SIP Parser for Modern C++")


# ____________________________________
# Options; define and declare defaults
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # We run the tests by default when we're in development mode for this library
    option(${PROJECT_NAME}_BUILD_TESTS "Build tests. Uncheck for install only runs" OFF)
endif()


# ____________________________________
#  Library Definition
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
                            INTERFACE
                            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)


# ____________________________________
# Dependencies
# Here we explicitly declare that we depend on the nlohmann_json package and that
# the client will only need to depend on our file where the nlohmann_json will
# automatically included.
cpmaddpackage("gh:nlohmann/json#9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03")# v3.11.3
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)


# ____________________________________
# Testing
# Available only when building in our repo; controlled by the switch: ${PROJECT_NAME}_BUILD_TESTS = ON
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    if(${PROJECT_NAME}_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests" )
        message(STATUS "Asked to build tests.. ${PROJECT_NAME}_BUILD_TESTS = ${${PROJECT_NAME}_BUILD_TESTS}")
        add_subdirectory(tests)
        include(CTest)
    endif()
endif()