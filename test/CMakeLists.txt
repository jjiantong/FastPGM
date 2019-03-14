cmake_minimum_required(VERSION 3.12)
project(BayesianNetwork)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(googletest-release-1.8.1)
include_directories(googletest-release-1.8.1/googletest/include)

file(GLOB SRC ../*.cpp)

add_executable(test_exe test_main.cpp)

target_link_libraries(test_exe gtest gtest_main)
target_link_libraries(test_exe lib_src)