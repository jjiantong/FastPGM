cmake_minimum_required(VERSION 3.10)
project(BayesianNetwork)

set(CMAKE_CXX_STANDARD 14)

#include_directories(${PROJECT_SOURCE_DIR}/lib/googletest/googlemock/include)
#include_directories(${PROJECT_SOURCE_DIR}/lib/googletest/googletest/include)

file(GLOB TEST ./*.cpp)

add_executable(BayesianNetwork_test ${TEST})

target_link_libraries(BayesianNetwork_test lib_src)
target_link_libraries(BayesianNetwork_test gtest gtest_main)
target_link_libraries(BayesianNetwork_test tinyxml2)
target_link_libraries(BayesianNetwork_test ARFF)
