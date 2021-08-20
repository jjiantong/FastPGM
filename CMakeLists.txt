cmake_minimum_required(VERSION 3.10)
project(BayesianNetwork)

add_definitions("-Wall -g")

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/ARFF/src)
include_directories(${PROJECT_SOURCE_DIR}/lib/stats/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/gcem/include)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)

file(GLOB SRC src/*.cpp)
file(GLOB HEAD include/*.h)

add_executable(BayesianNetwork ${SRC} ${HEAD})
target_link_libraries(BayesianNetwork tinyxml2 ARFF stats gcem)
