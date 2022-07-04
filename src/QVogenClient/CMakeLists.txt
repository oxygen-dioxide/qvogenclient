include(${PROJECT_SCRIPTS_DIR}/Basic.cmake)
include(${PROJECT_SCRIPTS_DIR}/FindQt.cmake)
include(${PROJECT_SCRIPTS_DIR}/ListUtil.cmake)
include(${PROJECT_SCRIPTS_DIR}/Translate.cmake)

# ------------------------------ Basic information ------------------------------

# Include current directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Qt Auto Compiler
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Variables
set(QT_LIBS)
set(QT_RESOURCES)
set(RC_FILES)
set(MAC_RES)

set(PROJECT_SOURCES)
set(PROJECT_HEADERS)
set(INCLUDE_PATHS)

# ------------------------------ Macros/Functions ------------------------------

# Use to conveniently add files in this CMakeLists
macro(add_current_files List)
    list_append_with_prefix(${List} ${CMAKE_CURRENT_SOURCE_DIR}/ ${ARGN})
endmacro()

# ------------------------------ Include/Import/Link ------------------------------
add_qt_module(QT_LIBS Widgets)
add_qt_module(QT_LIBS Svg)
add_qt_module(QT_LIBS Multimedia)

include_qt_private(Gui)
include_qt_private(Widgets)

# Qt resources file (.qrc)
add_current_files(QT_RESOURCES Resources/res.qrc)

# Add resource files
if(SYS_WIN)
    # Add windows resource file
    add_current_files(RC_FILES Resources/logo/win/res.rc)
elseif(SYS_MAC)
    # Add mac icon file
    add_current_files(MAC_RES Resources/logo/mac/QVogenClient.icns)
endif()

# Add fixed files
list_add_recursively(PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Framework/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Framework/*.cc")
list_add_recursively(PROJECT_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/Framework/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/Framework/*.h")
list_add_recursively(PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Meta/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Meta/*.cc")
list_add_recursively(PROJECT_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/Meta/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/Meta/*.h")
list_add_recursively(PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/*.cc")
list_add_recursively(PROJECT_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/*.h")
list_add_recursively(PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Staging/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Staging/*.cc")
list_add_recursively(PROJECT_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/Staging/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/Staging/*.h")

# Add specific files
add_current_files(PROJECT_SOURCES
    main.cpp
)

add_current_files(PROJECT_HEADERS
)

# Add executable
if(SYS_WIN)
    # Windows need rc files
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${QT_RESOURCES} ${RC_FILES})
elseif(SYS_MAC)
    foreach (Item ${MAC_RES})
        set_source_files_properties(${Item} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    endforeach()
    # Mac need icon
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${QT_RESOURCES} ${MAC_RES})
else()
    # Linux
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${QT_RESOURCES})
endif()

# Link OS dependent libraries
if(SYS_WIN)
    target_link_libraries(${PROJECT_NAME} PRIVATE dwmapi user32 shell32 advapi32 winmm)
elseif(SYS_MAC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY
          MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Resources/logo/mac/Info.plist.in")
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE asound)
endif()

# Set Properties
if(APP_DEPLOY)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_VERSION 1.0.0.0
        MACOSX_BUNDLE_SHORT_VERSION_STRING 1.0
    )
endif()

# Link Custom libraries
target_link_libraries(${PROJECT_NAME} PRIVATE FramelessHelper::Widgets)

target_link_libraries(${PROJECT_NAME} PRIVATE QMidi)

target_link_libraries(${PROJECT_NAME} PRIVATE QUtauStd)

target_link_libraries(${PROJECT_NAME} PRIVATE wave)

target_link_libraries(${PROJECT_NAME} PRIVATE QuaZip::QuaZip)

target_link_libraries(${PROJECT_NAME} PRIVATE RenderHost)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_LIBS})

# Write location to logs
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    -D filename=${PROJECT_ARCHIVE_DIR}/main_target_lst_file
    -D content=$<TARGET_FILE:${PROJECT_NAME}>
    -P ${PROJECT_SCRIPTS_DIR}/Execute/Log.cmake
)

# ------------------------------ Translations ------------------------------
find_qt_module(LinguistTools)

# Project Translations
set(TS_FILES)
set(QM_FILES)

add_current_files(TS_FILES Resources/translations/qvogenclient_zh_CN.ts) # Simplified Chinese

# Add lupdate
# Avoid using qt5_create_translation because CMake Clean will remove the ts files which are specified as an output file
add_custom_target(lupdate_task)
add_lupdate_commands(${TS_FILES} ${PROJECT_HEADERS} ${PROJECT_SOURCES} TARGET lupdate_task)

# Add lrelease
qt5_add_translation(QM_FILES ${TS_FILES})
add_custom_target(lrelease_task ALL DEPENDS ${QM_FILES})

# Copy .qm files to translations dir after build if they exists
wrap_filenames(QM_FILES_WRAP ${QM_FILES})
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    -D type=copy
    -D files=${QM_FILES_WRAP}
    -D dest=$<TARGET_FILE_DIR:${PROJECT_NAME}>/${TRANSLATIONS_DIRNAME}
    -P ${PROJECT_SCRIPTS_DIR}/Execute/Copy.cmake
)

# Library Translations
set(BASE_TS_FILES)
set(BASE_QM_FILES)

add_current_files(BASE_TS_FILES Resources/translations/qtbase_zh_CN.ts)

# This ts file is not generated by current project, it comes from Qt source code
# But we need to add an lrelease task to update it when it changes

# Add lrelease
qt5_add_translation(BASE_QM_FILES ${BASE_TS_FILES})
add_custom_target(base_lrelease_task ALL DEPENDS ${BASE_QM_FILES})

# Copy .qm files to translations dir after build if they exists
wrap_filenames(BASE_QM_FILES_WRAP ${BASE_QM_FILES})
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    -D type=copy
    -D files=${BASE_QM_FILES_WRAP}
    -D dest=$<TARGET_FILE_DIR:${PROJECT_NAME}>/${TRANSLATIONS_DIRNAME}
    -P ${PROJECT_SCRIPTS_DIR}/Execute/Copy.cmake
    DEPENDS base_lrelease_task
)

# ------------------------------ Promote code experience ------------------------------

# Include directories
add_current_files(INCLUDE_PATHS
    Framework
    Framework/Custom
    Framework/Helpers
    Framework/Metatypes
    Framework/Metatypes/Core
    Framework/Metatypes/User
    Framework/Visual
    Meta
    Staging
    Staging/Application
    Staging/Application/Managers
    Staging/Backend/Basic
    Staging/Backend/Project
    Staging/Backend/Utils
    Staging/Window
    Staging/Window/Managers
    Staging/Window/Tabs
    Staging/Frontend/Basic
    Staging/Frontend/Utils
    Vendor/Frontend
    Vendor/Frontend/Utils
    Vendor/Frontend/Window
    Vendor/Backend
    Vendor/Backend/Basic
    Vendor/Backend/Import
    Vendor/Backend/Utils
    Vendor/LowLevel
    Vendor/LowLevel/CommonTune
    Vendor/LowLevel/Utils
)

foreach(Path ${INCLUDE_PATHS})
    include_directories(${Path})
endforeach()
