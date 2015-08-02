cmake_minimum_required(VERSION 2.8)
project(redmine)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
  endif()
else()
  message(FATAL_ERROR "Platform ${CMAKE_SYSTEM_NAME} not supported.")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/json)

option(BUILD_CURL_EXE "Set to ON to build cURL executable." OFF)
option(BUILD_CURL_TESTS "Set to ON to build cURL tests." OFF)
option(ENABLE_MANUAL "to provide the built-in manual" OFF)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_MACOSX_RPATH OFF)
endif()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/curl)
set_target_properties(libcurl PROPERTIES COMPILE_FLAGS -w)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/external/json/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external/curl/include
  ${CMAKE_CURRENT_BINARY_DIR}/external/curl/include/curl
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(redmine
  ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/issue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/project.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/redmine.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/request.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/issue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/project.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/redmine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/request.cpp)
target_link_libraries(redmine JSON libcurl)
