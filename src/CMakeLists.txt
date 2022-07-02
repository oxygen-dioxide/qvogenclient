# ------------------------------ Basic information ------------------------------

cmake_minimum_required(VERSION 3.17)

project(QVogenClient LANGUAGES CXX)

# CPP Version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Application Information
set(APP_DEPLOY ON)
set(APP_VERSION 0.2.21)

# Set Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

if(NOT DEFINED PROJECT_SCRIPTS_DIR)
    set(PROJECT_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/cmake) # Scripts
endif()

if(NOT DEFINED PROJECT_ARCHIVE_DIR)
    set(PROJECT_ARCHIVE_DIR ${PROJECT_BINARY_DIR}/etc) # Etc
endif()

if(NOT DEFINED PROJECT_DEPLOY_PATH)
    set(PROJECT_DEPLOY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../release) # Release
endif()

set(APP_TRANSLATIONS_DIR translations)
set(SYNTH_DIRNAME resources/synth)
set(TRANSLATIONS_DIRNAME translations)

# Determining Current System
set(SYS_WIN OFF)
set(SYS_MAC OFF)
set(SYS_LINUX OFF)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(SYS_WIN ON)
    message(STATUS "[INFO] Current System is Windows")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(SYS_LINUX ON)
    message(STATUS "[INFO] Current System is Linux")
elseif(APPLE)
    set(SYS_MAC ON)
    message(STATUS "[INFO] Current System is Macintosh")
else()
    message(FATAL_ERROR "Unsupported System !!!")
endif()

# Global
add_compile_definitions(APP_VERSION="${APP_VERSION}")

# ------------------------------ Sub Projects ------------------------------

# Libraries
add_subdirectory(FramelessHelper)

add_subdirectory(ImportFormats)

add_subdirectory(QuaZip)

# Modules
add_subdirectory(RenderHost)

# Application
add_subdirectory(QVogenClient)

# ------------------------------ Deploy Targets ------------------------------
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

if(SYS_WIN)
    string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} ARCH_NAME)
    set(RELEASE_DIR_NAME qvogenclient-win-${ARCH_NAME}-${APP_VERSION})

    add_custom_target(app_deploy
        COMMAND ${CMAKE_COMMAND}
        -D ETC_DIR="${PROJECT_ARCHIVE_DIR}"
        -D RELEASE_DIR="${PROJECT_DEPLOY_PATH}/${RELEASE_DIR_NAME}"
        -D QT_TOOLS_DIR="${_qt5Core_install_prefix}/bin"
        -P "${PROJECT_SCRIPTS_DIR}/Deploy/WinDeploy.cmake"
        DEPENDS lrelease_task base_lrelease_task QVogenClient RenderHost
    )

elseif(SYS_MAC)
else()
endif()
