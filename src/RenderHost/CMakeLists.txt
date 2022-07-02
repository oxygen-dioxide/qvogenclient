include(${PROJECT_SCRIPTS_DIR}/FindQt.cmake)
include(${PROJECT_SCRIPTS_DIR}/ListUtil.cmake)

project(RenderHost)

set(RENDER_HOST_LIB_NAME RenderHost)

# Include current directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Qt Auto Compiler
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set Empty
set(RENDER_HOST_TEST on)
set(RENDER_HOST_STATIC off)

set(QT_LIBS)
set(SOURCE_FILES)
set(RESOURCE_FILES)

# Current Project Directories
set(RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Res)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Lib)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    if (NOT RENDER_HOST_STATIC)
        # Add RC File
        list(APPEND RESOURCE_FILES ${RES_DIR}/win32/res.rc)
    endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
elseif(APPLE)
else()
    message(FATAL_ERROR "Unsupported System !!!")
endif()

# Add Qt Libs
add_qt_module(QT_LIBS Core)
add_qt_module(QT_LIBS Network)

# Add Sources
file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list_add_recursively(SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.cpp")

# Add Library
if (RENDER_HOST_STATIC)
    add_compile_definitions(RENDER_HOST_BUILD_STATIC)
    add_library(${RENDER_HOST_LIB_NAME} STATIC ${SOURCE_FILES} ${RESOURCE_FILES})
else()
    add_library(${RENDER_HOST_LIB_NAME} SHARED ${SOURCE_FILES} ${RESOURCE_FILES})
endif()

target_compile_definitions(${RENDER_HOST_LIB_NAME} PRIVATE RENDER_HOST_LIBRARY)

# Link Libraries
target_link_libraries(${RENDER_HOST_LIB_NAME} PUBLIC ${QT_LIBS})

target_include_directories(${RENDER_HOST_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add Test
if (RENDER_HOST_TEST)
    add_executable(RenderHostTest ${CMAKE_CURRENT_SOURCE_DIR}/Test/main.cpp)
    target_link_libraries(RenderHostTest PRIVATE ${RENDER_HOST_LIB_NAME})
endif()
