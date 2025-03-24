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
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
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

macro(generate_exe)
    set(CONFIG_RELEASE ON)

    get_filename_component(MINGW_PATH ${CMAKE_CXX_COMPILER} DIRECTORY)

    get_filename_component(MINGW_PATH ${MINGW_PATH} DIRECTORY)

    message(STATUS "MinGW path: ${MINGW_PATH}")

    if(QT_VERSION_MAJOR EQUAL 6)
        qt_finalize_executable(${PROJECT_NAME})
    endif()

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_PREFIX_PATH}/bin/windeployqt.exe
            $<TARGET_FILE:${PROJECT_NAME}>
            --no-compiler-runtime
            --no-translations
            --release
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/*.d
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/*.cmake
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${PROJECT_SOURCE_DIR}/libs/PortAudio/bin/libportaudio.dll
            ${PROJECT_SOURCE_DIR}/libs/Ffmpeg/bin/swresample-4.dll
            ${PROJECT_SOURCE_DIR}/libs/Ffmpeg/bin/avutil-58.dll
            ${MINGW_PATH}/bin/libgcc_s_seh-1.dll
            ${MINGW_PATH}/bin/libstdc++-6.dll
            ${MINGW_PATH}/bin/libwinpthread-1.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

endmacro()

macro(init_build)

    # message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
    internal_apply_compile_definitions()
    internal_apply_target_libraries()
    update_output_name()
    core_init_namespace()
    generate_exe()
endmacro()
