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

# ____________________________________
# Darwin (macOS) Compiler Resolution
# Ensures clean builds without requiring maintainers to flip xcode-select between
# Xcode.app and CommandLineTools. Directly resolves the clang/clang++ binary to bypass
# /usr/bin/xcrun and avoid xcodebuild -find clang++ failures (e.g. exit code 65280).
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    # C++ Compiler resolution
    if(NOT DEFINED CMAKE_CXX_COMPILER OR NOT EXISTS "${CMAKE_CXX_COMPILER}")
        set(_DARWIN_CXX_CANDIDATES
            "/Library/Developer/CommandLineTools/usr/bin/clang++"
            "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++"
            "/opt/homebrew/opt/llvm/bin/clang++"
        )
        foreach(_cand IN LISTS _DARWIN_CXX_CANDIDATES)
            if(EXISTS "${_cand}")
                set(CMAKE_CXX_COMPILER "${_cand}" CACHE FILEPATH "CXX compiler" FORCE)
                message(STATUS "sip2json - Selected macOS CXX compiler: ${CMAKE_CXX_COMPILER}")
                break()
            endif()
        endforeach()
        unset(_DARWIN_CXX_CANDIDATES)
    endif()

    # C Compiler resolution (used by dependencies like GoogleTest)
    if(NOT DEFINED CMAKE_C_COMPILER OR NOT EXISTS "${CMAKE_C_COMPILER}")
        set(_DARWIN_C_CANDIDATES
            "/Library/Developer/CommandLineTools/usr/bin/clang"
            "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
            "/opt/homebrew/opt/llvm/bin/clang"
        )
        foreach(_cand IN LISTS _DARWIN_C_CANDIDATES)
            if(EXISTS "${_cand}")
                set(CMAKE_C_COMPILER "${_cand}" CACHE FILEPATH "C compiler" FORCE)
                message(STATUS "sip2json - Selected macOS C compiler: ${CMAKE_C_COMPILER}")
                break()
            endif()
        endforeach()
        unset(_DARWIN_C_CANDIDATES)
    endif()

    # Build tool resolution (standalone make or ninja)
    if(NOT DEFINED CMAKE_MAKE_PROGRAM)
        if(CMAKE_GENERATOR STREQUAL "Ninja")
            if(EXISTS "/opt/homebrew/bin/ninja")
                set(CMAKE_MAKE_PROGRAM "/opt/homebrew/bin/ninja" CACHE FILEPATH "Ninja program" FORCE)
            endif()
        elseif(EXISTS "/Library/Developer/CommandLineTools/usr/bin/make")
            set(CMAKE_MAKE_PROGRAM "/Library/Developer/CommandLineTools/usr/bin/make" CACHE FILEPATH "Make program" FORCE)
        endif()
    endif()
endif()

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
# CI & Environment Detection
set(IS_CI_BUILD FALSE)
if(DEFINED CI_BUILDID
   OR DEFINED ENV{CI}
   OR DEFINED ENV{TF_BUILD}
   OR DEFINED ENV{GITHUB_ACTIONS}
   OR DEFINED ENV{AGENT_NAME}
   OR DEFINED ENV{BUILD_BUILDID})
    set(IS_CI_BUILD TRUE)
endif()

# Default clang-format to OFF on CI, and ON for local development
if(IS_CI_BUILD)
    set(${PROJECT_NAME}_ENABLE_CLANG_FORMAT_DEFAULT OFF)
else()
    set(${PROJECT_NAME}_ENABLE_CLANG_FORMAT_DEFAULT ON)
endif()

# ____________________________________
# Options; define and declare defaults
option(${PROJECT_NAME}_BUILD_TESTS "${PROJECT_NAME} - Build tests. Uncheck for install only runs" OFF)
option(${PROJECT_NAME}_BUILD_BENCHMARKS "${PROJECT_NAME} - Build benchmarks" OFF)
option(${PROJECT_NAME}_ENABLE_CLANG_FORMAT "${PROJECT_NAME} - Format source code with clang-format prior to build (local Debug builds only)" ${${PROJECT_NAME}_ENABLE_CLANG_FORMAT_DEFAULT})

# ____________________________________
#  Library Definition
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# ____________________________________
# Source Code Formatting (clang-format)
# Ensures clang-format is applied to our source code prior to build on local Debug builds only
if(${PROJECT_NAME}_ENABLE_CLANG_FORMAT)
    find_program(CLANG_FORMAT_EXE
        NAMES clang-format clang-format-20 clang-format-19 clang-format-18 clang-format-17 clang-format-16 clang-format-15 clang-format-14
        HINTS
            "$ENV{VCINSTALLDIR}/Tools/Llvm/bin"
            "$ENV{VCINSTALLDIR}/Tools/Llvm/x64/bin"
            "$ENV{VSINSTALLDIR}/VC/Tools/Llvm/bin"
            "$ENV{VSINSTALLDIR}/VC/Tools/Llvm/x64/bin"
            "/opt/homebrew/bin"
            "/usr/local/bin"
    )

    if(CLANG_FORMAT_EXE)
        file(GLOB_RECURSE FORMAT_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/*.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/*.h"
        )

        if(FORMAT_SOURCES)
            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/run_clang_format.cmake" "
if(CONFIG AND NOT CONFIG MATCHES \"^[Dd][Ee][Bb][Uu][Gg]$\")
    message(STATUS \"${PROJECT_NAME} - Skipping clang-format in \${CONFIG} configuration (Debug only).\")
    return()
endif()

if(FORMAT_SOURCES)
    message(STATUS \"${PROJECT_NAME} - Applying clang-format to source files prior to build...\")
    execute_process(
        COMMAND \"${CLANG_FORMAT_EXE}\" -i --style=file \${FORMAT_SOURCES}
        WORKING_DIRECTORY \"${CMAKE_CURRENT_SOURCE_DIR}\"
        RESULT_VARIABLE res
    )
    if(NOT res EQUAL 0)
        message(WARNING \"${PROJECT_NAME} - clang-format exited with code \${res}\")
    endif()
endif()
")

            add_custom_target(${PROJECT_NAME}_format ALL
                COMMAND ${CMAKE_COMMAND}
                    "-DCONFIG=$<CONFIG>"
                    "-DFORMAT_SOURCES=${FORMAT_SOURCES}"
                    -P "${CMAKE_CURRENT_BINARY_DIR}/run_clang_format.cmake"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                COMMENT "${PROJECT_NAME} - Checking and applying clang-format in Debug mode..."
                VERBATIM
            )
            add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_format)

            if(NOT TARGET clang-format)
                add_custom_target(clang-format DEPENDS ${PROJECT_NAME}_format)
            endif()
            if(NOT TARGET format)
                add_custom_target(format DEPENDS ${PROJECT_NAME}_format)
            endif()
            message(STATUS "${PROJECT_NAME} - clang-format enabled (${CLANG_FORMAT_EXE}); formatting will run for local Debug builds.")
        endif()
    else()
        message(STATUS "${PROJECT_NAME} - clang-format executable not found; skipping automatic code formatting.")
    endif()
else()
    if(IS_CI_BUILD)
        message(STATUS "${PROJECT_NAME} - CI environment detected; clang-format is disabled.")
    else()
        message(STATUS "${PROJECT_NAME} - clang-format is disabled (${PROJECT_NAME}_ENABLE_CLANG_FORMAT=OFF).")
    endif()
endif()

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
