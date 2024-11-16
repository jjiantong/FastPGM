cmake_minimum_required(VERSION 3.27)
project(BayesianNetwork)

add_definitions("-Wall -g")

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp") #todo: for ubuntu and other cases

option(USE_MPI "option for mpi" OFF)
if (USE_MPI)
    message("Use MPI")
    add_definitions(-DUSE_MPI)
    find_package(MPI REQUIRED)
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
endif ()

if(APPLE)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(OpenMP_C "${CMAKE_C_COMPILER}" CACHE STRING "" FORCE)
        set(OpenMP_C_FLAGS "-fopenmp -Wno-unused-command-line-argument" CACHE STRING "" FORCE)
        set(OpenMP_C_LIB_NAMES "omp" CACHE STRING "" FORCE)
        set(OpenMP_omp_LIBRARY "/usr/local/opt/libomp/lib/libomp.dylib" CACHE STRING "" FORCE)
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(OpenMP_CXX "${CMAKE_CXX_COMPILER}" CACHE STRING "" FORCE)
        set(OpenMP_CXX_FLAGS "-fopenmp -Wno-unused-command-line-argument" CACHE STRING "" FORCE)
        set(OpenMP_CXX_LIB_NAMES "omp" CACHE STRING "" FORCE)
        set(OpenMP_omp_LIBRARY "/usr/local/opt/libomp/lib/libomp.dylib" CACHE STRING "" FORCE)
    endif()

    # FIND OPENMP
    find_package(OpenMP REQUIRED)
    if(OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}
         ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

include_directories(${PROJECT_SOURCE_DIR}/lib/tinyxml2)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/ARFF/src)
include_directories(${PROJECT_SOURCE_DIR}/lib/stats/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/gcem/include)

add_subdirectory(lib)
add_subdirectory(src/fastbn)
add_subdirectory(src/test)

file(GLOB_RECURSE SRC src/fastbn/*.cpp)
file(GLOB HEAD include/*.h)

#add_library(fastbn SHARED ${SRC})

add_executable(BayesianNetwork ${SRC} ${HEAD})
target_link_libraries(BayesianNetwork tinyxml2 ARFF stats gcem)
target_link_libraries(BayesianNetwork ${MPI_C_LIBRARIES})
