# cmake_minimum_required(VERSION <specify CMake version here>)
project(BayesianNetwork)

set(CMAKE_CXX_STANDARD 14)

set(Boost_INCLUDE_DIR D:/SDK/boost_1_68_0)
set(Boost_LIBRARY_DIR D:/SDK/boost_1_68_0/libs)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_executable(BayesianNetwork main.cpp Network.cpp Network.h Node.cpp Node.h Edge.cpp Edge.h Factor.cpp Factor.h Trainer.cpp Trainer.h SomeTest.h gadget.cpp gadget.h)