#include <benchmark/benchmark.h>

#include <boost/sort/sort.hpp>
#include <tbb/parallel_sort.h>

#include "SortBenchmarkCommon.h"

int main(int argc, char **argv) {
  benchmark::Initialize(&argc, argv);

  /* const size_t numElements(5400000000000); */
  const size_t numElements(50000000);

  GENERATE_DATASET(dataset, uint64_t,
                   GenerateRandomUniformDatasetWithSinglePeak, numElements);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, dataset,
                          boost::sort::block_indirect_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, dataset,
                          boost::sort::sample_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, dataset, tbb::parallel_sort);

  benchmark::RunSpecifiedBenchmarks();
}
