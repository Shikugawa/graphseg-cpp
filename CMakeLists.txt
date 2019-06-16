cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(graphseg CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "CMAKE_BUILD_TYPE not specified: Use Release by default.")
endif(NOT CMAKE_BUILD_TYPE)

set(OUTPUT_DEBUG Debug/bin)
set(OUTPUT_RELEASE Release/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${OUTPUT_RELEASE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DNDEBUG)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unknown-pragmas -Wconversion")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
endif()

# ======== Boost ========
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.65.0.0 REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
# =======================

# add_subdirectory(graphseg)
add_subdirectory(src)
