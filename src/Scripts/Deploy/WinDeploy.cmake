if (NOT WIN32)
    message(FATAL_ERROR "OS Type not match!!!")
endif ()

include(${CMAKE_CURRENT_LIST_DIR}/../Basic.cmake)

# ----------------------------- Check Env -----------------------------

check_defined(ETC_DIR)
check_defined(RELEASE_DIR)
check_defined(QT_TOOLS_DIR)


# ----------------------------- Get Logs -----------------------------

file(READ ${ETC_DIR}/main_target_lst_file MAIN_TARGET_PATH)
string(STRIP ${MAIN_TARGET_PATH} MAIN_TARGET_PATH)

get_filename_component(MAIN_TARGET ${MAIN_TARGET_PATH} NAME)
get_filename_component(BIN_DIR ${MAIN_TARGET_PATH} DIRECTORY)


# ----------------------------- Deploy -----------------------------

set(WIN_DEPLOY_QT_EXECUTABLE ${QT_TOOLS_DIR}/windeployqt.exe)

# Get All Targets
file(GLOB PROJECT_TARGETS "${BIN_DIR}/*.exe" "${BIN_DIR}/*.dll")

# Make release dir
if (EXISTS ${RELEASE_DIR})
    file(REMOVE_RECURSE ${RELEASE_DIR})
endif ()
file(MAKE_DIRECTORY ${RELEASE_DIR})

# Copy file
wrap_filenames(PROJECT_TARGETS_WRAP ${PROJECT_TARGETS})
execute_process(COMMAND ${CMAKE_COMMAND}
        -D type=copy
        -D files=${PROJECT_TARGETS_WRAP}
        -D dest=${RELEASE_DIR}
        -P ${CMAKE_CURRENT_LIST_DIR}/../Execute/Copy.cmake
        )

# Execute Windeployqt
execute_process(
        COMMAND ${WIN_DEPLOY_QT_EXECUTABLE} "${RELEASE_DIR}/${MAIN_TARGET}"
        WORKING_DIRECTORY ${RELEASE_DIR}
)

# Delete Default Translations
file(REMOVE_RECURSE ${RELEASE_DIR}/translations)

# Copy Directories
set(APP_DIRS ${BIN_DIR}/resources ${BIN_DIR}/translations)
wrap_filenames(APP_DIRS_WRAP ${APP_DIRS})
execute_process(COMMAND ${CMAKE_COMMAND}
        -D type=copy_dir
        -D files=${APP_DIRS_WRAP}
        -D dest=${RELEASE_DIR}
        -P ${CMAKE_CURRENT_LIST_DIR}/../Execute/Copy.cmake
        )