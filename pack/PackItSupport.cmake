if(${PROJECT_NAME}_BUILD_PACKAGE)
    message(STATUS "-- Building the package `${PROJECT_NAME}_BUILD_PACKAGE`")
    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/pack/${PROJECT_NAME}.pc.in
                    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
                    @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

    write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
                                    COMPATIBILITY AnyNewerVersion)

    install(FILES   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
                    ${CMAKE_CURRENT_SOURCE_DIR}/pack/${PROJECT_NAME}-config.cmake
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})

    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}-targets
            FILE_SET HEADERS)

    install(EXPORT ${PROJECT_NAME}-targets
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

    # Continue on to the install/package stage..
    set(CPACK_SOURCE_GENERATOR TBZ2 TGZ TXZ ZIP)
    set(CPACK_SOURCE_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME})

    # Packaging Configuration
    set(CPACK_PACKAGE_NAME                  "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VERSION_MAJOR         "${VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR         "${VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH         "${VERSION_PATCH}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   ${PROJECT_DESCRIPTION})
    set(CPACK_PACKAGE_VENDOR                "SiddiqSoft")
    set(CPACK_PACKAGE_FILE_NAME             "${PROJECT_NAME}-${PROJECT_VERSION}")
    set(CPACK_RESOURCE_FILE_LICENSE         "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README          "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    set(CPACK_SOURCE_IGNORE_FILES           gmock/ gtest/ build/ .DS_Store .gitignore _deps/ tests/ ${CMAKE_BINARY_DIR}/ ${PROJECT_BINARY_DIR}/)
    #set(CPACK_PACKAGE_DIRECTORY             ${PROJECT_BINARY_DIR}/package)

    if(WIN32)
        set(CPACK_GENERATOR ZIP)
    elseif(Darwin)
        set(CPACK_GENERATOR ZIP)
    elseif(APPLE)
        set(CPACK_GENERATOR TGZ productbuild)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(CPACK_GENERATOR TGZ)
    else()
        set(CPACK_GENERATOR TGZ)
    endif()

    message(STATUS "Writing the package for `${CPACK_PACKAGE_NAME}` --> CPACK_PACKAGE_FILE_NAME: ${CPACK_PACKAGE_FILE_NAME}")
    include(CPack)
endif()