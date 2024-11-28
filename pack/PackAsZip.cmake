# 
# Reference
# https://www.foonathan.net/2022/06/cmake-fetchcontent/
#

set(package_files include/ CMakeLists.txt LICENSE)
set(package_filename ${PROJECT_NAME}-${PROJECT_VERSION}-src.zip)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${package_filename}
    COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_CURRENT_BINARY_DIR}/${package_filename} --format=zip -- ${package_files}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${package_files})
add_custom_target(${PROJECT_NAME}_package DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${package_filename})
