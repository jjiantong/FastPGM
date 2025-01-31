cmake_minimum_required(VERSION 3.22)
project(cmake_example)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

# Manually set FASTPGM_DIR as the absolute path of FastPGM
set(FASTPGM_DIR "/Users/jjt/my_doc/work/research_project/BN/FastPGM")
#set(FASTPGM_DIR "/home/parallels/work/FastPGM")
#set(FASTPGM_DIR "/home/jjt/work/BN/FastBN")

include_directories(${FASTPGM_DIR}/include)
include_directories(${FASTPGM_DIR}/lib/tinyxml2)
include_directories(${FASTPGM_DIR}/lib/ARFF/src)
include_directories(${FASTPGM_DIR}/lib/stats/include)
include_directories(${FASTPGM_DIR}/lib/gcem/include)

add_subdirectory(pybind11)
file(GLOB_RECURSE FASTPGM_SRC ${FASTPGM_DIR}/src/fastpgm/*.cpp)
pybind11_add_module(cmake_example api_main.cpp
        ${FASTPGM_SRC} ${FASTPGM_DIR}/lib/tinyxml2/tinyxml2.cpp)

#target_link_libraries(cmake_example PRIVATE tinyxml2)

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(cmake_example
        PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
