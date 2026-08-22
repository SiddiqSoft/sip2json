cmake_minimum_required(VERSION 3.31)

if(${${PROJECT_NAME}_BUILD_TESTS})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/regression" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/regression")
        add_subdirectory(regression)
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/compliance" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/compliance")
        add_subdirectory(compliance)
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/validation" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/validation")
        add_subdirectory(validation)
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/coverage" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/coverage")
        add_subdirectory(coverage)
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/security" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/security")
        add_subdirectory(security)
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vulnerability" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vulnerability")
        add_subdirectory(vulnerability)
    endif()

    message(STATUS "  Finished configuring all test suites for ${PROJECT_NAME}")
endif()