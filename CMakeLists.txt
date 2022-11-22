cmake_minimum_required(VERSION 3.8)
project(MaximaFinderC)

set(CMAKE_CXX_STANDARD 14)

add_executable(${PROJECT_NAME} cfindmaxima.cpp)

# since pybind11 is header-only
include_directories(extern/pybind11/include)

add_subdirectory(extern/pybind11)
pybind11_add_module(cfindmaxima ./pybind11_call/cfindmaxima.cpp)