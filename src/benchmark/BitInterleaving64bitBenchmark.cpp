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
