#include <benchmark/benchmark.h>

#include <random>

#include <boost/sort/sort.hpp>

#include "MDEvent.h"

void GenerateRandomUniformEvents(std::vector<MDEvent<4>> &d, size_t n) {
  std::mt19937 generator(0);
  std::uniform_int_distribution<uint64_t> dist(
      0, std::numeric_limits<uint64_t>::max());

  for (int i = 0; i < n; i++) {
    MDEvent<4> e(dist(generator));
    d.push_back(e);
  }
}

int main(int argc, char **argv) {
  benchmark::Initialize(&argc, argv);

  benchmark::RegisterBenchmark("sort", [](benchmark::State &state) {
    for (auto _ : state) {
      state.PauseTiming();
      std::vector<MDEvent<4>> v;
      GenerateRandomUniformEvents(v, 100000000);
      state.ResumeTiming();

      boost::sort::block_indirect_sort(v.begin(), v.end());
    }
  })->Unit(benchmark::kMillisecond);

  benchmark::RunSpecifiedBenchmarks();
}
