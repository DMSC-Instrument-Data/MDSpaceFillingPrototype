# Overview

TODO: fill this out in more detail later on

## Coordinate spaces

MD events in this prototype have three coordinate spaces:

- MD/floating point (`float`)
- Intermediate integer (commonly templated as `IntT`)
- Morton number (commonly templated as `MortonT`)

The MD (or floating point) coordinates are the result of the conversion from raw
events to the MD space.

Intermediate integer space is the result of mapping the MD space onto an
unsigned integer type, for this the bounds of the MD space must be known before
conversion can take place.

Morton number "space" is the 1D range that makes up the Z-order curve, it is the
result of interleaving the intermediate integers.

It is required that the number of bits that `MortonT` can store is at least the
number of dimensions times the number of bits `IntT` can store.
