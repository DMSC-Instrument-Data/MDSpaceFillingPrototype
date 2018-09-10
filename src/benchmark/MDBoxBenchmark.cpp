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

#include <random>

#include <boost/sort/sort.hpp>

#include "MDBox.h"
#include "MDEvent.h"

const size_t ND(4);
using IntT = uint16_t;
using MortonT = uint64_t;

using Event = MDEvent<ND, IntT, MortonT>;
using Box = MDBox<ND, IntT, MortonT>;

void BM_MDBox_fill(benchmark::State &state) {
  std::mt19937 gen;
  std::uniform_int_distribution<MortonT> dist;

  for (auto _ : state) {
    state.PauseTiming();

    /* Test parameters */
    const size_t numEvents(state.range(0));
    const size_t splitThreshold(state.range(1));
    const size_t maxDepth(state.range(2));

    /* Generate test event Z-curve */
    Event::ZCurve curve;
    for (size_t i = 0; i < numEvents; i++) {
      curve.emplace_back(dist(gen));
    }
    boost::sort::block_indirect_sort(curve.begin(), curve.end());

    state.ResumeTiming();

    /* Create root box and perform splitting/distribution */
    Box root(curve.cbegin(), curve.cend());
    root.distributeEvents(splitThreshold, maxDepth);
  }
}
/* NB: ConvertToMD currently defaults to split threshold of 1000 and max depth
 * of 20 */
BENCHMARK(BM_MDBox_fill)
    ->Args({1000000, 1000, 20})
    ->Args({10000000, 1000, 20})
    ->Args({100000000, 1000, 20})
    ->Args({1000000000, 1000, 20})
    ->Args({469388241, 1000, 20})
    /* ->Args({75000000000, 1000, 20}) */
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
