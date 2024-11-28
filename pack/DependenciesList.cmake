#
# Dependencies List
# https://github.com/cpm-cmake/CPM.cmake
#

# download CPM.cmake..
file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake ${CMAKE_CURRENT_SOURCE_DIR}/pack/CPM.cmake)
# import the helper into our process..
include(pack/CPM.cmake)

# ________________________
# our project dependencies
CPMAddPackage("gh:nlohmann/json@3.11.3")
