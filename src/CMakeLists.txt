cmake_minimum_required(VERSION 3.25)

file(GLOB sip2json
     "*/*.hpp"
     "*.hpp"
)

add_library(sip2json INTERFACE)

message(STATUS "CMake install directory: " ${CMAKE_INSTALL_INCLUDEDIR})

target_include_directories(sip2json PUBLIC ${NLOHMANNJSON_INCLUDE_DIRS})

target_include_directories(sip2json
                           INTERFACE
                           $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)