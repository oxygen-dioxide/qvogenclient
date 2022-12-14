include(${PROJECT_SCRIPTS_DIR}/FindQt.cmake)
include(${PROJECT_SCRIPTS_DIR}/ListUtil.cmake)

project(RenderHost)

# Runtime Versions (Change if necessary)
set(ONNX_RUNTIME_DIRNAME_WINDOWS "onnxruntime-win-x64-1.11.1")
set(ONNX_RUNTIME_DIRNAME_WINDOWS_GPU "onnxruntime-win-x64-1.11.1")
set(ONNX_RUNTIME_DIRNAME_LINUX "onnxruntime-linux-x64-1.11.1")
set(ONNX_RUNTIME_DIRNAME_LINUX_GPU "onnxruntime-linux-x64-1.11.1")
set(ONNX_RUNTIME_DIRNAME_MACOSX "onnxruntime-osx-x86_64-1.11.1")

# Set Directories
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
endif()

if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
endif()

if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
endif()

# Set Empty
set(ONNX_RUNTIME_DIR)
set(ONNX_RUNTIME_LIB)
set(ONNX_RUNTIME_BIN)

set(QT_LIBS)
set(SOURCE_FILES)
set(RESOURCE_FILES)

# Current Project Directories
set(RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Res)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Lib)

set(VOGEN_MODEL_DIR Models)
set(VOGEN_ROM_DIR Romanization)
set(VOGEN_DICTS_DIR RomDicts)



# Find ONNX Runtime Library
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(ONNX_RUNTIME_DIR ${LIB_DIR}/${ONNX_RUNTIME_DIRNAME_WINDOWS})

    # Add Lib Files
    file(GLOB ONNX_RUNTIME_LIB "${ONNX_RUNTIME_DIR}/lib/*.lib")
    file(GLOB ONNX_RUNTIME_BIN "${ONNX_RUNTIME_DIR}/lib/*.dll" "${ONNX_RUNTIME_DIR}/lib/*.pdb")

    # Add RC File
    list(APPEND RESOURCE_FILES ${RES_DIR}/win32/res.rc)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(ONNX_RUNTIME_DIR ${LIB_DIR}/${ONNX_RUNTIME_DIRNAME_LINUX})
elseif(APPLE)
    set(ONNX_RUNTIME_DIR ${LIB_DIR}/${ONNX_RUNTIME_DIRNAME_MACOSX})
else()
    message(FATAL_ERROR "Unsupported System !!!")
endif()

# Add World
add_subdirectory(World)

# Add Sources
add_qt_module(QT_LIBS Core)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list_add_recursively(SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Meta/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/Meta/*.cpp")

# Add Executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${RESOURCE_FILES})

# Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_LIBS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${ONNX_RUNTIME_LIB})

target_link_libraries(${PROJECT_NAME} PRIVATE world::tool)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE "${ONNX_RUNTIME_DIR}/include")

# Copy Libraries to target path after build
foreach(BIN_FILE ${ONNX_RUNTIME_BIN})
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endforeach()

# Get Synth Dir Name
if(NOT DEFINED SYNTH_DIRNAME)
    set(SYNTH_DIRNAME resources/synth)
endif()

# Copy Resources to resources path after build
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RES_DIR}/${VOGEN_MODEL_DIR} "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${SYNTH_DIRNAME}/${VOGEN_MODEL_DIR}")

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RES_DIR}/${VOGEN_ROM_DIR} "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${SYNTH_DIRNAME}/${VOGEN_ROM_DIR}")

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RES_DIR}/${VOGEN_DICTS_DIR} "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${SYNTH_DIRNAME}/${VOGEN_DICTS_DIR}")
