# Event access

Methods for accessing the created data structure are described here.

The two main methods are; cases where the coordinates of individual events are
important (such as the splatter plot view in the VSI) and cases where the
integrated signal of an area is desired (such as the BinMD algorithm in Mantid).

## Single event/coordinate retrieval

Retrieving the original floating point coordinates of an event in MD space is
essentially the opposite of the procedure described in
`coordinate_conversion.md`.

Given the original MD extents used to create the MD event list the floating
point coordinates can be retrieved via the `MDEvent::coordinates()` function.

This operation has been benchmarked below.

### Benchmark

This benchmark creates a random event list and performs the coordinate
retrieval operation on every event (for a varied integer bit width and event
count).

Note that the distribution of events is irrelevant for this benchmark as the
retrieval operation is constant time and unrelated to the relative location of
the event in MD space.

All event access operations were performed on a single thread.

Command:
```bash
./src/benchmark/EventAccessBenchmark --benchmark_min_time=60 --benchmark_filter=AccessCoordinates
```

Output:
```
2018-09-25 09:58:07
Running ./src/benchmark/EventAccessBenchmark
Run on (32 X 3700 MHz CPU s)
CPU Caches:
  L1 Data 32K (x16)
  L1 Instruction 32K (x16)
  L2 Unified 1024K (x16)
  L3 Unified 25344K (x2)
---------------------------------------------------------------------------------------------------
Benchmark                                                            Time           CPU Iterations
---------------------------------------------------------------------------------------------------
BM_AccessCoordinates<uint16_t, uint64_t>/1000000000              11276 ms      11275 ms          8
BM_AccessCoordinates<uint16_t, uint64_t>/469388241                4585 ms       4585 ms         19
BM_AccessCoordinates<uint16_t, uint64_t>/100000000                 939 ms        939 ms         89
BM_AccessCoordinates<uint32_t, uint128_t>/1000000000             40836 ms      40835 ms          2
BM_AccessCoordinates<uint32_t, uint128_t>/469388241              17337 ms      17337 ms          5
BM_AccessCoordinates<uint32_t, uint128_t>/100000000               3791 ms       3788 ms         22
```

This shows roughly, the expected linear increase in time with the number of
events used in the test event list.

## Binning

This follows the same implementation used by `BinMD` in Mantid. Due to the
majority of the implementation being identical only the implementation details
that differ have been prototyped and benchmarked.

`BinMD` operates by defining a transformation that given a coordinate in MD
space, produces a bin index in each axis. This is achieved using a relatively
simple 3D transformation (which also allows scaling and rotations for changing
basis).

With simple modifications it is possible to have this transformation take the
intermediate integer coordinates and avoid having to convert back to floating
point. The coordinates must still be deinterleaved first.

The transformation `T` to bin from intermediate integer is defined using the
transformation to perform binning (`Tb`) and the transformation to convert from
intermediate integer to floating point (`Ti`): `T = Ti * Tb`.

Given that the transformation can be precomputed for many events the only
additional cost per event is deinterleaving (to obtain intermediate integer
coordinates from the Morton number representation).

### Benchmark: Existing Implementation

This benchmark sets a baseline for the performance of the portion of `BinMD`
that would have to be replaced to make use of the Morton number coordinate
representation. This is simply applying the binning transformation to a set of
floating point coordinates.

Command:
```bash
./src/benchmark/EventAccessBenchmark --benchmark_min_time=60 --benchmark_filter=_BinMD
```

Output:
```
2018-09-25 13:57:20
Running ./src/benchmark/EventAccessBenchmark
Run on (32 X 3700 MHz CPU s)
CPU Caches:
  L1 Data 32K (x16)
  L1 Instruction 32K (x16)
  L2 Unified 1024K (x16)
  L3 Unified 25344K (x2)
---------------------------------------------------------------------------------
Benchmark                                          Time           CPU Iterations
---------------------------------------------------------------------------------
BM_GenerateBinningTransform_BinMD                 15 ns         15 ns 1000000000
BM_ApplyBinningTransform_BinMD/1000000000       3005 ms       3005 ms         28
BM_ApplyBinningTransform_BinMD/469388241        1419 ms       1419 ms         60
BM_ApplyBinningTransform_BinMD/100000000         302 ms        302 ms        279
```

### Benchmark: Prototype

This set of benchmarks show the time taken to calculate the transformation used
to convert integer coordinates to bin indices for 3 and 4 dimensions with 16 and
32 bit integers as well as the time taken to apply them for various event counts
(of course the time taken is linear with the event count, this simply provides
easier comparison for certain datasets).

Command:
```bash
./src/benchmark/EventAccessBenchmark --benchmark_min_time=60 --benchmark_filter=_Prototype
```

