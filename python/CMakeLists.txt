cmake_minimum_required(VERSION 3.27)
project(cmake_example)

set(CMAKE_CXX_STANDARD 14)

# Manually set FASTBN_DIR as the absolute path of FastBN
set(FASTBN_DIR "/home/jjt/work/BN/FastBN")

include_directories(${FASTBN_DIR}/include)
include_directories(${FASTBN_DIR}/lib/tinyxml2)
include_directories(${FASTBN_DIR}/include)
include_directories(${FASTBN_DIR}/lib/ARFF/src)
include_directories(${FASTBN_DIR}/lib/stats/include)
include_directories(${FASTBN_DIR}/lib/gcem/include)

add_subdirectory(pybind11)
file(GLOB_RECURSE FASTBN_SRC ${FASTBN_DIR}/src/fastbn/*.cpp)
pybind11_add_module(cmake_example python_interface.cpp g${FASTBN_SRC})

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(cmake_example
        PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})