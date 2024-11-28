#
# Dependencies List
# https://github.com/cpm-cmake/CPM.cmake
#

# download CPM.cmake..
file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake ${CMAKE_CURRENT_SOURCE_DIR}/pack/CPM.cmake)
# import the helper into our process..
include(pack/CPM.cmake)

# Wrap the depdendency list in a function to prevent options from leaking to the parent
function(DependenciesList)
    # ________________________
    # our project dependencies
    #cpmaddpackage("gh:nlohmann/json#9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03")# v3.11.3
    CPMAddPackage(  NAME nlohmann_json
                    VERSION 3.11.3
                    GITHUB_REPOSITORY nlohmann/json
                    OPTIONS "JSON_BuildTests OFF")
    # This is the fastest download; avoid using the shortcut since this repo is huge and we'd end up
    # downloading a ton of history for this project just to link to it.
    #CPMAddPackage(  NAME nlohmann_json
    #                VERSION 3.11.3
    #                URL https://github.com/nlohmann/json/releases/download/v3.11.3/include.zip)
    
    #if(nlohmann_json_ADDED)	 
    #    add_library(nlohmann_json INTERFACE)	 
    #    target_include_directories(nlohmann_json INTERFACE ${nlohmann_json_SOURCE_DIR})	 
    #endif()
endfunction()
