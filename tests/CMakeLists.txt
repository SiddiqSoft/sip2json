cmake_minimum_required(VERSION 3.25)

# Require C++20
message(STATUS "Require C++20..")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Git REQUIRED)
include(FetchContent)

# Get the googletest
FetchContent_Declare(googletest
					GIT_REPOSITORY https://github.com/google/googletest.git
					GIT_TAG	v1.15.2
)
FetchContent_MakeAvailable(googletest)

file(GLOB testSRC "./test.cpp")

add_executable(sip2json_test ${testSRC})

target_link_libraries(sip2json_test PRIVATE GTest::gtest_main)
target_link_libraries(sip2json_test PRIVATE nlohmann_json::nlohmann_json)

include(GoogleTest)

# In order to properly locate the tests folder, we must provide the environment variable
# SAMPLES_DIR. This should
gtest_discover_tests(sip2json_test
                     XML_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/tests"
					 PROPERTIES ENVIRONMENT "SAMPLES_DIR=${PROJECT_SOURCE_DIR}/tests/samples"
)
