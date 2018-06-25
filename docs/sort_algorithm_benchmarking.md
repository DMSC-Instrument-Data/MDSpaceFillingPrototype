# Sort Algorithm Benchmarking

## Algorithm selection

Currently only "off the shelf" library implementations have been considered,
they include:

- Single threaded
  - `std::sort`
  - `boost::sort::spreadsort::spreadsort`
  - `boost::sort::pdqsort`
- Parallel
  - `boost::sort::block_indirect_sort`
  - `boost::sort::sample_sort`
  - `boost::sort::parallel_stable_sort`
  - `tbb::parallel_sort`

## Dataset generation

Currently benchmarks run against two datasets; one with a uniform distribution
(generated using `std::uniform_int_distribution`), the other with several peaks
generated using `std::cauchy_distribution`.

## Benchmark results

All benchmarks were run on a workstation with two Intel(R) Xeon(R) Gold 6134
(32 threads total, max 3.7GHz) and 128GB memory.

### Small dataset

Benchmarks run on datasets of 10000000 elements for both distributions
(source: `SortBenchmark.cpp`).

#### Results

Command:
```bash
./src/benchmark/SortBenchmark --benchmark_min_time=1
```

Output:
```
Generating dataset uniform
Generating dataset multiple_peaks
2018-06-12 13:41:13
Running ./src/benchmark/SortBenchmark
Run on (32 X 3700 MHz CPU s)
CPU Caches:
  L1 Data 32K (x16)
  L1 Instruction 32K (x16)
  L2 Unified 1024K (x16)
  L3 Unified 25344K (x2)
---------------------------------------------------------------------------------------------------------------
Benchmark                                                                        Time           CPU Iterations
---------------------------------------------------------------------------------------------------------------
uint64_t std::vector std::sort uniform                                         753 ms        753 ms          2
uint64_t std::vector std::sort multiple_peaks                                  743 ms        743 ms          2
uint64_t std::vector boost::sort::spreadsort::spreadsort uniform               631 ms        631 ms          2
uint64_t std::vector boost::sort::spreadsort::spreadsort multiple_peaks        538 ms        538 ms          3
uint64_t std::vector boost::sort::pdqsort uniform                              349 ms        349 ms          4
uint64_t std::vector boost::sort::pdqsort multiple_peaks                       362 ms        362 ms          4
uint64_t std::vector boost::sort::block_indirect_sort uniform                   44 ms          1 ms       1000
uint64_t std::vector boost::sort::block_indirect_sort multiple_peaks            43 ms          1 ms       1273
uint64_t std::vector boost::sort::sample_sort uniform                           80 ms         14 ms        103
uint64_t std::vector boost::sort::sample_sort multiple_peaks                    80 ms         14 ms        102
uint64_t std::vector boost::sort::parallel_stable_sort uniform                 124 ms         64 ms         22
uint64_t std::vector boost::sort::parallel_stable_sort multiple_peaks          120 ms         61 ms         23
uint64_t std::vector tbb::parallel_sort uniform                                108 ms        108 ms         13
uint64_t std::vector tbb::parallel_sort multiple_peaks                          75 ms         75 ms         19
```

### Large dataset

This benchmark is only run on `boost::sort::block_indirect_sort`,
`boost::sort::sample_sort` and `tbb::parallel_sort` as they are the highest
performing algorithms in the smaller scale benchmark.

This tests scalability of a selection of highest performing sort algorithms with
a dataset similar in size (in terms of event count) to an expected dataset from
an ESS instrument.

In this case the dataset consisted of 10^10 events (in the same order of
magnitude as a run of 1 hour at 10^7 events per second).

The data uses the distribution with multiple peaks, is pre-generated using the
`DatasetGenerator` executable and loaded via HDF5 file.

#### Results

Command:
```bash
./src/exe/DatasetGenerator -chunks 1000 -chunkSize 10000000
./src/benchmark/SortDatasetBenchmark --benchmark_min_time=600
```

Output:
```
Allocating vector of size 10000000000
2018-06-12 10:26:44
Running ./src/benchmark/SortDatasetBenchmark
Run on (32 X 3700 MHz CPU s)
CPU Caches:
  L1 Data 32K (x16)
  L1 Instruction 32K (x16)
  L2 Unified 1024K (x16)
  L3 Unified 25344K (x2)
----------------------------------------------------------------------------------
Benchmark                                           Time           CPU Iterations
----------------------------------------------------------------------------------
boost::sort::block_indirect_sort/real_time      89666 ms         44 ms          9
boost::sort::sample_sort/real_time             205949 ms       4049 ms          4
tbb::parallel_sort/real_time                   110945 ms     110911 ms          8
```

With the full size dataset the memory usage of `boost::sort::sample_sort`
exceeded the amount of physical memory available on the workstation, causing it
to use disk swap space. Both `boost::sort::block_indirect_sort` and
`tbb::parallel_sort` used only the memory required to hold the dataset.

To provide a fair performance comparison the following additional test was
performed with 7.5^10 events.

Command:
```bash
./src/exe/DatasetGenerator -chunks 750 -chunkSize 10000000
./src/benchmark/SortDatasetBenchmark --benchmark_min_time=600
```

Output:
```
Allocating vector of size 7500000000
2018-06-14 12:05:27
Running ./src/benchmark/SortDatasetBenchmark
Run on (32 X 3700 MHz CPU s)
CPU Caches:
  L1 Data 32K (x16)
  L1 Instruction 32K (x16)
  L2 Unified 1024K (x16)
  L3 Unified 25344K (x2)
----------------------------------------------------------------------------------
Benchmark                                           Time           CPU Iterations
----------------------------------------------------------------------------------
boost::sort::block_indirect_sort/real_time      43526 ms          7 ms         18
boost::sort::sample_sort/real_time              50224 ms       3111 ms         16
tbb::parallel_sort/real_time                    73433 ms      73411 ms         14
```
