cmake_minimum_required(VERSION 3.30)

if(${${PROJECT_NAME}_BUILD_TESTS})
    set(TESTPROJ ${PROJECT_NAME}_tests)

    cpmaddpackage("gh:google/googletest#v1.15.2")

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
    
    target_link_libraries(${TESTPROJ} PRIVATE GTest::gtest_main nlohmann_json::nlohmann_json)

    gtest_discover_tests(${TESTPROJ} XML_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/tests/results"
                        PROPERTIES ENVIRONMENT "SAMPLES_DIR=${PROJECT_SOURCE_DIR}/tests/samples")

    include(CTest)
    message(STATUS "  Finished configuring for ${PROJECT_NAME}_BUILD_TESTS = ${${PROJECT_NAME}_BUILD_TESTS}")
endif()