Output:
```
2018-09-25 09:58:07
Running ./src/benchmark/EventAccessBenchmark
Run on (32 X 3700 MHz CPU s)
CPU Caches:
  L1 Data 32K (x16)
  L1 Instruction 32K (x16)
  L2 Unified 1024K (x16)
  L3 Unified 25344K (x2)
---------------------------------------------------------------------------------------------------
Benchmark                                                            Time           CPU Iterations
---------------------------------------------------------------------------------------------------
BM_GenerateBinningTransform<3, uint16_t, uint64_t>                  15 ns         15 ns 1000000000
BM_GenerateBinningTransform<3, uint32_t, uint128_t>                 15 ns         15 ns 1000000000
BM_GenerateBinningTransform<4, uint16_t, uint64_t>                 116 ns        116 ns  727022006
BM_GenerateBinningTransform<4, uint32_t, uint128_t>                104 ns        104 ns  807302263
BM_ApplyBinningTransform<3, uint16_t, uint64_t>/1000000000        3180 ms       3179 ms         27
BM_ApplyBinningTransform<3, uint16_t, uint64_t>/469388241         1536 ms       1536 ms         56
BM_ApplyBinningTransform<3, uint16_t, uint64_t>/100000000          327 ms        327 ms        259
BM_ApplyBinningTransform<3, uint32_t, uint128_t>/1000000000       3314 ms       3311 ms         26
BM_ApplyBinningTransform<3, uint32_t, uint128_t>/469388241        1536 ms       1536 ms         55
BM_ApplyBinningTransform<3, uint32_t, uint128_t>/100000000         330 ms        330 ms        263
BM_ApplyBinningTransform<4, uint16_t, uint64_t>/1000000000        5314 ms       5314 ms         16
BM_ApplyBinningTransform<4, uint16_t, uint64_t>/469388241         2524 ms       2522 ms         34
BM_ApplyBinningTransform<4, uint16_t, uint64_t>/100000000          536 ms        536 ms        158
BM_ApplyBinningTransform<4, uint32_t, uint128_t>/1000000000       5339 ms       5338 ms         16
BM_ApplyBinningTransform<4, uint32_t, uint128_t>/469388241        2539 ms       2537 ms         33
BM_ApplyBinningTransform<4, uint32_t, uint128_t>/100000000         532 ms        532 ms        158
```

### BinMD equivalent benchmark

This benchmark shows the time taken for `BinMD` to bin the test datasets with
various numbers of bins (bin count was included when comparing a previous
approach taken in the prototype, it makes no significant difference to execution
time).

Command:
```bash
mantidpython scripts/mantid_benchmark_binmd.py
```

Output:
```
Data file: /data/WISH00034509.nxs
Bins per axis: 100
Iteration count:  15
Total time:  123.953759193
Average time:  8.26358394623
Bins per axis: 300
Iteration count:  16
Total time:  125.249131203
Average time:  7.82807070017
Bins per axis: 1000
Iteration count:  16
Total time:  125.583920956
Average time:  7.84899505973

Data file: /data/WISH00034509_2x_larger.nxs
Bins per axis: 100
Iteration count:  8
Total time:  128.585271835
Average time:  16.0731589794
Bins per axis: 300
Iteration count:  8
Total time:  126.137060165
Average time:  15.7671325207
Bins per axis: 1000
Iteration count:  8
Total time:  129.10214901
Average time:  16.1377686262

Data file: /data/WISH00037828_event.nxs
Bins per axis: 100
Iteration count:  326
Total time:  120.13397935
Average time:  0.368509139111
Bins per axis: 300
Iteration count:  324
Total time:  120.337115318
Average time:  0.371410849746
Bins per axis: 1000
Iteration count:  321
Total time:  120.232893944
Average time:  0.374557302006

Data file: /data/WISH00037868_event.nxs
Bins per axis: 100
Iteration count:  878
Total time:  120.060672253
Average time:  0.136743362475
Bins per axis: 300
Iteration count:  883
Total time:  120.050552063
Average time:  0.135957590105
Bins per axis: 1000
Iteration count:  880
Total time:  120.136512406
Average time:  0.136518764098

Data file: /data/WISH00038423_event.nxs
Bins per axis: 100
Iteration count:  582
Total time:  120.164014474
Average time:  0.206467378821
Bins per axis: 300
Iteration count:  582
Total time:  120.16413027
Average time:  0.206467577784
Bins per axis: 1000
Iteration count:  573
Total time:  120.096700996
Average time:  0.209592846416

Data file: /data/TOPAZ_3132_event.nxs
Bins per axis: 100
Iteration count:  990
Total time:  120.120311588
Average time:  0.121333648069
Bins per axis: 300
Iteration count:  969
Total time:  120.042923905
Average time:  0.123883306403
Bins per axis: 1000
Iteration count:  968
Total time:  120.072323881
Average time:  0.124041656902

Data file: /data/SXD23767_event.nxs
Bins per axis: 100
Iteration count:  1642
Total time:  120.048120469
Average time:  0.0731109138056
Bins per axis: 300
Iteration count:  1633
Total time:  120.065374069
Average time:  0.0735244176785
Bins per axis: 1000
Iteration count:  1630
Total time:  120.012905017
Average time:  0.0736275490899
```
