#include <benchmark/benchmark.h>

#include "BitInterleaving128bit.h"

static void BM_Interleave_4_32_128(benchmark::State &state) {
  uint32_t a, b, c, d;
  uint64_t msb, lsb;
  for (auto _ : state) {
    Interleave_4_32_128(msb, lsb, a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(msb);
    benchmark::DoNotOptimize(lsb);
  }
}
BENCHMARK(BM_Interleave_4_32_128);

static void BM_Deinterleave_4_32_128(benchmark::State &state) {
  uint32_t a, b, c, d;
  uint64_t msb, lsb;
  for (auto _ : state) {
    Deinterleave_4_32_128(msb, lsb, a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(msb);
    benchmark::DoNotOptimize(lsb);
  }
}
BENCHMARK(BM_Deinterleave_4_32_128);

static void BM_RoundTrip_4_32_128(benchmark::State &state) {
  uint32_t a, b, c, d;
  uint64_t msb, lsb;
  for (auto _ : state) {
    Interleave_4_32_128(msb, lsb, a, b, c, d);
    Deinterleave_4_32_128(msb, lsb, a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(msb);
    benchmark::DoNotOptimize(lsb);
  }
}
BENCHMARK(BM_RoundTrip_4_32_128);

static void BM_Interleave_4_32_128_Boost(benchmark::State &state) {
  IntArray<4, uint32_t> data;
  uint128_t z;
  for (auto _ : state) {
    z = interleave<4, uint32_t, uint128_t>(data);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(z);
  }
}
BENCHMARK(BM_Interleave_4_32_128_Boost);

static void BM_Deinterleave_4_32_128_Boost(benchmark::State &state) {
  IntArray<4, uint32_t> data;
  uint128_t z;
  for (auto _ : state) {
    data = deinterleave<4, uint32_t, uint128_t>(z);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(z);
  }
}
BENCHMARK(BM_Deinterleave_4_32_128_Boost);

static void BM_RoundTrip_4_32_128_Boost(benchmark::State &state) {
  IntArray<4, uint32_t> data;
  uint128_t z;
  for (auto _ : state) {
    z = interleave<4, uint32_t, uint128_t>(data);
    data = deinterleave<4, uint32_t, uint128_t>(z);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(z);
  }
}
BENCHMARK(BM_RoundTrip_4_32_128_Boost);

BENCHMARK_MAIN();
