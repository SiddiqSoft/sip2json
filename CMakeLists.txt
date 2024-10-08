#
# sip2json CMakeLists
# Copyright 2020 Abdulkareem Siddiq
#
cmake_minimum_required(VERSION 3.28)

project(sip2json VERSION 1.16.0
				HOMEPAGE_URL "%%myurl%%"
				LANGUAGES CXX
				DESCRIPTION "SIP Parser for Modern C++ (C++23)"
)

# Require C++20
message(STATUS "Require C++23..")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Git REQUIRED)
include(FetchContent)
include(GNUInstallDirs)

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always --abbrev=1 OUTPUT_VARIABLE GIT_COMMIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX MATCH "^[0-9]\.[0-9]\.[0-9]" GIT_VERSION ${GIT_COMMIT_VERSION})

# Produce the version based on the version configured here in the project(xxx VERSION xx.yy.zz) section above
# It is important to use the ${PROJECT_SOURCE_DIR} in order to ensure that the github build works properly!
message(STATUS "Configure version file..${PROJECT_SOURCE_DIR}/src")
configure_file(${PROJECT_SOURCE_DIR}/src/version.in ${PROJECT_SOURCE_DIR}/src/version.h)

add_subdirectory("src")

option(ENABLE_TESTING ON)

if (ENABLE_TESTING)
	enable_testing()
	add_subdirectory("test")
endif()
