project(graphseg_module CXX)
add_library(${PROJECT_NAME} INTERFACE)

find_package(RapidJSON REQUIRED)

target_include_directories(${PROJECT_NAME}
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)