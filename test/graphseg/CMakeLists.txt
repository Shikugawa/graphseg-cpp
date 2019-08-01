cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(graphseg_test CXX)

# the test program
add_executable(graphseg_test test.cpp)
target_link_libraries(graphseg_test
                      PRIVATE graphseg_module GTest::GTest GTest::Main)

# add google test
gtest_add_tests(TARGET graphseg_test)