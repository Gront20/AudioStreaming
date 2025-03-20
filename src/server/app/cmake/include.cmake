# ==================================================================================================
# ============================================== PATH ==============================================
# ==================================================================================================

if(NOT DEFINED PROJECTS_ROOT_DIR)
    set(PROJECTS_ROOT_DIR ${PROJECT_SOURCE_DIR}/..)
endif()

# ==================================================================================================
# =========================================== COMPONENT ============================================
# ==================================================================================================

macro(move_component_variables_to_parent)
    set(PROJECT_SOURCES  ${PROJECT_SOURCES}  PARENT_SCOPE)
    set(QT_COMPONENTS    ${QT_COMPONENTS}    PARENT_SCOPE)
    set(COMPILE_DEFINES  ${COMPILE_DEFINES}  PARENT_SCOPE)
    set(TARGET_LIBRARIES ${TARGET_LIBRARIES} PARENT_SCOPE)
endmacro()

function(add_component_ex FOLDER NAME)
    set(FILEPATH components/${FOLDER}/${NAME}.cmake)

    if(EXISTS ${PROJECT_SOURCE_DIR}/${FILEPATH})
        # Local/custom components
        include(${PROJECT_SOURCE_DIR}/${FILEPATH})
    elseif(EXISTS ${PROJECTS_ROOT_DIR}/${FILEPATH})
        # Global components
        include(${PROJECTS_ROOT_DIR}/${FILEPATH})
    else()
        # Stop
        message(FATAL_ERROR "Component \"${NAME}\" was not found.")
    endif()

    move_component_variables_to_parent()
endfunction()

function(add_component NAME)
    string(TOLOWER ${NAME} FOLDER)
    add_component_ex(${FOLDER} ${NAME})

    move_component_variables_to_parent()
endfunction()

macro(add_components)
    foreach(NAME IN ITEMS ${ARGN})
        add_component(${NAME})
    endforeach()
endmacro()

# ==================================================================================================
# ============================================ LIBRARY =============================================
# ==================================================================================================

function(_add_library NAME)
    string(TOLOWER ${NAME} FOLDER)
    set(FILEPATH libs/${FOLDER}/${NAME}.cmake)

    message(STATUS "Looking for: ${PROJECT_SOURCE_DIR}/${FILEPATH}")

    if(EXISTS ${PROJECT_SOURCE_DIR}/${FILEPATH})
        include(${PROJECT_SOURCE_DIR}/${FILEPATH})
    else()
        message(FATAL_ERROR "Library \"${NAME}\" was not found.")
    endif()

    move_component_variables_to_parent()
endfunction()
