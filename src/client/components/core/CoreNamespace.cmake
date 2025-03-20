# ==================================================================================================
# =========================================== NAMESPACE ============================================
# ==================================================================================================

function(core_update_namespace_file)
    if(NAMESPACE STREQUAL "")
        set(LINES "BEGIN_NAMESPACE" "END_NAMESPACE")
    else()
        set(LINES "BEGIN_NAMESPACE namespace ${NAMESPACE} {" "END_NAMESPACE }")
    endif()

    write_def_file(namespace "${LINES}")
endfunction()

macro(core_init_namespace)
    core_update_namespace_file()
    core_include_defines_dir()
endmacro()
