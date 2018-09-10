/*
 * Space filling curve prototype for MD event data structure
 * Copyright (C) 2018 European Spallation Source
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
