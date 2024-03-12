cmake_minimum_required(VERSION 3.10)

# Scikit-build-core sets these values for you, or you can just hard-code the
# name and version.
project(
  ${SKBUILD_PROJECT_NAME}
  VERSION ${SKBUILD_PROJECT_VERSION}
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

# Find the module development requirements (requires FindPython from 3.17 or
# scikit-build-core's built-in backport)
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

# Add a library using FindPython's tooling (pybind11 also provides a helper like
# this)
python_add_library(_core MODULE src/python_interface.cpp WITH_SOABI)
target_include_directories(_core PRIVATE ${pybind11_INCLUDE_DIRS})
target_link_libraries(_core PRIVATE pybind11::headers)

file(GLOB SRC ./src/*.cpp)

add_library(lib_src ${SRC})
target_include_directories(lib_src PRIVATE ${pybind11_INCLUDE_DIRS})
target_link_libraries(lib_src PRIVATE)

# This is passing in the version as a define just as an example
target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

# The install directory is the output (wheel) directory
install(TARGETS _core DESTINATION scikit_build_example)
