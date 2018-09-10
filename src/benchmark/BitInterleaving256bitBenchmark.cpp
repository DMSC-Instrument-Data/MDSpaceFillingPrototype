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
