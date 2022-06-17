cmake_minimum_required(VERSION 3.17)

project(QVogenClient LANGUAGES CXX)

# CPP Version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Application Information
set(APP_DEPLOY ON)
set(APP_VERSION 0.2.17)

# Set Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Make Resources Directory
set(APP_RESOURCES_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)
if (NOT IS_DIRECTORY ${APP_RESOURCES_DIR})
    file(MAKE_DIRECTORY ${APP_RESOURCES_DIR})
endif()

# Determining Current System
set(SYS_WIN OFF)
set(SYS_MAC OFF)
set(SYS_LINUX OFF)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(SYS_WIN ON)
    message(STATUS "[INFO] Current System is Windows")
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(SYS_LINUX ON)
    message(STATUS "[INFO] Current System is Linux")
elseif (APPLE)
    set(SYS_MAC ON)
    message(STATUS "[INFO] Current System is Macintosh")
else ()
    message(FATAL_ERROR "Unsupported System !!!")
endif ()

# Global
add_compile_definitions(APP_VERSION="${APP_VERSION}")

# Libraries
add_subdirectory(FramelessHelper)

add_subdirectory(QMidi)

add_subdirectory(QUtauStd)

add_subdirectory(QuaZip)

# Modules
add_subdirectory(RenderHost)

# Application
add_subdirectory(QVogenClient)
