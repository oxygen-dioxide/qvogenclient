# Usage:
# cmake
# -D content="..."
# -D filename="/foo/bar"
# -P Log.cmake

include(${CMAKE_CURRENT_LIST_DIR}/../Basic.cmake)

check_defined(content)
check_defined(filename)

# Message
get_filename_component(SCRIPT_NAME ${CMAKE_CURRENT_LIST_FILE} NAME)
message("Run ${SCRIPT_NAME} Script")

string(REPLACE "\"" "" filename ${filename})
file(WRITE ${filename} "${content}\n")