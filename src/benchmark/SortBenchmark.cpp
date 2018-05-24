#include <benchmark/benchmark.h>

#include <boost/sort/sort.hpp>
#include <tbb/parallel_sort.h>

#include "SortBenchmarkCommon.h"

int main(int argc, char **argv) {
  benchmark::Initialize(&argc, argv);

  std::vector<uint64_t> uniform_1000000;
  GenerateRandomUniformDataset(uniform_1000000, 1000000);

  std::vector<uint64_t> multiple_peaks_1000000;
  {
    std::vector<std::pair<float, float>> peaks = {
        {0.1f, 0.001f}, {0.2f, 0.001f}, {0.3f, 0.001f},
        {0.4f, 0.001f}, {0.5f, 0.001f}, {0.6f, 0.001f},
        {0.7f, 0.001f}, {0.8f, 0.001f}, {0.9f, 0.001f}};
    GenerateRandomUniformDatasetWithPeaks(multiple_peaks_1000000, 1000000,
                                          peaks);
  }

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform_1000000, std::sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks_1000000,
                          std::sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform_1000000,
                          boost::sort::spreadsort::spreadsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks_1000000,
                          boost::sort::spreadsort::spreadsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform_1000000,
                          boost::sort::pdqsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks_1000000,
                          boost::sort::pdqsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform_1000000,
                          boost::sort::block_indirect_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks_1000000,
                          boost::sort::block_indirect_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform_1000000,
                          boost::sort::sample_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks_1000000,
                          boost::sort::sample_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform_1000000,
                          boost::sort::parallel_stable_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks_1000000,
                          boost::sort::parallel_stable_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform_1000000,
                          tbb::parallel_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks_1000000,
                          tbb::parallel_sort);

  benchmark::RunSpecifiedBenchmarks();
}
