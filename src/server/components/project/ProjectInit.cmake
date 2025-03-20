# ==================================================================================================
# ============================================== INIT ==============================================
# ==================================================================================================

macro(init_qt)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    add_qt_packages()
endmacro()

macro(init)
    core_init()

    check_qt_min_version()
    init_qt()

    set_build_type_variables()
    version_info_init()

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
endmacro()

# ==================================================================================================
# ====================================== INIT BUILD INTERNAL =======================================
# ==================================================================================================

macro(internal_init_qt_common)
    if (CONFIG_RELEASE)
        target_compile_definitions(${PROJECT_NAME} PUBLIC
            QT_NO_DEBUG_OUTPUT QT_NO_INFO_OUTPUT QT_NO_WARNING_OUTPUT)
    endif()

    if(CONFIG_STATIC)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_LIBRARIES} -static)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_LIBRARIES})
    endif()
endmacro()

macro(internal_apply_compile_definitions)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ${COMPILE_DEFINES})
endmacro()

macro(internal_apply_target_libraries)
    list(REMOVE_DUPLICATES TARGET_LIBRARIES)

    if(CONFIG_STATIC)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${TARGET_LIBRARIES} -static)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE ${TARGET_LIBRARIES})
    endif()
endmacro()

# ==================================================================================================
# =========================================== INIT BUILD ===========================================
# ==================================================================================================

macro(init_qt_gui_build)
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})
    internal_init_qt_common()

    set_target_properties(${PROJECT_NAME} PROPERTIES ${BUNDLE_ID_OPTION} WIN32_EXECUTABLE TRUE)

    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    qt_finalize_executable(${PROJECT_NAME})
endmacro()

macro(init_qt_console_build)
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    internal_init_qt_common()

    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endmacro()

macro(init_build)
    internal_apply_compile_definitions()
    internal_apply_target_libraries()
    update_output_name()
    core_init_namespace()

    if(QT_VERSION_MAJOR EQUAL 6)
        qt_finalize_executable(${PROJECT_NAME})
    endif()

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_PREFIX_PATH}/bin/windeployqt.exe
            $<TARGET_FILE:${PROJECT_NAME}>
            --no-compiler-runtime
            --no-translations
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${PROJECT_SOURCE_DIR}/libs/PortAudio/bin/libportaudio.dll
            ${PROJECT_SOURCE_DIR}/libs/Ffmpeg/bin/avcodec-60.dll
            ${PROJECT_SOURCE_DIR}/libs/Ffmpeg/bin/avutil-58.dll
            ${PROJECT_SOURCE_DIR}/libs/Ffmpeg/bin/avformat-60.dll
            ${PROJECT_SOURCE_DIR}/libs/Ffmpeg/bin/swresample-4.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

    # add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy
    #         $<TARGET_FILE:Qt6::Core>
    #         $<TARGET_FILE_DIR:${PROJECT_NAME}>
    #     COMMAND ${CMAKE_COMMAND} -E copy
    #         $<TARGET_FILE:Qt6::Widgets>
    #         $<TARGET_FILE_DIR:${PROJECT_NAME}>
    # )
endmacro()
