# ==================================================================================================
# ========================================= VERSION FILES ==========================================
# ==================================================================================================

function(get_app_version_build FILE)
    if(EXISTS ${FILE})
        file(STRINGS ${FILE} TEMP)

        if("${TEMP}" MATCHES "^[0-9]+$")
            set(VERSION_BUILD ${TEMP})
        else()
            message("WARNING: the version build file is invalid; the default value is used.")
        endif()

    else()
        message("WARNING: the version build file not found; the default value is used.")
    endif()

    if(NOT DEFINED VERSION_BUILD)
        set(VERSION_BUILD "0")
    else()
        increment(VERSION_BUILD)
    endif()

    file(WRITE "${FILE}" ${VERSION_BUILD})

    set(APP_VERSION_BUILD ${VERSION_BUILD} PARENT_SCOPE)
endfunction()

function(get_app_version FILE)
    if(EXISTS ${FILE})
        file(STRINGS ${FILE} TEMP)

        if("${TEMP}" MATCHES "^[0-9]+;[0-9]+;[0-9]+$")
            set(VERSION ${TEMP})
        else()
            message("WARNING: the version file is invalid; the default value is used.")
        endif()

    else()
        message("WARNING: the version file not found; the default value is used.")
    endif()

    if(NOT DEFINED VERSION)
        set(VERSION "0;0;0")

        list(JOIN VERSION "\n" TEMP)
        file(WRITE "${FILE}" ${TEMP})
    endif()

    get_app_version_build("${VERSION_INFO_DIR}/version-build.ver")

    set(PRODUCT_VERSION ${VERSION} PARENT_SCOPE)
    set(APP_VERSION ${VERSION} ${APP_VERSION_BUILD} PARENT_SCOPE)
endfunction()

# ==================================================================================================
# ====================================== PROJECT DEFINES FILE ======================================
# ==================================================================================================

function(update_project_defines_file)
    list(GET APP_VERSION 0 VERSION_MAJOR)
    list(GET APP_VERSION 1 VERSION_MINOR)
    list(GET APP_VERSION 2 VERSION_MICRO)
    list(GET APP_VERSION 3 VERSION_BUILD)

    set(LINES "APP_NAME \"${APP_NAME}\""
        "APP_DESC \"${APP_DESCRIPTION}\""
        "BIN_NAME \"${BIN_NAME}\""
        "VERSION_MAJOR ${VERSION_MAJOR}"
        "VERSION_MINOR ${VERSION_MINOR}"
        "VERSION_MICRO ${VERSION_MICRO}"
        "VERSION_BUILD ${VERSION_BUILD}"
    )

    write_def_file(project "${LINES}")
endfunction()

# ==================================================================================================
# ================================== VERSION INFO IMPLEMENTATION ===================================
# ==================================================================================================

function(rc_set_icons)
    if(APP_ICONS STREQUAL "")
        unset(APP_ICONS)
    endif()

    if(NOT DEFINED APP_ICONS)
        set(ICON_FILE ${PROJECT_SOURCE_DIR}/app/resources/${PROJECT_NAME}.ico)

        if(EXISTS ${ICON_FILE})
            set(APP_ICONS ${ICON_FILE})
        endif()
    endif()

    if (APP_ICONS)
        set(INDEX 1)
        set(ICONS "\n")

        foreach(ICON IN LISTS APP_ICONS)
            string(APPEND ICONS "IDI_ICON${INDEX} ICON \"${ICON}\"\n")
            increment(INDEX)
        endforeach()
    endif()

    set(ICONS ${ICONS} PARENT_SCOPE)
endfunction()

macro(rc_set_content)
    set(RC_CONTENT
"#include <windows.h>
${ICONS}
VS_VERSION_INFO VERSIONINFO
    FILEVERSION ${APP_VERSION_COMMA}
    FILEFLAGSMASK 0x3fL
#ifdef _DEBUG
    FILEFLAGS VS_FF_DEBUG
#else
    FILEFLAGS 0x0L
#endif
    FILEOS VOS_NT_WINDOWS32
    FILETYPE ${FILE_TYPE}
    FILESUBTYPE VFT2_UNKNOWN
    BEGIN
        BLOCK \"StringFileInfo\"
        BEGIN
            BLOCK \"040904b0\"
            BEGIN
                VALUE \"FileDescription\", \"${FILE_DESCRIPTION}\\0\"
                VALUE \"FileVersion\", \"${APP_VERSION_DOT}\\0\"
                VALUE \"Comments\", \"\\0\"
            END
        END
        BLOCK \"VarFileInfo\"
        BEGIN
            VALUE \"Translation\", 0x0409, 1200
        END
    END
/* End of Version info */\n"
    )
endmacro()

macro(rc_set_variables)
    # version
    list(JOIN APP_VERSION "," APP_VERSION_COMMA)
    list(JOIN APP_VERSION "." APP_VERSION_DOT)

    #file type
    if(PROJECT_TYPE STREQUAL "EXECUTABLE")
        set(FILE_TYPE "VFT_APP")
    else()
        set(FILE_TYPE "VFT_DLL")
    endif()

    # file description
    set(FILE_DESCRIPTION ${APP_NAME}")

    # copyright
    string(TIMESTAMP CURRENT_YEAR "%Y")
    if(NOT ${APP_YEAR} STREQUAL ${CURRENT_YEAR})
        set(APP_YEAR "${APP_YEAR}-${CURRENT_YEAR}")
    endif()

    set(COPYRIGHT "${APP_YEAR} ${COMPANY_NAME}")

    # file name
    set(OUTPUT_NAME ${OUTPUT_NAME}.exe)
endmacro()

# ==================================================================================================
# ======================================= VERSION INFO FILE ========================================
# ==================================================================================================

function(update_versioninfo_file)
    rc_set_icons()
    rc_set_variables()
    rc_set_content()

    write_file(${RC_FILE} ${RC_CONTENT})
endfunction()
