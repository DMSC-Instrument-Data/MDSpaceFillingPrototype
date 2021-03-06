cmake_minimum_required(VERSION 3.5)

project(md_workspace_structure)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS)

find_package(Threads REQUIRED)
include(OpenMP)

enable_testing()

# Workaround new default flags (emit RUNPATH instead of RPATH) and issues with
# finding shared libraries in RUNPATH
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")

include(targets)
add_subdirectory(src)
