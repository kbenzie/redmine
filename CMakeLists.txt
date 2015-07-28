cmake_minimum_required(VERSION 2.8)
project(redmine)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
  endif()
else()
  message(FATAL_ERROR "Platform ${CMAKE_SYSTEM_NAME} not supported.")
endif()

# TODO: json
# TODO: curl

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(redmine
  ${CMAKE_CURRENT_SOURCE_DIR}/source/config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/issue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/project.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/redmine.cpp)
