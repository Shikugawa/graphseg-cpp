cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(graphseg_main CXX)
find_package(RapidJSON REQUIRED)

add_executable(graphseg_main main.cpp)

# target_link_libraries(graphseg_main PRIVATE graphseg_module)
