cmake_minimum_required(VERSION 3.10)
project(BayesianNetwork)

add_definitions("-Wall -g")

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

option(USE_MPI "option for mpi" OFF)
if (USE_MPI)
    message("Use MPI")
    add_definitions(-DUSE_MPI)
    find_package(MPI REQUIRED)
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
endif ()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/ARFF/src)
include_directories(${PROJECT_SOURCE_DIR}/lib/stats/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/gcem/include)

add_subdirectory(lib)
add_subdirectory(src/fastbo)
add_subdirectory(src/test)

file(GLOB_RECURSE SRC src/fastbo/*.cpp)
file(GLOB HEAD include/*.h)

add_executable(BayesianNetwork ${SRC} ${HEAD})
target_link_libraries(BayesianNetwork tinyxml2 ARFF stats gcem)
target_link_libraries(BayesianNetwork ${MPI_C_LIBRARIES})
