# ==================================================================================================
# ========================================= CONTROL MACROS =========================================
# ==================================================================================================

macro(src_component)
    set(COMPONENT_FLAG ON)
endmacro()

macro(src_library)
    set(LIBRARY_FLAG ON)
endmacro()

macro(src_begin dir name)
    set(TEMP_ROOT ${CMAKE_CURRENT_LIST_DIR}/${dir})
    set(TEMP_FILE_DIR ${TEMP_ROOT})

    if(COMPONENT_FLAG)
        set(TEMP_NAME "Components/${name}")
        include_directories(${CMAKE_CURRENT_LIST_DIR})
    elseif(LIBRARY_FLAG)
        set(TEMP_ROOT ${CMAKE_CURRENT_LIST_DIR}/include/${dir})
        set(TEMP_FILE_DIR ${TEMP_ROOT})

        set(TEMP_NAME "Libraries/${name}")
        include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
    else()
        set(TEMP_NAME ${name})
        include_directories(${TEMP_ROOT})
    endif()
endmacro()

macro(src_set_dir)
    if(${ARGC})
        set(TEMP_FILE_DIR ${TEMP_ROOT}/${ARGV0})
    else()
        set(TEMP_FILE_DIR ${TEMP_ROOT})
    endif()
endmacro()

macro(src_end)
    source_group(TREE ${TEMP_ROOT} PREFIX "${TEMP_NAME}/Header Files" FILES ${TEMP_HEADERS})
    source_group(TREE ${TEMP_ROOT} PREFIX "${TEMP_NAME}/Source Files" FILES ${TEMP_SOURCES})
    source_group(TREE ${TEMP_ROOT} PREFIX "${TEMP_NAME}/Form Files" FILES ${TEMP_FORMS})
    source_group(TREE ${TEMP_ROOT} PREFIX "${TEMP_NAME}/Text Files" FILES ${TEMP_TEXTS})

    if(COMPONENT_FLAG)
        source_group(TREE ${TEMP_ROOT}/res PREFIX "${TEMP_NAME}/Resources" FILES ${TEMP_RESOURCES})
    else()
        source_group("${TEMP_NAME}/Resources" FILES ${TEMP_RESOURCES})
    endif()

    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${TEMP_HEADERS} ${TEMP_SOURCES} ${TEMP_FORMS}
        ${TEMP_TEXTS} ${TEMP_RESOURCES})

    unset_all(TEMP_HEADERS TEMP_SOURCES TEMP_FORMS TEMP_TEXTS TEMP_RESOURCES
        TEMP_FILE_DIR TEMP_ROOT TEMP_NAME COMPONENT_FLAG LIBRARY_FLAG)
endmacro()

macro(src_translations_end)
    set(TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
        ${TEMP_HEADERS} ${TEMP_SOURCES} ${TEMP_FORMS} ${TEMP_TEXTS})

    unset_all(TEMP_HEADERS TEMP_SOURCES TEMP_FORMS TEMP_TEXTS TEMP_RESOURCES
        TEMP_FILE_DIR TEMP_ROOT TEMP_NAME COMPONENT_FLAG LIBRARY_FLAG)
endmacro()

# ==================================================================================================
# ========================================== ADD ONE ITEM ==========================================
# ==================================================================================================

macro(add_header name)
    set(TEMP_HEADERS ${TEMP_HEADERS} ${TEMP_FILE_DIR}/${name}.h)
endmacro()

macro(add_hppeader name)
    set(TEMP_HEADERS ${TEMP_HEADERS} ${TEMP_FILE_DIR}/${name}.hpp)
endmacro()

macro(add_source name)
    set(TEMP_SOURCES ${TEMP_SOURCES} ${TEMP_FILE_DIR}/${name}.cpp)
endmacro()

macro(add_txt name)
    set(TEMP_TEXTS ${TEMP_TEXTS} ${TEMP_FILE_DIR}/${name})
endmacro()

macro(add_tri name) # translation include file
    set(TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
        ${CMAKE_SOURCE_DIR}/app/translations/${name}.tri)
endmacro()

macro(add_class name)
    set(TEMP_HEADERS ${TEMP_HEADERS} ${TEMP_FILE_DIR}/${name}.h)
    set(TEMP_SOURCES ${TEMP_SOURCES} ${TEMP_FILE_DIR}/${name}.cpp)
endmacro()

macro(add_gui_class name)
    set(TEMP_HEADERS ${TEMP_HEADERS} ${TEMP_FILE_DIR}/${name}.h)
    set(TEMP_SOURCES ${TEMP_SOURCES} ${TEMP_FILE_DIR}/${name}.cpp)
    set(TEMP_FORMS   ${TEMP_FORMS}   ${TEMP_FILE_DIR}/${name}.ui)
endmacro()

macro(add_resource name)
    if(COMPONENT_FLAG)
        set(TEMP_RESOURCES ${TEMP_RESOURCES} ${TEMP_FILE_DIR}/res/${name})
    else()
        set(TEMP_RESOURCES ${TEMP_RESOURCES} ${TEMP_FILE_DIR}/resources/${name}.qrc)
    endif()
endmacro()

# ==================================================================================================
# ======================================= ADD MULTIPLE ITEMS =======================================
# ==================================================================================================

macro(add_header_n)
    foreach(arg IN ITEMS ${ARGN})
        add_header(${arg})
    endforeach()
endmacro()

macro(add_source_n)
    foreach(arg IN ITEMS ${ARGN})
        add_source(${arg})
    endforeach()
endmacro()

macro(add_txt_n)
    foreach(arg IN ITEMS ${ARGN})
        add_txt(${arg})
    endforeach()
endmacro()

macro(add_class_n)
    foreach(arg IN ITEMS ${ARGN})
        add_class(${arg})
    endforeach()
endmacro()

macro(add_gui_class_n)
    foreach(arg IN ITEMS ${ARGN})
        add_gui_class(${arg})
    endforeach()
endmacro()
