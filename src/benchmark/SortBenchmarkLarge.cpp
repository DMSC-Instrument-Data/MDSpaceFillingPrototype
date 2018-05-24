#include <benchmark/benchmark.h>

#include <boost/sort/sort.hpp>
#include <tbb/parallel_sort.h>

#include "SortBenchmarkCommon.h"

#include <fstream>

int main(int argc, char **argv) {
  benchmark::Initialize(&argc, argv);

  /* const size_t numElements(5400000000000); */
  const size_t numElements(10000000);

  GENERATE_DATASET(multiplePeaks, uint64_t,
                   GenerateRandomPeakDataset_ClusteredPeaks, numElements);

  /* SaveDataset("multiplePeaks.txt", multiplePeaks); */

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiplePeaks,
                          boost::sort::block_indirect_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiplePeaks,
                          boost::sort::sample_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiplePeaks,
                          tbb::parallel_sort);

  benchmark::RunSpecifiedBenchmarks();
}
