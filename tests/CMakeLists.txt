#
# sip2json CMakeLists
# Copyright 2020 Abdulkareem Siddiq
#
cmake_minimum_required(VERSION 3.28)

# Require C++23
message(STATUS "Require C++23..")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Git REQUIRED)
include(FetchContent)
FetchContent_Declare(googletest
					GIT_REPOSITORY https://github.com/google/googletest.git
					GIT_TAG	v1.15.2
)
FetchContent_MakeAvailable(googletest)


FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

file(GLOB testSRC "./test.cpp")

add_executable(sip2json_test ${testSRC})

target_link_libraries(sip2json_test GTest::gtest_main PRIVATE nlohmann_json::nlohmann_json)

include(GoogleTest)
gtest_discover_tests(sip2json_test)
