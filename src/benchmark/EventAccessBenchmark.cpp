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

#include <boost/random.hpp>
#include <boost/sort/sort.hpp>

#include "EventToMDEventConversion.h"
#include "IsisEventNexusLoader.h"
#include "MDBox.h"
#include "MDEvent.h"
#include "MantidEventNexusLoader.h"
#include "Rebin.h"

const std::string dataDirPath("..");

template <typename T> T generate_random_number() {
  static std::mt19937 gen;
  static std::uniform_int_distribution<T> dist;
  return dist(gen);
}

template <> uint128_t generate_random_number() {
  using namespace boost::random;
  static independent_bits_engine<mt19937, 128, uint128_t> gen;
  return gen();
}

template <typename IntT, typename MortonT>
static void BM_AccessCoordinates(benchmark::State &state) {
  constexpr size_t ND(4);
  const size_t numEvents(state.range(0));

  using Event = MDEvent<ND, IntT, MortonT>;

  /* Generate test event Z-curve */
  typename Event::ZCurve curve;
  for (size_t i = 0; i < numEvents; i++) {
    curve.emplace_back(generate_random_number<MortonT>());
  }
  boost::sort::block_indirect_sort(curve.begin(), curve.end());

  MDSpaceBounds<ND> space;
  // clang-format off
  space <<
    -10.0f, 10.0f,
    -10.0f, 10.0f,
    -10.0f, 10.0f,
    -10.0f, 10.0f;
  // clang-format on

  for (auto _ : state) {
    for (const auto &e : curve) {
      const MDCoordinate<ND> coords = e.coordinates(space);
      benchmark::DoNotOptimize(coords);
    }
  }
  state.SetItemsProcessed(state.iterations() * numEvents);
}
BENCHMARK_TEMPLATE(BM_AccessCoordinates, uint16_t, uint64_t)
    ->Args({1000000000})
    ->Args({469388241})
    ->Args({100000000})
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_AccessCoordinates, uint32_t, uint128_t)
    ->Args({1000000000})
    ->Args({469388241})
    ->Args({100000000})
    ->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
static void BM_MaskedMortonLessThanEqual(benchmark::State &state) {
  constexpr size_t ND(4);
  const size_t numComparisons(state.range(0));

  MortonT a;
  MortonT b;

  for (auto _ : state) {
    for (int i = 0; i < numComparisons; i++) {
      bool result = masked_morton_lte<ND, IntT, MortonT>(a, b);
      benchmark::DoNotOptimize(result);
    }
  }
  state.SetItemsProcessed(state.iterations() * numComparisons);
}
BENCHMARK_TEMPLATE(BM_MaskedMortonLessThanEqual, uint16_t, uint64_t)->Arg(100);
BENCHMARK_TEMPLATE(BM_MaskedMortonLessThanEqual, uint32_t, uint128_t)->Arg(100);

void BM_GenerateBinningTransform_BinMD(benchmark::State &state) {
  constexpr size_t ND(3);
  constexpr size_t binCount(100);

  const MDSpaceBounds<ND> mdSpace;

  for (auto _ : state) {
    MDSpaceDimensions<ND> mdSpaceSize = mdSpace.col(1) - mdSpace.col(0);

    Eigen::Quaternionf rot = Eigen::Quaternionf::Identity();
    Eigen::Vector3f scale =
        Eigen::Array3f::Constant(binCount - 1) / mdSpaceSize;
    Eigen::Vector3f origin =
        -Eigen::Array3f(mdSpace.col(0).transpose()) * Eigen::Array3f(scale);

    Eigen::Affine3f transform;
    transform.fromPositionOrientationScale(origin, rot, scale);

    benchmark::DoNotOptimize(transform);
  }
}
BENCHMARK(BM_GenerateBinningTransform_BinMD);

void BM_ApplyBinningTransform_BinMD(benchmark::State &state) {
  constexpr size_t ND(3);
  constexpr size_t binCount(100);

  const size_t numEvents(state.range(0));

  const MDSpaceBounds<ND> mdSpace;

  const MDSpaceDimensions<ND> mdSpaceSize = mdSpace.col(1) - mdSpace.col(0);

  const Eigen::Quaternionf rot = Eigen::Quaternionf::Identity();
  const Eigen::Vector3f scale =
      Eigen::Array3f::Constant(binCount - 1) / mdSpaceSize;
  const Eigen::Vector3f origin =
      -Eigen::Array3f(mdSpace.col(0).transpose()) * Eigen::Array3f(scale);

  Eigen::Affine3f transform;
  transform.fromPositionOrientationScale(origin, rot, scale);

  const MDCoordinate<ND> coord{-1.0f, 4.0f, 5.0f};

  for (auto _ : state) {
    for (size_t i = 0; i < numEvents; i++) {
      BinIndices<ND> transformed = (transform * coord).cast<size_t>();
      benchmark::DoNotOptimize(transformed);
    }
  }
  state.SetItemsProcessed(state.iterations() * numEvents);
}
BENCHMARK(BM_ApplyBinningTransform_BinMD)
    ->Arg(1000000000)
    ->Arg(469388241)
    ->Arg(100000000)
    ->Unit(benchmark::kMillisecond);

template <size_t ND, typename IntT, typename MortonT>
void BM_GenerateBinningTransform_Prototype(benchmark::State &state) {
  MDSpaceBounds<ND> mdSpace;

  for (auto _ : state) {
    const AffineND<float, ND> transform =
        generate_binning_transform<ND, IntT, MortonT>(mdSpace, 100);
    benchmark::DoNotOptimize(transform);
  }
}
BENCHMARK_TEMPLATE(BM_GenerateBinningTransform_Prototype, 3, uint16_t,
                   uint64_t);
BENCHMARK_TEMPLATE(BM_GenerateBinningTransform_Prototype, 3, uint32_t,
                   uint128_t);
BENCHMARK_TEMPLATE(BM_GenerateBinningTransform_Prototype, 4, uint16_t,
                   uint64_t);
BENCHMARK_TEMPLATE(BM_GenerateBinningTransform_Prototype, 4, uint32_t,
                   uint128_t);

template <size_t ND, typename IntT, typename MortonT>
void BM_ApplyBinningTransform_Prototype(benchmark::State &state) {
  using Event = MDEvent<ND, IntT, MortonT>;

  const size_t numEvents(state.range(0));

  MDSpaceBounds<ND> mdSpace;
  const Event event(MDCoordinate<ND>(), mdSpace);

  const AffineND<float, ND> transform =
      generate_binning_transform<ND, IntT, MortonT>(mdSpace, 100);

  for (auto _ : state) {
    for (size_t i = 0; i < numEvents; i++) {
      const BinIndices<ND> result =
          (transform * event.integerCoordinates().template cast<float>())
              .template cast<size_t>();
      benchmark::DoNotOptimize(result);
    }
  }
  state.SetItemsProcessed(state.iterations() * numEvents);
}
BENCHMARK_TEMPLATE(BM_ApplyBinningTransform_Prototype, 3, uint16_t, uint64_t)
    ->Arg(1000000000)
    ->Arg(469388241)
    ->Arg(100000000)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_ApplyBinningTransform_Prototype, 3, uint32_t, uint128_t)
    ->Arg(1000000000)
    ->Arg(469388241)
    ->Arg(100000000)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_ApplyBinningTransform_Prototype, 4, uint16_t, uint64_t)
    ->Arg(1000000000)
    ->Arg(469388241)
    ->Arg(100000000)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_ApplyBinningTransform_Prototype, 4, uint32_t, uint128_t)
    ->Arg(1000000000)
    ->Arg(469388241)
    ->Arg(100000000)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
