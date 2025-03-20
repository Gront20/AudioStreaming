# ==================================================================================================
# ========================================= INITIALIZATION =========================================
# ==================================================================================================

function(core_init)
    set(CORE_DIR ${CMAKE_CURRENT_FUNCTION_LIST_DIR} PARENT_SCOPE)
    set(GEN_FILE_DIR "main" PARENT_SCOPE)
    set(DEF_FILE_DIR "def" PARENT_SCOPE)
    set(DEF_FILE_EXT "def" PARENT_SCOPE)

    if(NOT DEFINED NAMESPACE)
        set(NAMESPACE "" PARENT_SCOPE)
    endif()
endfunction()

# ==================================================================================================
# ============================================= FILES ==============================================
# ==================================================================================================

function(write_file FILE CONTENT)
    if(EXISTS ${FILE})
        file(READ ${FILE} OLD_CONTENT)

        if(NOT "${CONTENT}" STREQUAL "${OLD_CONTENT}")
            file(WRITE ${FILE} ${CONTENT})
        endif()

    else()
        file(WRITE ${FILE} ${CONTENT})
    endif()
endfunction()

function(write_def_file_ex FILE LINES GUARD)
    list(LENGTH LINES LINES_COUNT)
    if(LINES_COUNT)
        list(JOIN LINES "\n#define " CONTENT)
        string(PREPEND CONTENT "#define ")
    endif()

    set(FILE_BEGIN "#ifndef ${GUARD}\n#define ${GUARD}")
    set(FILE_END "#endif // ${GUARD}")
    set(CONTENT "${FILE_BEGIN}\n\n${CONTENT}\n\n${FILE_END}")

    write_file(${FILE} ${CONTENT})
endfunction()

function(write_def_file TITLE LINES)
    string(REPLACE " " "_" TEMP ${TITLE})
    string(TOUPPER ${TEMP} TEMP)
    set(GUARD "${TEMP}_DEF_FILE")

    string(REPLACE " " "" TEMP ${TITLE})
    string(TOLOWER ${TEMP} TEMP)
    set(DIR ${CMAKE_CURRENT_BINARY_DIR}/${GEN_FILE_DIR}/${DEF_FILE_DIR})
    set(FILE "${DIR}/${TEMP}.${DEF_FILE_EXT}")

    write_def_file_ex(${FILE} "${LINES}" ${GUARD})
endfunction()

# ==================================================================================================
# ============================================== DIRS ==============================================
# ==================================================================================================

macro(core_include_defines_dir)
    include_directories("${CMAKE_CURRENT_BINARY_DIR}/${GEN_FILE_DIR}/${DEF_FILE_DIR}")
endmacro()

# ==================================================================================================
# ============================================ NUMBERS =============================================
# ==================================================================================================

macro(increment VAR)
    MATH(EXPR ${VAR} "${${VAR}}+1")
endmacro()

macro(increment_last LIST_VAR)
    list(POP_BACK ${LIST_VAR} MACRO_TEMP)
    increment(MACRO_TEMP)

    list(APPEND ${LIST_VAR} ${MACRO_TEMP})
    unset(MACRO_TEMP)
endmacro()

# ==================================================================================================
# =========================================== VARIABLES ============================================
# ==================================================================================================

macro(add_list_prefix ITEMS_IN ITEMS_OUT PREFIX)
    set(${ITEMS_OUT})

    foreach(ITEM ${${ITEMS_IN}})
        list(APPEND ${ITEMS_OUT} ${PREFIX}${ITEM})
    endforeach()
endmacro()

macro(set_bool VAR)
    if(${ARGN})
        set(${VAR} TRUE)
    else()
        set(${VAR} FALSE)
    endif()
endmacro()

macro(unset_all)
    foreach(VAR IN ITEMS ${ARGN})
        unset(${VAR})
    endforeach()
endmacro()

# ==================================================================================================
# ============================================= DEBUG ==============================================
# ==================================================================================================

macro(print VAR)
    message("${VAR} = ${${VAR}}")
endmacro()
