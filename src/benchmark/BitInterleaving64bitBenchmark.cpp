#include <benchmark/benchmark.h>

#include "BitInterleaving.h"
#include "BitInterleavingEigen.h"

static void BM_Interleave_4_16_64(benchmark::State &state) {
  IntArray<4, uint16_t> data;
  uint64_t res;
  for (auto _ : state) {
    res = interleave<4, uint16_t, uint64_t>(data);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Interleave_4_16_64);

static void BM_Deinterleave_4_16_64(benchmark::State &state) {
  IntArray<4, uint16_t> data;
  uint64_t res;
  for (auto _ : state) {
    data = deinterleave<4, uint16_t, uint64_t>(res);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Deinterleave_4_16_64);

static void BM_RoundTrip_4_16_64(benchmark::State &state) {
  IntArray<4, uint16_t> data;
  uint64_t res;
  for (auto _ : state) {
    res = interleave<4, uint16_t, uint64_t>(data);
    data = deinterleave<4, uint16_t, uint64_t>(res);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_RoundTrip_4_16_64);

static void BM_Interleave_Eigen_4(benchmark::State &state) {
  IntArray<4, uint16_t> data;
  uint64_t res;
  for (auto _ : state) {
    res = Interleave<uint64_t, uint16_t, 4>(data);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Interleave_Eigen_4);

static void BM_Deinterleave_Eigen_4(benchmark::State &state) {
  IntArray<4, uint16_t> data;
  uint64_t res;
  for (auto _ : state) {
    data = Deinterleave<uint64_t, uint16_t, 4>(res);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Deinterleave_Eigen_4);

static void BM_RoundTrip_Eigen_4(benchmark::State &state) {
  IntArray<4, uint16_t> data;
  uint64_t res;
  for (auto _ : state) {
    res = Interleave<uint64_t, uint16_t, 4>(data);
    data = Deinterleave<uint64_t, uint16_t, 4>(res);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_RoundTrip_Eigen_4);

BENCHMARK_MAIN();
