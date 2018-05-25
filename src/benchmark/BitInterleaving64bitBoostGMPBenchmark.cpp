#include <benchmark/benchmark.h>

#include <boost/multiprecision/gmp.hpp>

#include "BitInterleavingBoostGMP.h"

static void BM_Interleave_4_16_64_BoostGMP(benchmark::State &state) {
  mpz_int a, b, c, d;
  mpz_int res;
  for (auto _ : state) {
    res = Interleave_4_16_64(a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Interleave_4_16_64_BoostGMP);

static void BM_Deinterleave_4_16_64_BoostGMP(benchmark::State &state) {
  mpz_int a, b, c, d;
  mpz_int res;
  for (auto _ : state) {
    Deinterleave_4_16_64(res, a, b, c, d);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_Deinterleave_4_16_64_BoostGMP);

static void BM_RoundTrip_4_16_64_BoostGMP(benchmark::State &state) {
  mpz_int a, b, c, d;
  mpz_int res;
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
BENCHMARK(BM_RoundTrip_4_16_64_BoostGMP);

BENCHMARK_MAIN();
