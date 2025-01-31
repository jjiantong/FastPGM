cmake_minimum_required(VERSION 3.17)
project(fastpgm)

add_definitions("-Wall -g")

set(CMAKE_CXX_STANDARD 14)

option(USE_MPI "option for mpi" OFF)
if (USE_MPI)
    message("Use MPI")
    add_definitions(-DUSE_MPI)
    find_package(MPI REQUIRED)
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
endif ()

if(APPLE)
    # FIND OPENMP
    find_package(OpenMP REQUIRED)
    if(OPENMP_FOUND)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

include_directories(${PROJECT_SOURCE_DIR}/lib/tinyxml2)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/ARFF/src)
include_directories(${PROJECT_SOURCE_DIR}/lib/stats/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/gcem/include)

add_subdirectory(lib)
add_subdirectory(src/fastpgm)
add_subdirectory(src/test)

file(GLOB_RECURSE SRC src/fastpgm/*.cpp)
file(GLOB HEAD include/*.h)

add_executable(fastpgm ${SRC} ${HEAD})
target_link_libraries(fastpgm tinyxml2 ARFF stats gcem)
target_link_libraries(fastpgm ${MPI_C_LIBRARIES})
