# cmake_minimum_required(VERSION <specify CMake version here>)
project(BayesianNetwork)

set(CMAKE_CXX_STANDARD 14)

set(Boost_INCLUDE_DIR D:/SDK/boost_1_68_0)
set(Boost_LIBRARY_DIR D:/SDK/boost_1_68_0/libs)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SRC src/*.cpp)
file(GLOB HEAD include/*.h)

add_executable(BayesianNetwork ${SRC} ${HEAD})