#!/bin/bash

# Usage: ./conan_setup.sh [path/to/conanfile.txt] [Release/Debug]

conan install $1 \
  -s build_type=$2 \
  -s compiler.libcxx=libstdc++11 \
  --build gtest
