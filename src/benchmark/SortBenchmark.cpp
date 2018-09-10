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

#include <boost/sort/sort.hpp>
#include <tbb/parallel_sort.h>

#include "DistributionGenerator.h"

#define REGISTER_SORT_BENCHMARK(type, container, data, sort)                   \
  benchmark::RegisterBenchmark(#type " " #container " " #sort " " #data,       \
                               [=](benchmark::State & state) {                 \
    for (auto _ : state) {                                                     \
      state.PauseTiming();                                                     \
      container<type> v;                                                       \
      std::copy(data.begin(), data.end(), std::back_inserter(v));              \
      state.ResumeTiming();                                                    \
      sort(v.begin(), v.end());                                                \
    }                                                                          \
                               })->Unit(benchmark::kMillisecond);

int main(int argc, char **argv) {
  benchmark::Initialize(&argc, argv);

  GENERATE_DATASET(uniform, uint64_t, GenerateRandomUniformDataset, 10000000);

  GENERATE_DATASET(multiple_peaks, uint64_t,
                   GenerateRandomPeakDataset_ClusteredPeaks, 10000000);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform, std::sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks, std::sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform,
                          boost::sort::spreadsort::spreadsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          boost::sort::spreadsort::spreadsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform, boost::sort::pdqsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          boost::sort::pdqsort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform,
                          boost::sort::block_indirect_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          boost::sort::block_indirect_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform,
                          boost::sort::sample_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          boost::sort::sample_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform,
                          boost::sort::parallel_stable_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          boost::sort::parallel_stable_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, uniform, tbb::parallel_sort);

  REGISTER_SORT_BENCHMARK(uint64_t, std::vector, multiple_peaks,
                          tbb::parallel_sort);

  benchmark::RunSpecifiedBenchmarks();
}
