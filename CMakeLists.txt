cmake_minimum_required(VERSION 3.5)

project(md_workspace_structure)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
set(MANTID_PATH "/home/igudich/work/mantid")

#Includes for mantid types
include_directories("${MANTID_PATH}/Framework/Types/inc")
include_directories("${MANTID_PATH}/python3Build/Framework/Types")
include_directories("${MANTID_PATH}/Framework/DataHandling/inc")
include_directories("${MANTID_PATH}/Framework/API/inc")
include_directories("${MANTID_PATH}/Framework/Kernel/inc")
include_directories("${MANTID_PATH}/Framework/Parallel/inc")
include_directories("${MANTID_PATH}/Framework/Geometry/inc")
include_directories("${MANTID_PATH}/Framework/HistogramData/inc")
include_directories("${MANTID_PATH}/Framework/DataObjects/inc")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS)

find_package(Threads REQUIRED)
include(OpenMP)

enable_testing()

# Workaround new default flags (emil RUNPATH instead of RPATH) and issues with
# finding shared libraries in RUNPATH
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")

include(targets)
add_subdirectory(src)
