# MD Space Filling Prototype [![Build Status](https://travis-ci.org/DMSC-Instrument-Data/MDSpaceFillingPrototype.svg?branch=master)](https://travis-ci.org/DMSC-Instrument-Data/MDSpaceFillingPrototype)

This investigation aims to evaluate the use of space filling curves as an
alternative method of underlying storage for *MDEventWorkspaces* within Mantid.
Currently this is done using a recursive box structure already described
[here](https://github.com/DMSC-Instrument-Data/documents/blob/master/investigations/MultiDimensionalInvestigation/md_data_structures/md_event.md).

## Proposed alternative

In the space filling curve approach all events are stored in a single container
which is sorted in the order of the individual events position on the curve.

This can be done via a conversion of floating point coordinates to interleaved
integers (aka Morton number) then sorting the container using this number as the
sorting key. The result of this is the event list sorted in
[Z-order](https://en.wikipedia.org/wiki/Z-order_curve).

The box structure can then be recreated from the sorted event list. Each `MDBox`
would contain iterators to the start and end of the events that were contained
within it in the container of all events.

## Build instructions

Building requires GCC 5 or greater, CMake, [Conan](https://conan.io/). Linux is
the only tested OS.

1. `mkdir build`
2. `cd build`
3. `conan remote add ess-dmsc https://api.bintray.com/conan/ess-dmsc/conan`
4. `conan install /path/to/src/dir/conanfile.txt -s build_type=[Release/Debug] -s compiler.libcxx=libstdc++11 --build missing`
5. `cmake /path/to/src/dir -DCMAKE_BUILD_TYPE=[Release/Debug]`
6. `make`

To run unit tests:
```bash
ctest -L UnitTest
```

To run benchmarks (that are not dependant on data files):
```bash
ctest -V -L Benchmark -LE Data
```
