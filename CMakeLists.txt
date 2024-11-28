#   SiddiqSoft CMakeLists 
#   Copyright 2024 Abdulkareem Siddiq. All rights reserved.
#   See LICENSE file
#
#   When building from our CI server, the following options must be set
#   ${PROJECT_NAME}_BUILD_TESTS ON
#   ${PROJECT_NAME}_BUILD_PACKAGE ON
#   CI_BUILDID must be set to the gitversion
# 
cmake_minimum_required(VERSION 3.25)

# _________________
# Version detection; must preceed the declaration of the actual project()
# Figures out the value ${CURRENT_PROJECT_VERSION} and therefore the ${PROJECT_VERSION}
include(pack/DetectAndFixupProjectVersion.cmake)

# ______________________
# Project Metadata block
project(sip2json
        VERSION         "${CURRENT_PROJECT_VERSION}"
        LANGUAGES       CXX
        HOMEPAGE_URL    "https://github.com/siddiqsoftware/sip2json"
        DESCRIPTION     "SIP Parser for Modern C++")

# ____________________________________
# Options; define and declare defaults
include(pack/ProjectOptions.cmake)

# ____________________________________
# Dependencies
include(pack/DependenciesList.cmake)

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
# Testing; controlled by the switch: ${PROJECT_NAME}_BUILD_TESTS = ON
include(pack/TestingSupport.cmake)
