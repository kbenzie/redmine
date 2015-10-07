# Copyright (C) 2015 Kenenth Benzie
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
# OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 2.8)
project(redmine)

option(REDMINE_BUILD_CURL "Enabling building curl from source." OFF)

if(${CMAKE_BUILD_TYPE})
  if(${CMAKE_BUILD_TYPE} MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    set(REDMINE_DEBUG TRUE)
  endif()
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(REDMINE_PLATFORM_LINUX TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(REDMINE_PLATFORM_MAC TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(REDMINE_PLATFORM_WINDOWS TRUE)
  option(REDMINE_BUILD_CURL "Enabling building curl from source." ON)
else()
  message(FATAL_ERROR "Platform ${CMAKE_SYSTEM_NAME} not supported.")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/generate/include/defines.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/include/defines.h)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
  endif()
elseif(WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/json)

if(${REDMINE_BUILD_CURL})
  message(STATUS "Using in source libcurl with static linkage.")

  option(BUILD_CURL_EXE "Set to ON to build cURL executable." OFF)
  option(BUILD_CURL_TESTS "Set to ON to build cURL tests." OFF)
  option(ENABLE_MANUAL "to provide the built-in manual" OFF)
  option(CURL_STATICLIB "Set to ON to build libcurl with static linking." ON)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/curl)
  set_target_properties(libcurl PROPERTIES COMPILE_FLAGS -w)

  set(CURL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/external/curl/include
    ${CMAKE_CURRENT_BINARY_DIR}/external/curl/include/curl)
  set(CURL_LIBRARIES libcurl)
else()
  find_package(CURL)
  if(${CURL_FOUND})
    message(STATUS "Using system libcurl with dynamic linkage.")
  else()
    message(FATAL_ERROR
      "Could not find the curl library, please install using your systems "
      "package manager or set REDMINE_BUILD_CURL.")
  endif()
endif()

include_directories(
  ${CURL_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external/json/include)

add_executable(redmine
  ${CMAKE_CURRENT_SOURCE_DIR}/include/command_line.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/enumeration.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/error.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/issue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/http.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/project.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/membership.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/redmine.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/role.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/user.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tracker.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/util.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/command_line.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/enumeration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/issue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/http.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/project.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/membership.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/redmine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/role.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/tracker.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/user.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/util.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/version.cpp)
target_link_libraries(redmine JSON ${CURL_LIBRARIES})
