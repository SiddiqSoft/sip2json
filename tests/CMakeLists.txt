cmake_minimum_required(VERSION 3.30)

if(${${PROJECT_NAME}_BUILD_TESTS})
    set(TESTPROJ ${PROJECT_NAME}_tests)

    if(NOT CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD 23)
    endif()
    set( CMAKE_CXX_STANDARD_REQUIRED On)
    set( CMAKE_CXX_EXTENSIONS Off)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_ENABLE_C_CLANG_TIDY ON)
    set(CMAKE_ENABLE_CXX_CLANG_TIDY ON)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    cpmaddpackage("gh:google/googletest#v1.17.0")

    add_executable(${TESTPROJ})

    # Check if mold is available on the system
    find_program(MOLD_LINKER mold)

    if(MOLD_LINKER)
        message(STATUS "  >> Found mold linker, using it for build.")
        # Apply mold as the linker for executables and shared libraries
        add_link_options("-fuse-ld=mold")
    else()
        message(STATUS "  >> mold not found, falling back to default linker.")
    endif()

    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")

    if(CLANG_TIDY_EXE AND CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        # This runs the analyzer with all default checks enabled
        # We skip anything that is not in our project.
        message(STATUS "  >> Using clang-tidy checks=*")
        set(CMAKE_CXX_CLANG_TIDY 
            ${CLANG_TIDY_EXE} 
            "-checks=*"
            "--header-filter=^(${CMAKE_CURRENT_SOURCE_DIR}/|${PROJECT_SOURCE_DIR}/include/)")
    endif()

    option(${PROJECT_NAME}_ENABLE_COVERAGE "${PROJECT_NAME} - Enable code coverage instrumentation" OFF)

    # UndefinedBehaviorSanitizer is not available on Windows
    if( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
        message(STATUS "  >> UBSAN is not supported on Windows/MSVC, skipping..")
        message(STATUS "  >> Coverage flags is not supported on Windows/MSVC, skipping..")
        set(DEBUG_COMPILE_FLAGS /Zi /Od)
        set(DEBUG_LINK_FLAGS /DEBUG:FULL)
        set(OTHER_FLAGS /vmg /limitTemplateNotes:0 /templateDepth:2000)
    else()
        set(ASAN_FLAGS "-fsanitize=address")
        set(UBSAN_FLAGS "-fsanitize=undefined")
        if(${PROJECT_NAME}_ENABLE_COVERAGE)
            message(STATUS "  >> Code coverage instrumentation enabled.")
            set(COVERAGE_FLAGS "-coverage")
        endif()
        set(OTHER_FLAGS "-g")
        set(DEBUG_COMPILE_FLAGS -g3 -O0)

        # Remove stale GCDA coverage files before building/executing test binary to avoid "cannot merge previous GCDA file" warnings
        add_custom_command(
            TARGET ${TESTPROJ} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E rm -f "${CMAKE_CURRENT_BINARY_DIR}/*.gcda"
            COMMAND ${CMAKE_COMMAND} -E rm -f "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${TESTPROJ}.dir/*.gcda"
            COMMENT "Cleaning stale GCDA coverage files..."
        )
    endif()

    # LeakSanitizer is not available on Windows and Darwin/macOS with AppleClang
    if( CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_SYSTEM_NAME STREQUAL "Windows" )
        message(STATUS "  >> LSAN is not supported on Windows/MSVC and AppleClang, skipping..")
    else()
        set(LSAN_FLAGS "-fsanitize=leak")
    endif()

    message(STATUS "  >> Enable compile options for ${TESTPROJ} -- ${OPTIMIZE_FLAGS} ${OTHER_FLAGS} ${COVERAGE_FLAGS} ${ASAN_FLAGS} ${LSAN_FLAGS} ${UBSAN_FLAGS}")
    target_compile_options(${TESTPROJ} PRIVATE  ${DEBUG_COMPILE_FLAGS}  ${COVERAGE_FLAGS} ${OPTIMIZE_FLAGS} ${OTHER_FLAGS} ${ASAN_FLAGS} ${LSAN_FLAGS} ${UBSAN_FLAGS})
    target_link_options(${TESTPROJ} PRIVATE ${DEBUG_LINK_FLAGS}  ${COVERAGE_FLAGS} ${OPTIMIZE_FLAGS} ${OTHER_FLAGS} ${ASAN_FLAGS} ${LSAN_FLAGS} ${UBSAN_FLAGS})

    if( CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        message(STATUS "  >> Setting DEBUG macro..")
        target_compile_definitions(${TESTPROJ} PRIVATE DEBUG=1 _DEBUG=1)
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
                    ${PROJECT_SOURCE_DIR}/tests/bug_trigger_tests.cpp
                    ${PROJECT_SOURCE_DIR}/tests/security_tests.cpp
                    ${PROJECT_SOURCE_DIR}/tests/test.cpp)

    include(GoogleTest)
    enable_testing()

    target_compile_features(${TESTPROJ} PRIVATE cxx_std_${CMAKE_CXX_STANDARD})
    
    target_link_libraries(${TESTPROJ} PRIVATE GTest::gtest_main nlohmann_json::nlohmann_json ctre::ctre)

    gtest_discover_tests(${TESTPROJ} XML_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/tests/results"
                        PROPERTIES ENVIRONMENT "SAMPLES_DIR=${PROJECT_SOURCE_DIR}/tests/samples")

    include(CTest)
    message(STATUS "  Finished configuring for ${PROJECT_NAME}_BUILD_TESTS = ${${PROJECT_NAME}_BUILD_TESTS}")
endif()