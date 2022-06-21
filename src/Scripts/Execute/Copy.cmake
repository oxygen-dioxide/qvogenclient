# Usage:
# cmake
# -D type=[copy/copy_if_different]
# -D files="file1|file2|..."
# -D dest="/foo/bar"
# -P Copy.cmake

include(${CMAKE_CURRENT_LIST_DIR}/../Basic.cmake)

check_defined(type)
check_defined(files)
check_defined(dest)

# Message
get_filename_component(SCRIPT_NAME ${CMAKE_CURRENT_LIST_FILE} NAME)
message("Run ${SCRIPT_NAME} Script")

# In case of multiple filenames
unwrap_filenames(${files} FILE_LIST)

# Make destination dir
string(REPLACE "\"" "" dest ${dest})

if(NOT IS_DIRECTORY ${dest})
    file(MAKE_DIRECTORY ${dest})
endif()

# Copy if exists
if(${type} STREQUAL copy)
    foreach(file ${FILE_LIST})
        if(EXISTS ${file})
            message("Copy \"${file}\" to \"${dest}\"")
            execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${file} ${dest})
        endif()
    endforeach()
elseif(${type} STREQUAL copy_if_different)
    foreach(file ${FILE_LIST})
        if(EXISTS ${file})
            message("Copy \"${file}\" to \"${dest}\"")
            execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file} ${dest})
        endif()
    endforeach()
elseif(${type} STREQUAL copy_dir)
    foreach(file ${FILE_LIST})
        if(EXISTS ${file})
            get_filename_component(name ${file} NAME)
            message("Copy \"${file}\" to \"${dest}/${name}\"")
            execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${file} "${dest}/${name}")
        endif()
    endforeach()
endif()
