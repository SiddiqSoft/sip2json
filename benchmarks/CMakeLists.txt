cmake_minimum_required(VERSION 3.29)

if(${${PROJECT_NAME}_BUILD_BENCHMARKS})
    set(BENCHPROJ ${PROJECT_NAME}_benchmarks)

    cpmaddpackage(
        NAME benchmark
        GITHUB_REPOSITORY google/benchmark
        GIT_TAG v1.9.5
        OPTIONS
            "BENCHMARK_ENABLE_TESTING OFF"
            "BENCHMARK_ENABLE_INSTALL OFF"
            "BENCHMARK_ENABLE_GTEST_TESTS OFF"
    )

    add_executable(${BENCHPROJ})

    target_sources(${BENCHPROJ}
                   PRIVATE
                   ${PROJECT_SOURCE_DIR}/benchmarks/benchmark.cpp)

    target_compile_features(${BENCHPROJ} PRIVATE cxx_std_20)
    target_compile_options(${BENCHPROJ}
                           PRIVATE
                           $<$<CXX_COMPILER_ID:MSVC>:/std:c++20 /O2>
                           $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-O2>)

    target_link_libraries(${BENCHPROJ}
                          PRIVATE
                          benchmark::benchmark
                          benchmark::benchmark_main
                          nlohmann_json::nlohmann_json)

    target_include_directories(${BENCHPROJ}
                               PRIVATE
                               ${PROJECT_SOURCE_DIR}/include)

    message(STATUS "  Finished configuring for ${PROJECT_NAME}_BUILD_BENCHMARKS = ${${PROJECT_NAME}_BUILD_BENCHMARKS}")
endif()
