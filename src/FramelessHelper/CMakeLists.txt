#[[
  MIT License

  Copyright (C) 2022 by wangwenx190 (Yuhang Zhao)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
]]

project (FramelessHelper)

option(FRAMELESSHELPER_BUILD_STATIC "Build FramelessHelper as a static library." OFF)
option(FRAMELESSHELPER_BUILD_WIDGETS "Build FramelessHelper's Widgets module." ON)
option(FRAMELESSHELPER_BUILD_QUICK "Build FramelessHelper's Quick module." OFF)
option(FRAMELESSHELPER_BUILD_EXAMPLES "Build FramelessHelper demo applications." OFF)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    if(NOT DEFINED CMAKE_INTERPROCEDURAL_OPTIMIZATION)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endif()

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
    if(WIN32)
        set(CMAKE_DEBUG_POSTFIX d)
    else()
        set(CMAKE_DEBUG_POSTFIX _debug)
    endif()
endif()

if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
endif()

if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
endif()

if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Quick)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick)

if(TARGET Qt${QT_VERSION_MAJOR}::Core AND TARGET Qt${QT_VERSION_MAJOR}::Gui)
    add_subdirectory(src)
endif()

if(FRAMELESSHELPER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()