cmake_minimum_required(VERSION 3.30)

if(${${PROJECT_NAME}_BUILD_TESTS})
    set(TESTPROJ ${PROJECT_NAME}_tests)

    cpmaddpackage("gh:google/googletest#v1.17.0")

    add_executable(${TESTPROJ})

    # ASAN and Coverage only for Debug builds on Linux
    #if(((CMAKE_CXX_COMPILER_ID MATCHES [Cc][Ll][Aa][Nn][Gg]) 
    #    OR (CMAKE_CXX_COMPILER_ID MATCHES [Gg][Nn][Uu]))
    #   AND (CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg]))
    #    message(STATUS "Using sanitizers for leak and address on ${CMAKE_CXX_COMPILER_ID}")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,leak")
    #    target_link_libraries(${TESTPROJ} PRIVATE -fsanitize=address,leak)
    #    message(STATUS "Enable instrumentation for code coverage")
    #    target_compile_options(${TESTPROJ} PRIVATE -coverage -O0 -g)
    #    target_link_options(${TESTPROJ} PRIVATE -coverage)
    #endif()

    if( CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        add_compile_definitions(${TESTPROJ} PRIVATE DEBUG=1 _DEBUG=1)
    endif()

    target_sources( ${TESTPROJ}
                    PRIVATE
                    ${PROJECT_SOURCE_DIR}/tests/siphelpers.cpp
                    ${PROJECT_SOURCE_DIR}/tests/synthetics.cpp
                    ${PROJECT_SOURCE_DIR}/tests/coverage_tests.cpp
                    ${PROJECT_SOURCE_DIR}/tests/edge_tests.cpp
                    ${PROJECT_SOURCE_DIR}/tests/edge_tests_2.cpp
                    ${PROJECT_SOURCE_DIR}/tests/rule_of_five_tests.cpp
                    ${PROJECT_SOURCE_DIR}/tests/stress_tests.cpp
                    ${PROJECT_SOURCE_DIR}/tests/test.cpp)

    include(GoogleTest)
    enable_testing()

    target_compile_features(${TESTPROJ} PRIVATE cxx_std_20)
    target_compile_options( ${TESTPROJ}
                            PRIVATE
                            $<$<CXX_COMPILER_ID:MSVC>:/std:c++20> )
    
    target_link_libraries(${TESTPROJ} PRIVATE GTest::gtest_main nlohmann_json::nlohmann_json ctre::ctre)

    gtest_discover_tests(${TESTPROJ} XML_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/tests/results"
                        PROPERTIES ENVIRONMENT "SAMPLES_DIR=${PROJECT_SOURCE_DIR}/tests/samples")

    include(CTest)
    set(CTEST_MEMORYCHECK_TYPE "AddressSanitizer")
    set(CTEST_MEMORYCHECK_SANITIZER_OPTIONS "verbosity=1:symbolize=1:abort_on_error=1:detect_leaks=1")
    message(STATUS "  Finished configuring for ${PROJECT_NAME}_BUILD_TESTS = ${${PROJECT_NAME}_BUILD_TESTS}")
endif()