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

# -[]-------------------------------------
if(DEFINED CI_BUILDID)
    message(STATUS "Using the CI supplied buildid: CURRENT_PROJECT_VERSION --> ${CI_BUILDID}")

    # Check if we have a last fragment.
    # When building a non release we end up with fragmented/temporary gitversion
    # 1.2.3-branch-name-5.9+99
    # where the `99` at the end indicates the commit count on this branch.
    set(VERSION_LAST "")
    string(REGEX MATCH "[\+]\+([0-9]+)$" VERSION_END_MATCH ${CI_BUILDID})
    message(STATUS "Match count: ${CMAKE_MATCH_COUNT}")
    if(CMAKE_MATCH_COUNT EQUAL 1)
        set(VERSION_LAST         "${CMAKE_MATCH_1}")
    endif()

    # Match the first full version
    string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.*([0-9]+)*" VERSION_MATCH ${CI_BUILDID})
    message(STATUS "Match count: ${CMAKE_MATCH_COUNT}")
    if(CMAKE_MATCH_COUNT EQUAL 3)
        set(VERSION_MAJOR         "${CMAKE_MATCH_1}")
        set(VERSION_MINOR         "${CMAKE_MATCH_2}")
        set(VERSION_PATCH         "${CMAKE_MATCH_3}")
        set(CURRENT_PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
    elseif(CMAKE_MATCH_COUNT EQUAL 4)
        set(VERSION_MAJOR         "${CMAKE_MATCH_1}")
        set(VERSION_MINOR         "${CMAKE_MATCH_2}")
        set(VERSION_PATCH         "${CMAKE_MATCH_3}")
        set(CURRENT_PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${CMAKE_MATCH_4})
    endif()
    if(NOT (VERSION_LAST STREQUAL ""))
        string(APPEND CURRENT_PROJECT_VERSION ".${VERSION_LAST}")
    endif()
else()
    if((DEFINED ${PROJECT_NAME}_BUILD_PACKAGE) AND (NOT DEFINED CI_BUILDID))
        message(ERROR "When building from CI the CI_BUILDID must be set")
    endif()
    message(STATUS "Using the dev-only version")
    set(CURRENT_PROJECT_VERSION "0.0.0.0")
endif()

message(STATUS "Parsed out the CI_BUILDID --> CURRENT_PROJECT_VERSION:${CURRENT_PROJECT_VERSION}")

#   ________________________________________________
# - Set the Name of the project and the details
# - Change the name to your name: sip2json
# - The namespace is set to "siddiqsoft"
# - The option project_name_BUILD_TESTS is set to OFF and used by our CI to build, test and package

# -[]-------------------------------------
project(sip2json
        VERSION         "${CURRENT_PROJECT_VERSION}"
        LANGUAGES       CXX
        HOMEPAGE_URL    "https://github.com/siddiqsoftware/sip2json"
        DESCRIPTION     "SIP Parser for Modern C++")


set(${PROJECT_NAME}_DEVMODE OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(${PROJECT_NAME}_DEVMODE ON)
    message(STATUS "Project is in the `library developer's home` directory. ${${PROJECT_NAME}_DEVMODE}-->ON")
endif()

#############
#  Options  #
#############

# We run the tests by default when we're in development mode for this library
option(${PROJECT_NAME}_BUILD_TESTS "Build tests. Uncheck for install only runs" OFF)
option(${PROJECT_NAME}_BUILD_PACKAGE "Package. Uncheck for install only runs" OFF)

message(STATUS  "Configuration options: ${PROJECT_NAME}_BUILD_TESTS  -- > ${${PROJECT_NAME}_BUILD_TESTS}\n"
                "                          CI_BUILDID            -- > ${CI_BUILDID} --> ${CURRENT_PROJECT_VERSION}\n"
                "                          ${PROJECT_NAME}_BUILD_PACKAGE --> ${${PROJECT_NAME}_BUILD_PACKAGE}")

include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(GNUInstallDirs)
include(FetchContent)

#########################
#  Library Definition   #
#########################

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# We need to add dependencies for this project
message(STATUS "-- Importing the nlohmann package directly..")
include(FetchContent)

FetchContent_Declare(nlohmann_json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(nlohmann_json)

target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)

# Continue with setting up our project
message(STATUS "Installed nlohmann_json -> ${NLOHMANNJSON_INCLUDE_DIRS}")
install(DIRECTORY ${NLOHMANNJSON_INCLUDE_DIRS} DESTINATION include)
target_include_directories( ${PROJECT_NAME} INTERFACE ${NLOHMANNJSON_INCLUDE_DIRS})
target_include_directories( ${PROJECT_NAME} INTERFACE
                            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:include>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

# Depencency Interface..
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json)
target_include_directories( nlohmann_json INTERFACE
                            $<BUILD_INTERFACE:${NLOHMANNJSON_INCLUDE_DIRS}>
                            $<INSTALL_INTERFACE:include>)

# For modules that are libraries (static or dynamic) we will need to include the src folder if present
if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src"
    AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src"
    AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt")
    # We're building a library.. add the src subdirectory.
    message(STATUS "We're building a library.. add the src subdirectory.")
    add_subdirectory(src)
endif()

if(${PROJECT_NAME}_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests" )
    message(STATUS "Asked to build tests.. ${PROJECT_NAME}_BUILD_TESTS = ${${PROJECT_NAME}_BUILD_TESTS}")
    add_subdirectory(tests)
    include(CTest)
endif()

if(${PROJECT_NAME}_BUILD_PACKAGE)
    message(STATUS "Asked to build package: ${PROJECT_NAME}_BUILD_PACKAGE = ${${PROJECT_NAME}_BUILD_PACKAGE}")

    install(TARGETS nlohmann_json
            EXPORT nlohmann_json-config
            DESTINATION include)

    install(EXPORT nlohmann_json DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}-targets)
    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}-config.cmake)
    install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    include_directories(AFTER ${CMAKE_INSTALL_INCLUDEDIR} ${CMAKE_CURRENT_LIST_DIR}/include)

    configure_package_config_file(  "${PROJECT_SOURCE_DIR}/pack/${PROJECT_NAME}-config.cmake.in"
                            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
                            INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
                            NO_SET_AND_CHECK_MACRO
                            NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    write_basic_package_version_file(   ${PROJECT_NAME}-config-version.cmake
                                VERSION ${PROJECT_VERSION}
                                COMPATIBILITY SameMajorVersion)

    install(EXPORT ${PROJECT_NAME}-targets 
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
            NAMESPACE ${PROJECT_NAME}::)
    install(FILES
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

    # Continue on to the install/package stage..
    set(CPACK_SOURCE_GENERATOR TBZ2 TGZ TXZ ZIP)
    set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME})

    set(PKG_CONFIG_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/pack/pkgconfig.pc.in" "${PKG_CONFIG_FILE_NAME}" @ONLY)
    install(FILES       "${PKG_CONFIG_FILE_NAME}"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

    # Packaging Configuration
    set(CPACK_PACKAGE_NAME                  "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VERSION_MAJOR         "${VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR         "${VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH         "${VERSION_PATCH}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   ${PROJECT_DESCRIPTION})
    set(CPACK_PACKAGE_VENDOR                "SiddiqSoft")
    set(CPACK_PACKAGE_FILE_NAME             "${PROJECT_NAME}-${PROJECT_VERSION}")
    set(CPACK_RESOURCE_FILE_LICENSE         "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README          "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    set(CPACK_SOURCE_IGNORE_FILES           gmock/ gtest/ build/ .DS_Store .gitignore _deps/ tests/ ${CMAKE_BINARY_DIR}/ ${PROJECT_BINARY_DIR}/)
    #set(CPACK_PACKAGE_DIRECTORY             ${PROJECT_BINARY_DIR}/package)

    if(WIN32)
        set(CPACK_GENERATOR ZIP)
    elseif(Darwin)
        set(CPACK_GENERATOR ZIP)
    elseif(APPLE)
        set(CPACK_GENERATOR TGZ productbuild)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(CPACK_GENERATOR TGZ)
    else()
        set(CPACK_GENERATOR TGZ)
    endif()

    message(STATUS "Writing the package for `${CPACK_PACKAGE_NAME}` --> CPACK_PACKAGE_FILE_NAME: ${CPACK_PACKAGE_FILE_NAME}")
    include(CPack)
endif()