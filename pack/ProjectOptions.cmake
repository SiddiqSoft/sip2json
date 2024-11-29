# Include helpers from CMake
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(GNUInstallDirs)
include(FetchContent)

# _____________________
# Project-Level Options

option(${PROJECT_NAME}_DEVMODE "Flag set when we're in the project's home directory" OFF)
# We run the tests by default when we're in development mode for this library
option(${PROJECT_NAME}_BUILD_TESTS "Build tests. Uncheck for install only runs" OFF)
option(${PROJECT_NAME}_BUILD_PACKAGE "Package. Uncheck for install only runs" OFF)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(${PROJECT_NAME}_DEVMODE ON)
    message(STATUS "Project is in the `library developer's home` directory. ${${PROJECT_NAME}_DEVMODE}-->ON")
    message(STATUS  "Configuration options: ${PROJECT_NAME}_BUILD_TESTS  -- > ${${PROJECT_NAME}_BUILD_TESTS}\n"
    "                          CI_BUILDID            -- > ${CI_BUILDID} --> ${CURRENT_PROJECT_VERSION}\n"
    "                          ${PROJECT_NAME}_BUILD_PACKAGE --> ${${PROJECT_NAME}_BUILD_PACKAGE}")
endif()
