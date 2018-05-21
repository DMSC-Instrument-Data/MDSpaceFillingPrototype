cmake_minimum_required(VERSION 3.5)

project(md_workspace_structure)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(conan)

conan_cmake_run(
  REQUIRES
    boost/1.67.0@conan/stable
    eigen/3.3.4@conan/stable
    gtest/1.8.0@bincrafters/stable
  BUILD
    gtest
  BASIC_SETUP
  NO_OUTPUT_DIRS)

find_package(Threads REQUIRED)
include(GoogleBenchmark)

enable_testing()

include(targets)
add_subdirectory(src)
