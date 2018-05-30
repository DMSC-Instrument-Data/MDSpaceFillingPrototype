# Coordinate conversion/mapping

This is a fairly trivial problem of mapping coordinates from floating point
values to a fixed width, unsigned integer range.

## Minimum integer bit width

Assume we have the upper and lower bounds of the MD space and the minimum step
in each axis (i.e. the resolution).

Using this we can calculate the integer width required to represent the floating
point coordinates in a lossless manner.

For a single point `x` on axis `X`, the number of unique measurable values `Nx`
and the number of bits required to represent this `Bx` are given by:

```
Nx = (Xmax - Xmin) / Xstep
Bx = ceil(log2(Nx))
```

As interleaving requires that the bit width of the input integers are identical
the number of bits required is calculated for each axis and the highest value
multiplied by the number of dimensions to obtain the total number of bits
required for the interleaved integer.

For example, for two dimensions with given bounds and resolutions the number of
bits required for the intermediate integer representation `Bi` and interleaved
Morton number `Bm` are calculated as follows:

```
Xmin = 1.0  Xmax = 8.0  Xstep = 0.02
Ymin = 5.0  Ymax = 6.0  Ystep = 0.01

Nx = (Xmax - Xmin) / Xstep
   = (8.0  - 1.0 ) / 0.02
   = (7.0        ) / 0.02
   = 350
Bx = ceil(log2(Nx))
   = ceil(log2(350))
   = ceil(8.45)
   = 9

Ny = (Ymax - Ymin) / Ystep
   = (6.0  - 5.0 ) / 0.01
   = (1.0        ) / 0.01
   = 100
By = ceil(log2(Ny))
   = ceil(log2(100))
   = ceil(6.64)
   = 7

Bi = max(Bx, By)
   = max(9 , 7 )
   = 9

Bm = Bi * N
   = 9  * 2
   = 18
```

## Mapping to integer range

The mapping of floating point coordinates to an integer range is a relatively
trivial operation; a linear scaling from the floating point axis range to
full unsigned integer range (for the given integer width).

For each axis coordinate `a` a fraction of the position of the point along the
axis is taken:

```
Fa = (a - Amin) / (Amax - Amin)
```

The fraction is then multiplied by the maximum value for the given integer width
and cast to the given integer type:

```
Ia = cast_to_I(Fa * max(I))
```

This method is prone to integer overflow when the floating point coordinate is
very close to the upper bound of the axis, this problem is resolved by expanding
the coordinate range by a sufficient amount to guarantee that no inter overflow
can occur.

The implementation of this uses
[`std::nexttoward`](https://en.cppreference.com/w/cpp/numeric/math/nextafter) to
expand the ranges to the next representable floating point value at either side
of the range.
