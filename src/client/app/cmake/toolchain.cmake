# ==================================================================================================
# ======================================== CMAKE VARIABLES =========================================
# ==================================================================================================

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# ==================================================================================================
# ========================================== QT VARIABLES ==========================================
# ==================================================================================================

##NOTE: machine dependent path
set(QT_HOST_PATH "C:/Qt/6.6/6.6.2"
    CACHE PATH "host path for Qt")
set(QT_HOST_PATH_CMAKE_DIR "${QT_HOST_PATH}/lib/cmake"
    CACHE PATH "host path for Cmake")
