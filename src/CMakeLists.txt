#
# sip2json CMakeLists
# Copyright 2020 Abdulkareem Siddiq
# Source Files
# 
cmake_minimum_required(VERSION 3.28)

file(GLOB sip2json_SRC
		"*/*.hpp"
)

add_library(sip2json INTERFACE)

message(STATUS "Project version        : " ${CMAKE_PROJECT_VERSION})
message(STATUS "CMake install directory: " ${CMAKE_INSTALL_INCLUDEDIR})

#target_link_libraries(sip2json nlohmann_json)

target_include_directories(sip2json INTERFACE .)