# set args
include(${TARGET_ARGS_FILE})

# include scripts
include(${CORE_DIR}/CoreFunctions.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/VersionInfo.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/VersionInfoFiles.cmake)

# run code
get_app_version("${VERSION_INFO_DIR}/version.ver")
update_project_defines_file()
update_versioninfo_file()
