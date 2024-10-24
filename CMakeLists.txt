cmake_minimum_required(VERSION 3.25)

# The project version is updated below with the tag from git
#   We need to support "C" language in order for the googletest to build.
project(sip2json
        VERSION 0.0.0
        LANGUAGES CXX C
        HOMEPAGE_URL "%%myurl%%"
        DESCRIPTION "sip2json SIP parser and encode header-only library"
)

if(WIN32)
  message(STATUS "Selecting Visual Studio as generator")
  set(CMAKE_GENERATOR "Visual Studio 17 2022")
  set(CMAKE_GENERATOR_PLATFORM "x64,version=10.0.22621.0")
  set(CMAKE_GENERATOR_TOOLSET "v143")
  set(CMAKE_VS_PLATFORM_TOOLSET_VERSION "14.41.34120")
endif(WIN32)

message(STATUS "Require C++20")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "-- Importing git, gnuinstalldirs..")
find_package(Git REQUIRED)
include(FetchContent)
include(GNUInstallDirs)

message(STATUS "-- Importing the nlohmann package directly..")
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
    GIT_NO_TAGS TRUE
)
FetchContent_MakeAvailable(nlohmann_json)


# Get the branch name and the version tag (requires that we have a semver tag at some point!)
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --is-inside-work-tree OUTPUT_VARIABLE IS_HERE_GIT OUTPUT_STRIP_TRAILING_WHITESPACE)

if(IS_HERE_GIT EQUAL 0)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always --abbrev=1 OUTPUT_VARIABLE GIT_COMMIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "-- Git Version: ${GIT_COMMIT_VERSION}")
    string(REGEX MATCH "^[0-9]\.[0-9]\.[0-9]" GIT_VERSION ${GIT_COMMIT_VERSION})
    # Produce the version based on the version configured here in the project(xxx VERSION xx.yy.zz) section above
    # It is important to use the ${PROJECT_SOURCE_DIR} in order to ensure that the github build works properly!
    message(STATUS "-- Git Version: ${GIT_VERSION}")
    # We will reset the project version to the gitversion tag
    message(STATUS "-- Before update the project version : " ${CMAKE_PROJECT_VERSION} )
    set(CMAKE_PROJECT_VERSION ${GIT_VERSION})
    message(STATUS "-- After updating the project version: " ${CMAKE_PROJECT_VERSION} )
else()
    message(STATUS "-- Not a git repo.. skip git version check..")
endif()

message(STATUS "-- Going for the src..")
add_subdirectory("src")

message(STATUS "-- The project source directory............: ${PROJECT_SOURCE_DIR}")
message(STATUS "-- The PROJECT_BINARY_DIR directory........: ${PROJECT_BINARY_DIR}")
message(STATUS "-- The CMAKE_CURRENT_BINARY_DIR directory..: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "-- The CMAKE_BINARY_DIR directory..........: ${CMAKE_BINARY_DIR}")
message(STATUS "-- The CMAKE_RUNTIME_OUTPUT_DIRECTORY......: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

message(STATUS "The test..")
include(CTest)
enable_testing()
add_subdirectory("tests")
