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

#include <Eigen/Dense>
#include <boost/multiprecision/cpp_int.hpp>

#include "CoordinateConversion.h"

using namespace boost::multiprecision;

void BM_CalculateRequiredCoordinateIntegerWidth_4(benchmark::State &state) {
  MDSpaceBounds<4> bounds;
  // clang-format off
  bounds <<
    1.0f, 8.0f,
    5.0f, 6.0f,
    3.0f, 7.0f,
    0.0f, 3.0f;
  // clang-format on

  MDSpaceSteps<4> steps;
  steps << 0.02f, 0.01f, 0.015f, 0.021f;

  size_t result;

  for (auto _ : state) {
    result = CalculateRequiredCoordinateIntegerWidth<4>(bounds, steps);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_CalculateRequiredCoordinateIntegerWidth_4);

template <typename T>
void BM_ConvertCoordinatesToIntegerRange(benchmark::State &state) {
  MDCoordinate<4> coord;
  coord << 1.0f, 6.0f, 5.0f, 12.5f;

  MDSpaceBounds<4> bounds;
  // clang-format off
  bounds <<
    0.0f, 2.0f,
    5.0f, 10.0f,
    3.0f, 7.0f,
    5.0f, 15.0f;
  // clang-format on

  for (auto _ : state) {
    auto result = ConvertCoordinatesToIntegerRange<4, T>(bounds, coord);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK_TEMPLATE(BM_ConvertCoordinatesToIntegerRange, uint8_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinatesToIntegerRange, uint16_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinatesToIntegerRange, uint32_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinatesToIntegerRange, uint64_t);

template <typename T>
void BM_ConvertCoordinatesFromIntegerRange(benchmark::State &state) {
  Eigen::Array<T, 4, 1> coord;
  coord << 100, 100, 100, 100;

  MDSpaceBounds<4> bounds;
  // clang-format off
  bounds <<
    0.0f, 2.0f,
    5.0f, 10.0f,
    3.0f, 7.0f,
    5.0f, 15.0f;
  // clang-format on

  for (auto _ : state) {
    const auto result = ConvertCoordinatesFromIntegerRange<4, T>(bounds, coord);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK_TEMPLATE(BM_ConvertCoordinatesFromIntegerRange, uint8_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinatesFromIntegerRange, uint16_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinatesFromIntegerRange, uint32_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinatesFromIntegerRange, uint64_t);

template <typename T>
void BM_ConvertCoordinateToIntegerRange(benchmark::State &state) {
  float lower(0.0f);
  float upper(2.0f);

  float coord(1.25f);

  T result;

  for (auto _ : state) {
    result = ConvertCoordinateToIntegerRange<T>(coord, lower, upper);
    benchmark::DoNotOptimize(coord);
    benchmark::DoNotOptimize(upper);
    benchmark::DoNotOptimize(lower);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK_TEMPLATE(BM_ConvertCoordinateToIntegerRange, uint8_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinateToIntegerRange, uint16_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinateToIntegerRange, uint32_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinateToIntegerRange, uint64_t);

template <typename T>
void BM_ConvertCoordinateFromIntegerRange(benchmark::State &state) {
  float lower(0.0f);
  float upper(2.0f);

  T coord(115);

  float result;

  for (auto _ : state) {
    result = ConvertCoordinateFromIntegerRange<T>(coord, lower, upper);
    benchmark::DoNotOptimize(coord);
    benchmark::DoNotOptimize(upper);
    benchmark::DoNotOptimize(lower);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK_TEMPLATE(BM_ConvertCoordinateFromIntegerRange, uint8_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinateFromIntegerRange, uint16_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinateFromIntegerRange, uint32_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinateFromIntegerRange, uint64_t);

template <typename T>
void BM_ConvertCoordinateToIntegerRange_4(benchmark::State &state) {
  float lower(0.0f);
  float upper(2.0f);

  float w(1.25f);
  float x(1.25f);
  float y(1.25f);
  float z(1.25f);

  T rw, rx, ry, rz;

  for (auto _ : state) {
    rw = ConvertCoordinateToIntegerRange<T>(w, lower, upper);
    rx = ConvertCoordinateToIntegerRange<T>(x, lower, upper);
    ry = ConvertCoordinateToIntegerRange<T>(y, lower, upper);
    rz = ConvertCoordinateToIntegerRange<T>(z, lower, upper);

    benchmark::DoNotOptimize(upper);
    benchmark::DoNotOptimize(lower);

    benchmark::DoNotOptimize(w);
    benchmark::DoNotOptimize(x);
    benchmark::DoNotOptimize(y);
    benchmark::DoNotOptimize(z);

    benchmark::DoNotOptimize(rw);
    benchmark::DoNotOptimize(rx);
    benchmark::DoNotOptimize(ry);
    benchmark::DoNotOptimize(rz);
  }
}
BENCHMARK_TEMPLATE(BM_ConvertCoordinateToIntegerRange_4, uint8_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinateToIntegerRange_4, uint16_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinateToIntegerRange_4, uint32_t);
BENCHMARK_TEMPLATE(BM_ConvertCoordinateToIntegerRange_4, uint64_t);

BENCHMARK_MAIN();
