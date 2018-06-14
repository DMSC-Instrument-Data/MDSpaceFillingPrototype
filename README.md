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

Building requires CMake and [Conan](https://conan.io/).

1. `mkdir build`
2. `cd build`
3. `./conan_setup.sh /path/to/src/dir/conanfile.txt [Release/Debug]`
4. `cmake /path/to/src/dir -DCMAKE_BUILD_TYPE=[Release/Debug]`
5. `make`
