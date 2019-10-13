project(graphseg_module CXX)
add_library(${PROJECT_NAME} INTERFACE)

set(RapidJSON_DIR "~/vcpkg/packages/rapidjson_x64-osx/share/rapidjson")
find_package(RapidJSON REQUIRED)

target_include_directories(${PROJECT_NAME}
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)