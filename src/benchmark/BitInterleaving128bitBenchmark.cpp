#include <benchmark/benchmark.h>

#include <boost/multiprecision/cpp_int.hpp>

#include "BitInterleaving128bit.h"
#include "BitInterleavingNaive.h"

static void BM_Interleave_128Bit_4(benchmark::State &state) {
  uint32_t a, b, c, d;
  uint64_t msb, lsb;
  for (auto _ : state) {
    Interleave4_128Bit<uint32_t>(msb, lsb, a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(msb);
    benchmark::DoNotOptimize(lsb);
  }
}
BENCHMARK(BM_Interleave_128Bit_4);

static void BM_Deinterleave_128Bit_4(benchmark::State &state) {
  uint32_t a, b, c, d;
  uint64_t msb, lsb;
  for (auto _ : state) {
    Deinterleave4_128Bit(msb, lsb, a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(msb);
    benchmark::DoNotOptimize(lsb);
  }
}
BENCHMARK(BM_Deinterleave_128Bit_4);

static void BM_RoundTrip_128Bit_4(benchmark::State &state) {
  uint32_t a, b, c, d;
  uint64_t msb, lsb;
  for (auto _ : state) {
    Interleave4_128Bit<uint32_t>(msb, lsb, a, b, c, d);
    Deinterleave4_128Bit(msb, lsb, a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(msb);
    benchmark::DoNotOptimize(lsb);
  }
}
BENCHMARK(BM_RoundTrip_128Bit_4);

static void BM_Interleave_Boost128Bit_4(benchmark::State &state) {
  using namespace boost::multiprecision;

  uint32_t a, b, c, d;
  uint128_t res;
  for (auto _ : state) {
    res = Interleave4<uint128_t, uint32_t>(a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Interleave_Boost128Bit_4);

static void BM_Deinterleave_Boost128Bit_4(benchmark::State &state) {
  using namespace boost::multiprecision;

  uint32_t a, b, c, d;
  uint128_t res;
  for (auto _ : state) {
    Deinterleave4(res, a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Deinterleave_Boost128Bit_4);

static void BM_RoundTrip_Boost128Bit_4(benchmark::State &state) {
  using namespace boost::multiprecision;

  uint32_t a, b, c, d;
  uint128_t res;
  for (auto _ : state) {
    res = Interleave4<uint128_t, uint32_t>(a, b, c, d);
    Deinterleave4(res, a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_RoundTrip_Boost128Bit_4);

BENCHMARK_MAIN();
