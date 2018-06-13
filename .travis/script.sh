#!/bin/sh

${CC} --version
${CXX} --version

conan remote add ess-dmsc https://api.bintray.com/conan/ess-dmsc/conan

mkdir build
cd build
a
../conan_setup.sh .. Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4

ctest -V
