cmake_minimum_required(VERSION 3.12)
project(BayesianNetwork)

set(CMAKE_CXX_STANDARD 14)

# If CMake is unable to find Boost,
# you need to specify the BOOST_ROOT variable
# pointing to the directory which contains Boost.

# I put Boost in "D:/SDK".
set(BOOST_ROOT D:/SDK)
find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SRC src/*.cpp)
file(GLOB HEAD include/*.h)

add_executable(BayesianNetwork ${SRC} ${HEAD})
