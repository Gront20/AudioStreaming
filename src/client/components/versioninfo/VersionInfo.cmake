# ==================================================================================================
# ============================================= PUBLIC =============================================
# ==================================================================================================

macro(add_app_icon FILE)
    list(APPEND APP_ICONS ${PROJECT_SOURCE_DIR}/app/resources/${FILE})
endmacro()

macro(version_info_init)
    set_version_info_variables()
    set_version_info_sources()

    include(${VERSION_INFO_DIR}/about.cmake)
endmacro()

macro(version_info_init_build)

endmacro()

# ==================================================================================================
# ========================================= IMPLEMENTATION =========================================
# ==================================================================================================

macro(set_version_info_variables)
    set(VERSION_INFO_DIR ${PROJECT_SOURCE_DIR}/app/cmake/versioninfo)
    set(RC_FILE ${PROJECT_BINARY_DIR}/${GEN_FILE_DIR}/${PROJECT_NAME}-versioninfo.rc)
endmacro()

function(set_version_info_sources)
    set(ABOUT_FILE ${VERSION_INFO_DIR}/about.cmake)
    if (EXISTS ${ABOUT_FILE})
        set(FILES ${FILES} ${ABOUT_FILE})
    endif()

    set(VERSION_FILE ${VERSION_INFO_DIR}/version.ver)
    if (EXISTS ${VERSION_FILE})
        set(FILES ${FILES} ${VERSION_FILE})
    endif()

    set(VERSION_BUILD_FILE ${VERSION_INFO_DIR}/version-build.ver)
    if (EXISTS ${VERSION_BUILD_FILE})
        set(FILES ${FILES} ${VERSION_BUILD_FILE})
    endif()

    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${FILES} PARENT_SCOPE)
    source_group("App/About" FILES ${FILES})
endfunction()

function(arg_file_add_block FILE_CONTENT TITLE)
    set(BLOCK_TITLE "# ${TITLE}")
    set(TEXT ${${FILE_CONTENT}}${BLOCK_TITLE})

    foreach(VAR IN ITEMS ${ARGN})
        set(TEMP "\nset(${VAR} \"${${VAR}}\")")
        set(TEXT ${TEXT}${TEMP})
    endforeach()

    set(${FILE_CONTENT} ${TEXT}\n\n PARENT_SCOPE)
endfunction()

function(create_version_info_argument_file)
    get_target_property(PROJECT_TYPE ${PROJECT_NAME} TYPE)
    get_target_property(OUTPUT_NAME ${PROJECT_NAME} OUTPUT_NAME)
    set(FILE "${PROJECT_BINARY_DIR}/${GEN_FILE_DIR}/cmake/versioninfoargs.cmake")

    arg_file_add_block(CONTENT "dirs"
        PROJECT_SOURCE_DIR VERSION_INFO_DIR CORE_DIR)
    arg_file_add_block(CONTENT "project"
        PROJECT_NAME PROJECT_TYPE OUTPUT_NAME BIN_NAME)
    arg_file_add_block(CONTENT "rc file vars"
        RC_FILE)
    arg_file_add_block(CONTENT "core vars"
        GEN_FILE_DIR DEF_FILE_DIR DEF_FILE_EXT)

    write_file(${FILE} ${CONTENT})
endfunction()

function(add_custom_target_version_info)
    set(FILE "${PROJECT_BINARY_DIR}/${GEN_FILE_DIR}/cmake/versioninfoargs.cmake")

    add_custom_target(version-info-custom-target COMMAND ${CMAKE_COMMAND}
        -D TARGET_ARGS_FILE=${FILE}
        -P ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/VersionInfoCustomTarget.cmake)

    add_dependencies(${PROJECT_NAME} version-info-custom-target)
endfunction()

macro(apply_version_info_file)
    set_source_files_properties(${RC_FILE} PROPERTIES GENERATED TRUE)
    target_sources(${PROJECT_NAME} PRIVATE ${RC_FILE})
endmacro()
