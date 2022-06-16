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

set(SOURCE_FILES)
set(RESOURCE_FILES)

# Find ONNX Runtime Library
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(ONNX_RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${ONNX_RUNTIME_DIRNAME_WINDOWS}")
    file(GLOB ONNX_RUNTIME_LIB
        "${ONNX_RUNTIME_DIR}/lib/*.lib")
    file(GLOB ONNX_RUNTIME_BIN
        "${ONNX_RUNTIME_DIR}/lib/*.dll"
        "${ONNX_RUNTIME_DIR}/lib/*.pdb")

    # Add RC File
    list(APPEND RESOURCE_FILES win32/res.rc)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(ONNX_RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${ONNX_RUNTIME_DIRNAME_LINUX}")
elseif(APPLE)
    set(ONNX_RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${ONNX_RUNTIME_DIRNAME_MACOSX}")
else()
    message(FATAL_ERROR "Unsupported System !!!")
endif()

# Add Sources
file(GLOB SOURCE_FILES main.cpp)

# Add Executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${RESOURCE_FILES})

# Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${ONNX_RUNTIME_LIB})

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE "${ONNX_RUNTIME_DIR}/include")

# Copy Libraries
foreach(BIN_FILE ${ONNX_RUNTIME_BIN})
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${BIN_FILE}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endforeach()
