#   SiddiqSoft CMakeLists 
#   Copyright 2024 Abdulkareem Siddiq. All rights reserved.
#   See LICENSE file
#
#   When building from our CI server, the following options must be set
#   BUILD_TESTS ON
#   CI_BUILDID must be set to the gitversion
# 
cmake_minimum_required(VERSION 3.31)

# _________________
# Version detection; must preceed the declaration of the actual project()
# Figures out the value ${CURRENT_PROJECT_VERSION} and therefore the ${PROJECT_VERSION}
# Brings in the CPM module into pack/CPM.make
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
include(pack/CMakeCommonHelpers.cmake)

# ______________________
# Project Metadata block
project(sip2json
        VERSION         "${CURRENT_PROJECT_VERSION}"
        LANGUAGES       CXX
        HOMEPAGE_URL    "%%myurl%%"
        DESCRIPTION     "A SIP Parser for Modern C++. Header-only library to parse and serialize SIP messages to/from JSON.")

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
endif()

# ____________________________________
# Options; define and declare defaults
option(${PROJECT_NAME}_BUILD_TESTS "${PROJECT_NAME} - Build tests. Uncheck for install only runs" OFF)
option(${PROJECT_NAME}_BUILD_BENCHMARKS "${PROJECT_NAME} - Build benchmarks" OFF)

# ____________________________________
#  Library Definition
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(GNUInstallDirs)

target_include_directories(${PROJECT_NAME}
                            INTERFACE
                            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Natvis debugger visualization support
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/siddiqsoft.${PROJECT_NAME}.natvis")
    target_sources(${PROJECT_NAME} PRIVATE siddiqsoft.${PROJECT_NAME}.natvis)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/siddiqsoft.${PROJECT_NAME}.natvis
            DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
            COMPONENT natvis
            OPTIONAL)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SiddiqSoft.${PROJECT_NAME}.natvis")
    target_sources(${PROJECT_NAME} PRIVATE SiddiqSoft.${PROJECT_NAME}.natvis)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/SiddiqSoft.${PROJECT_NAME}.natvis
            DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
            COMPONENT natvis
            OPTIONAL)
endif()
        
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

# ____________________________________
# Dependencies
cpmaddpackage("gh:nlohmann/json#v3.12.0")
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)

# ____________________________________
# Testing
# Available only when building in our repo; controlled by the switch: ${PROJECT_NAME}_BUILD_TESTS = ON
if(${${PROJECT_NAME}_BUILD_TESTS} AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests" )
    message(STATUS "${PROJECT_NAME} - Asked to build tests.. ${PROJECT_NAME}_BUILD_TESTS = ${${PROJECT_NAME}_BUILD_TESTS}")
    enable_testing()
    add_subdirectory(tests)
endif()

# ____________________________________
# Benchmarks
if(${${PROJECT_NAME}_BUILD_BENCHMARKS} AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks" )
    message(STATUS "${PROJECT_NAME} - Asked to build benchmarks.. ${PROJECT_NAME}_BUILD_BENCHMARKS = ${${PROJECT_NAME}_BUILD_BENCHMARKS}")
    add_subdirectory(benchmarks)
endif()
