#cmake_minimum_required(VERSION 3.27)
#
## Scikit-build-core sets these values for you, or you can just hard-code the
## name and version.
#project(
#        ${SKBUILD_PROJECT_NAME}
#        VERSION ${SKBUILD_PROJECT_VERSION}
#        LANGUAGES CXX)
#
#set(CMAKE_CXX_STANDARD 14)

# Manually set FASTBN_DIR as the absolute path of FastBN
set(FASTBN_DIR "/home/jjt/work/BN/FastBN")

# Find the module development requirements (requires FindPython from 3.17 or
# scikit-build-core's built-in backport)
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

include_directories(${FASTBN_DIR}/include)

# Add a library using FindPython's tooling (pybind11 also provides a helper like
# this)
python_add_library(_core MODULE src/python_interface.cpp
        ${FASTBN_DIR}/src/fastbn/Parameter.cpp
        WITH_SOABI)
target_link_libraries(_core PRIVATE pybind11::headers)

# This is passing in the version as a define just as an example
target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

# The install directory is the output (wheel) directory
install(TARGETS _core DESTINATION scikit_build_example)
