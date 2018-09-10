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

#include "MDEvent.h"

static void BM_CreateEvent_N4_100000000(benchmark::State &state) {
  MDSpaceBounds<4> space;
  // clang-format off
  space <<
    0.0f, 10.0f,
    0.0f, 10.0f,
    0.0f, 10.0f,
    0.0f, 10.0f;
  // clang-format on

  MDCoordinate<4> coord;
  coord << 5.0f, 6.0f, 7.0f, 3.0f;

  for (auto _ : state) {
    for (size_t i = 0; i < 10000000; i++) {
      MDEvent<4, uint16_t, uint64_t> event(coord, space);
      benchmark::DoNotOptimize(event);
    }
  }
}
BENCHMARK(BM_CreateEvent_N4_100000000)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
