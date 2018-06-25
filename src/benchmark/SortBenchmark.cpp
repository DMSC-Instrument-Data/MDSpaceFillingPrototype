#include <benchmark/benchmark.h>

#include <boost/sort/sort.hpp>
#include <tbb/parallel_sort.h>

#include "DistributionGenerator.h"

#define REGISTER_SORT_BENCHMARK(type, container, data, sort)                   \
  benchmark::RegisterBenchmark(#type " " #container " " #sort " " #data,       \
                               [=](benchmark::State & state) {                 \
    for (auto _ : state) {                                                     \
      state.PauseTiming();                                                     \
      container<type> v;                                                       \
      std::copy(data.begin(), data.end(), std::back_inserter(v));              \
      state.ResumeTiming();                                                    \
      sort(v.begin(), v.end());                                                \
    }                                                                          \
                               })->Unit(benchmark::kMillisecond);

int main(int argc, char **argv) {
  benchmark::Initialize(&argc, argv);

  GENERATE_DATASET(uniform, uint64_t, GenerateRandomUniformDataset, 10000000);

  GENERATE_DATASET(multiple_peaks, uint64_t,
                   GenerateRandomPeakDataset_ClusteredPeaks, 10000000);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform, std::sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks, std::sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform,
                          boost::sort::spreadsort::spreadsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          boost::sort::spreadsort::spreadsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform, boost::sort::pdqsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          boost::sort::pdqsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform,
                          boost::sort::block_indirect_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          boost::sort::block_indirect_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform,
                          boost::sort::sample_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          boost::sort::sample_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform,
                          boost::sort::parallel_stable_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          boost::sort::parallel_stable_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform, tbb::parallel_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          tbb::parallel_sort);

  benchmark::RunSpecifiedBenchmarks();
}
