cmake_minimum_required(VERSION 3.12)
project(BayesianNetwork)

set(CMAKE_CXX_STANDARD 14)

#This way of setting path is not so good, since it will only work in Windows.
#set(Boost_INCLUDE_DIR D:/SDK/boost_1_68_0)
#set(Boost_LIBRARY_DIR D:/SDK/boost_1_68_0/libs)
FIND_PACKAGE(Boost)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SRC src/*.cpp)
file(GLOB HEAD include/*.h)

add_executable(BayesianNetwork ${SRC} ${HEAD})