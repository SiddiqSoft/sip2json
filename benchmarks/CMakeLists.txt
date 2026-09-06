cmake_minimum_required(VERSION 3.31)

if(${${PROJECT_NAME}_BUILD_BENCHMARKS})
    set(BENCHPROJ ${PROJECT_NAME}_benchmarks)

    set( CMAKE_CXX_STANDARD 20)
    set( CMAKE_CXX_STANDARD_REQUIRED On)
    set( CMAKE_CXX_EXTENSIONS Off)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_ENABLE_C_CLANG_TIDY ON)
    set(CMAKE_ENABLE_CXX_CLANG_TIDY ON)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    add_executable(${BENCHPROJ})

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

    if( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
        message(STATUS "  >> MSVC benchmark optimization flags..")
        set(OPTIMIZE_FLAGS
            "/O2"
            "/Oi"
            "/Ot"
            "/Ob3"
            "/GL"
            "/DNDEBUG"
        )
        set(LINK_FLAGS
            "/LTCG"
            "/OPT:REF"
            "/OPT:ICF"
        )
        target_compile_options(${BENCHPROJ} PRIVATE ${OPTIMIZE_FLAGS})
        target_link_options(${BENCHPROJ} PRIVATE ${LINK_FLAGS})
    else()
        message(STATUS "  >> GCC/Clang benchmark optimization flags..")
        set(ARCH_FLAGS "-mcpu=native")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
            set(ARCH_FLAGS "-march=native")
        endif()

        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag("${ARCH_FLAGS}" COMPILER_SUPPORTS_ARCH_NATIVE)
        if(NOT COMPILER_SUPPORTS_ARCH_NATIVE)
            set(ARCH_FLAGS "")
        endif()

        set(OPTIMIZE_FLAGS
            "-O3"
            ${ARCH_FLAGS}
            "-flto=auto"
            "-fomit-frame-pointer"
            "-DNDEBUG"
        )
        if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            list(APPEND OPTIMIZE_FLAGS "-fno-semantic-interposition")
        endif()

        set(LINK_FLAGS "-flto=auto")

        target_compile_options(${BENCHPROJ} PRIVATE ${OPTIMIZE_FLAGS})
        target_link_options(${BENCHPROJ} PRIVATE ${LINK_FLAGS})
    endif()

    if( CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        message(STATUS "  >> Setting DEBUG macro..")
        target_compile_definitions(${BENCHPROJ} PRIVATE DEBUG=1 _DEBUG=1)
    endif()

    cpmaddpackage("gh:google/googletest#v1.17.0")

    cpmaddpackage(
        NAME benchmark
        GITHUB_REPOSITORY google/benchmark
        GIT_TAG v1.9.5
        OPTIONS
            "BENCHMARK_ENABLE_TESTING OFF"
            "BENCHMARK_ENABLE_INSTALL OFF"
            "BENCHMARK_ENABLE_GTEST_TESTS OFF"
    )

    target_sources(${BENCHPROJ}
                   PRIVATE
                   ${PROJECT_SOURCE_DIR}/benchmarks/benchmark.cpp)

    target_compile_features(${BENCHPROJ} PRIVATE cxx_std_20)

    target_link_libraries(${BENCHPROJ}
                          PRIVATE
                          sip2json::sip2json
                          benchmark::benchmark
                          benchmark::benchmark_main)

    target_include_directories(${BENCHPROJ}
                               PRIVATE
                               ${PROJECT_SOURCE_DIR}/include)

    message(STATUS "  Finished configuring for ${PROJECT_NAME}_BUILD_BENCHMARKS = ${${PROJECT_NAME}_BUILD_BENCHMARKS}")
endif()
