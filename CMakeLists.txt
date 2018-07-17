cmake_minimum_required(VERSION 3.5)

project(md_workspace_structure)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS)

find_package(Threads REQUIRED)
find_package(GMP REQUIRED)
include(GoogleBenchmark)
include(OpenMP)

enable_testing()

include(targets)
add_subdirectory(src)
