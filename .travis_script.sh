#!/bin/sh

conan remote add ess-dmsc https://api.bintray.com/conan/ess-dmsc/conan

mkdir -p build
cd build

conan install ${HOME}/src/conanfile.txt -s build_type=Release -s compiler.libcxx=libstdc++11 --build missing
cmake -DCMAKE_BUILD_TYPE=Release ${HOME}/src
make -j4

ctest -V -L UnitTest
