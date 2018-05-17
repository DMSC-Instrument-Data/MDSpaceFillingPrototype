#include <benchmark/benchmark.h>

#include "BitInterleaving.h"
#include "BitInterleavingEigen.h"

static void BM_Interleave_4_16_64(benchmark::State &state) {
  uint16_t a, b, c, d;
  uint64_t res;
  for (auto _ : state) {
    res = Interleave_4_16_64(a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Interleave_4_16_64);

static void BM_Deinterleave_4_16_64(benchmark::State &state) {
  uint16_t a, b, c, d;
  uint64_t res;
  for (auto _ : state) {
    Deinterleave_4_16_64(res, a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Deinterleave_4_16_64);

static void BM_RoundTrip_4_16_64(benchmark::State &state) {
  uint16_t a, b, c, d;
  uint64_t res;
  for (auto _ : state) {
    res = Interleave_4_16_64(a, b, c, d);
    Deinterleave_4_16_64(res, a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_RoundTrip_4_16_64);

static void BM_Interleave_Eigen_4(benchmark::State &state) {
  Eigen::Array<uint16_t, 4, 1> data;
  uint64_t res;
  for (auto _ : state) {
    res = Interleave<uint64_t, uint16_t, 4>(data);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Interleave_Eigen_4);

static void BM_Deinterleave_Eigen_4(benchmark::State &state) {
  Eigen::Array<uint16_t, 4, 1> data;
  uint64_t res;
  for (auto _ : state) {
    data = Deinterleave<uint64_t, uint16_t, 4>(res);
    benchmark::DoNotOptimize(data);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Deinterleave_Eigen_4);

static void BM_RoundTrip_Eigen_4(benchmark::State &state) {
  Eigen::Array<uint16_t, 4, 1> data;
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
