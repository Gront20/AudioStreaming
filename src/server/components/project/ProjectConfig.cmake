# ==================================================================================================
# ============================================ SETTERS =============================================
# ==================================================================================================

macro(set_cpp_version VALUE)
    set(CMAKE_CXX_STANDARD ${VALUE})
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endmacro()

macro(set_qt_min_version VALUE)
    set(QT_MINIMUM_REQUIRED_VERSION ${VALUE})
endmacro()

macro(add_qt_components)
    set(QT_COMPONENTS ${QT_COMPONENTS} ${ARGN})
endmacro()

macro(add_compile_definitions)
    set(COMPILE_DEFINES ${COMPILE_DEFINES} ${ARGN})
endmacro()

macro(add_target_libraries)
    set(TARGET_LIBRARIES ${TARGET_LIBRARIES} ${ARGN})
endmacro()

macro(set_build_type_variables)
    # static build value
    get_target_property(TEMP_QT_TYPE Qt${QT_VERSION_MAJOR}::Core TYPE)
    set_bool(CONFIG_STATIC TEMP_QT_TYPE STREQUAL STATIC_LIBRARY)
    unset(TEMP_QT_TYPE)

    # debug build value
    set_bool(CONFIG_DEBUG CMAKE_BUILD_TYPE STREQUAL Debug)
    set_bool(CONFIG_RELEASE NOT CONFIG_DEBUG)
endmacro()

# ==================================================================================================
# ============================================= CHECK ==============================================
# ==================================================================================================

macro(check_qt_min_version)
    if(DEFINED QT_MINIMUM_REQUIRED_VERSION)
        find_program(QMAKE_EXECUTABLE NAMES qmake HINTS ${QTDIR} ENV QTDIR PATH_SUFFIXES bin)
        execute_process(COMMAND ${QMAKE_EXECUTABLE} -query QT_VERSION OUTPUT_VARIABLE QT_VERSION)
        if(QT_VERSION LESS QT_MINIMUM_REQUIRED_VERSION)
            MESSAGE(FATAL_ERROR "Minimum required Qt version: ${QT_MINIMUM_REQUIRED_VERSION}.
                Installed version: ${QT_VERSION}")
        endif()
    endif()
endmacro()

# ==================================================================================================
# ============================================= FILES ==============================================
# ==================================================================================================

macro(update_output_name)
    if(NOT DEFINED BIN_NAME_WHITESPACE)
        set(BIN_NAME_WHITESPACE "-")
    endif()

    if(NOT DEFINED BIN_NAME_POSTFIX)
        set(BIN_NAME_POSTFIX TRUE)
    endif()

    if(NOT DEFINED BIN_NAME)
        string(TOLOWER ${APP_NAME} TEMP)
        string(REPLACE " " ${BIN_NAME_WHITESPACE} TEMP ${TEMP})

        set(BIN_NAME ${TEMP})
    else()
        set(TEMP ${BIN_NAME})
    endif()

    if (BIN_NAME_POSTFIX)
        if(CONFIG_STATIC)
            set(TEMP "${TEMP}${BIN_NAME_WHITESPACE}static")
        endif()

        if(CONFIG_DEBUG)
            set(TEMP "${TEMP}${BIN_NAME_WHITESPACE}debug")
        endif()
    endif()

    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${TEMP})

    unset(TEMP)
endmacro()

# ==================================================================================================
# ============================================ PACKAGES ============================================
# ==================================================================================================

macro(add_qt_packages)
    list(REMOVE_DUPLICATES QT_COMPONENTS)

    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})

    add_list_prefix(QT_COMPONENTS QT_LIBRARIES "Qt${QT_VERSION_MAJOR}::")
endmacro()
