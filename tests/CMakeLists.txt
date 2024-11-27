cmake_minimum_required(VERSION 3.25)

if(${${PROJECT_NAME}_BUILD_TESTS})
    set(TESTPROJ ${PROJECT_NAME}_tests)

    message(STATUS "Importing (via URL) googletest..")
    FetchContent_Declare(googletest URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
                                    FIND_PACKAGE_ARGS NAMES GTest)
    FetchContent_MakeAvailable(googletest)

    message(STATUS "Importing (via URL) nlohmann_json..")
    FetchContent_Declare(nlohmann_json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
    FetchContent_MakeAvailable(nlohmann_json)
    
    add_executable(${TESTPROJ})

    # For GCC/Clang coverage data to be included ONLY for our test
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fprofile-arcs -ftest-coverage -fprofile-instr-generate -fcoverage-mapping")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fprofile-arcs -ftest-coverage -fprofile-instr-generate -fcoverage-mapping")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fprofile-instr-generate")
    message(STATUS "Setting compiler flags for coverage: ${CMAKE_CXX_FLAGS}")
    message(STATUS "Setting linker flags for coverage  : ${CMAKE_LINKER_FLAGS}")

    target_sources( ${TESTPROJ}
                    PRIVATE
                    ${PROJECT_SOURCE_DIR}/tests/test.cpp)


    include(GoogleTest)
    enable_testing()

    target_compile_features(${TESTPROJ} PRIVATE cxx_std_20)
    target_compile_options( ${TESTPROJ}
                            PRIVATE
                            $<$<CXX_COMPILER_ID:MSVC>:/std:c++20> )
    target_link_libraries(${TESTPROJ} PRIVATE GTest::gtest_main)
    target_link_libraries(${TESTPROJ} PRIVATE nlohmann_json::nlohmann_json)
    gtest_discover_tests(${TESTPROJ} XML_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/tests/results"
                        PROPERTIES ENVIRONMENT "SAMPLES_DIR=${PROJECT_SOURCE_DIR}/tests/samples")

    include(CTest)
    message(STATUS "  Finished configuring for ${PROJECT_NAME}_BUILD_TESTS = ${${PROJECT_NAME}_BUILD_TESTS}")
endif()