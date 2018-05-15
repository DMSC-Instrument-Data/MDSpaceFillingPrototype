#include <benchmark/benchmark.h>

#include <boost/sort/sort.hpp>

#include "SortBenchmarkCommon.h"

int main(int argc, char **argv) {
  GENERATE_DATASET(single_peak_5400000000000, uint64_t,
                   GenerateRandomUniformDatasetWithSinglePeak, 5400000000000);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, single_peak_5400000000000,
                          boost::sort::block_indirect_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, single_peak_5400000000000,
                          boost::sort::sample_sort);

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}
