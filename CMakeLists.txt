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

include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(GNUInstallDirs)
include(FetchContent)

# ____________________________________
# Dependencies
include(pack/DependenciesList.cmake)

# ____________________________________
#  Library Definition
add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_sources(${PROJECT_NAME} INTERFACE 
               FILE_SET HEADERS
               BASE_DIRS include
               FILES include/*)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/pack/${PROJECT_NAME}.pc.in
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
                @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
                                COMPATIBILITY AnyNewerVersion)

install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
                ${CMAKE_CURRENT_SOURCE_DIR}/pack/${PROJECT_NAME}-config.cmake
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        FILE_SET HEADERS)

install(EXPORT ${PROJECT_NAME}-targets
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

# ____________________________________
# Testing; controlled by the switch: ${PROJECT_NAME}_BUILD_TESTS = ON
include(pack/TestingSupport.cmake)

# ____________________________________
# Packing; controlled by the switch: ${PROJECT_NAME}_BUILD_PACKAGE=ON
include(pack/PackItSupport.cmake)