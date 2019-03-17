cmake_minimum_required(VERSION 3.12)
project(BayesianNetwork)

set(CMAKE_CXX_STANDARD 14)


add_executable(test_exe test_network.cpp test_tinyxml2.cpp)

target_link_libraries(test_exe gtest gtest_main)
target_link_libraries(test_exe tinyxml2)
target_link_libraries(test_exe lib_src)