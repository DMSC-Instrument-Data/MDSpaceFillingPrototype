#include <benchmark/benchmark.h>

#include <boost/sort/sort.hpp>
#include <tbb/parallel_sort.h>

#include "SortBenchmarkCommon.h"

#include <fstream>

int main(int argc, char **argv) {
  benchmark::Initialize(&argc, argv);

  /* const size_t numElements(5400000000000); */
  const size_t numElements(500000);

  std::vector<uint64_t> multiplePeaks;
  {
    std::vector<std::pair<float, float>> peaks = {
        {0.1f, 0.001f}, {0.2f, 0.001f}, {0.3f, 0.001f},
        {0.4f, 0.001f}, {0.5f, 0.001f}, {0.6f, 0.001f},
        {0.7f, 0.001f}, {0.8f, 0.001f}, {0.9f, 0.001f}};
    GenerateRandomUniformDatasetWithPeaks(multiplePeaks, numElements, peaks);
  }

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiplePeaks,
                          boost::sort::block_indirect_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiplePeaks,
                          boost::sort::sample_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiplePeaks,
                          tbb::parallel_sort);

  benchmark::RunSpecifiedBenchmarks();
}
