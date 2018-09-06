#include <benchmark/benchmark.h>

#include "BitInterleaving.h"

static void BM_Interleave_4_64_256_Boost(benchmark::State &state) {
  IntArray<4, uint64_t> data;
  uint256_t z;
  for (auto _ : state) {
    z = interleave<4, uint64_t, uint256_t>(data);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(z);
  }
}
BENCHMARK(BM_Interleave_4_64_256_Boost);

static void BM_Deinterleave_4_64_256_Boost(benchmark::State &state) {
  IntArray<4, uint64_t> data;
  uint256_t z;
  for (auto _ : state) {
    data = deinterleave<4, uint64_t, uint256_t>(z);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(z);
  }
}
BENCHMARK(BM_Deinterleave_4_64_256_Boost);

static void BM_RoundTrip_4_64_256_Boost(benchmark::State &state) {
  IntArray<4, uint64_t> data;
  uint256_t z;
  for (auto _ : state) {
    z = interleave<4, uint64_t, uint256_t>(data);
    data = deinterleave<4, uint64_t, uint256_t>(z);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(z);
  }
}
BENCHMARK(BM_RoundTrip_4_64_256_Boost);

BENCHMARK_MAIN();
