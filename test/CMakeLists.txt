cmake_minimum_required(VERSION 3.12)
project(BayesianNetwork)

set(CMAKE_CXX_STANDARD 14)

file(GLOB TEST ./*.cpp)

add_executable(test_exe ${TEST} test_main.cpp test_scoring_function.cpp)

target_link_libraries(test_exe lib_src)
target_link_libraries(test_exe gtest gtest_main)
target_link_libraries(test_exe tinyxml2)