cmake_minimum_required(VERSION 3.5)

project(md_workspace_structure)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(${CMAKE_SOURCE_DIR}/CMake/conan.cmake)

conan_cmake_run(REQUIRES
  boost/1.67.0@conan/stable
  eigen/3.3.4@conan/stable
  gtest/1.8.0@bincrafters/stable
  BASIC_SETUP NO_OUTPUT_DIRS
  BUILD missing)

enable_testing()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(GBenchmark REQUIRED)

add_subdirectory(src)
