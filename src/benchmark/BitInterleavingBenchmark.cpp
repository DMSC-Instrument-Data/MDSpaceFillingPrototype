#include <benchmark/benchmark.h>

#include "BitInterleaving.h"

static void BM_Interleave4(benchmark::State &state) {
  uint16_t a, b, c, d;
  uint64_t res;
  for (auto _ : state) {
    res = Interleave4<uint64_t, uint16_t>(a, b, c, d);
    Deinterleave4(res, a, b, c, d);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Interleave4);

static void BM_Interleave4_128Bit(benchmark::State &state) {
  uint32_t a, b, c, d;
  uint64_t msb, lsb;
  for (auto _ : state) {
    msb = 0;
    lsb = 0;
    Interleave4_128Bit<uint32_t>(msb, lsb, a, b, c, d);
    Deinterleave4_128Bit(msb, lsb, a, b, c, d);
    benchmark::DoNotOptimize(a);
  }
}
BENCHMARK(BM_Interleave4_128Bit);

BENCHMARK_MAIN();
