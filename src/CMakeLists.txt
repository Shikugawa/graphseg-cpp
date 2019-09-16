cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(graphseg_main CXX)

link_directories("/usr/local/lib")
set(LIBRARIES graphseg_module libmecab.a)

add_executable(graphseg_main main.cpp)
target_link_libraries(graphseg_main PRIVATE ${LIBRARIES})
