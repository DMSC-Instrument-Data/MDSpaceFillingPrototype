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

#include <numeric>

#include <boost/sort/sort.hpp>

#include "BitInterleaving.h"
#include "EventToMDEventConversion.h"
#include "Instrument.h"
#include "IsisEventNexusLoader.h"
#include "MDBox.h"
#include "MDEvent.h"
#include "MantidEventNexusLoader.h"
#include "Merge.h"
#include "scoped_wallclock_timer.hpp"

const std::string dataDirPath("..");

template <size_t ND, typename IntT, typename MortonT>
void load_and_convert(typename MDEvent<ND, IntT, MortonT>::ZCurve &mdEvents,
                      const std::string &dataFilename) {
  /* MD space */
  MDSpaceBounds<3> mdSpace;
  // clang-format off
  mdSpace <<
    -60.0f, 60.0f,
    -100.0f, 100.0f,
    0.0f, 55.0f;
  // clang-format on

  /* Load instrument */
  Instrument inst;
  load_instrument(inst, dataDirPath + "/sxd.h5");

  std::vector<TofEvent> tofEvents;
  MantidEventNexusLoader loader(dataDirPath + dataFilename);

  /* Load a mapping from the NeXus file */
  loader.loadSpectrumDetectorMapping(inst.spectrum_detector_mapping);

  /* Load ToF events */
  loader.loadAllEvents(tofEvents);

  /* Convert to Q */
  convert_events(mdEvents, tofEvents, {false, Eigen::Matrix3f::Identity()},
                 inst, mdSpace);
  boost::sort::block_indirect_sort(mdEvents.begin(), mdEvents.end());
}

/**
 * Average counters (there are counter flags to do this in the latest Google
 * Benchmark)
 */
void average_counters(benchmark::State &state) {
  for (const auto &name : {"merge", "box_structure"}) {
    state.counters[name] /= state.iterations();
  }
}

template <typename IntT, typename MortonT>
void BM_Merge_Inplace(benchmark::State &state) {
  constexpr size_t ND(3);
  using Event = MDEvent<ND, IntT, MortonT>;

  typename Event::ZCurve mdEvents1, mdEvents2;
  load_and_convert<ND, IntT, MortonT>(mdEvents1, "/SXD30528_event.nxs");
  load_and_convert<ND, IntT, MortonT>(mdEvents2, "/SXD30529_event.nxs");
  state.counters["md_events_curve_1"] = mdEvents1.size();
  state.counters["md_events_curve_2"] = mdEvents2.size();

  for (auto _ : state) {
    /* Copy first curve so that original data is not modified */
    state.PauseTiming();
    typename Event::ZCurve mdEvents1Copy(mdEvents1);
    state.ResumeTiming();

    /* Merge event curves */
    {
      scoped_wallclock_timer timer(state, "merge");
      merge_event_curves_inplace<Event>(mdEvents1Copy, mdEvents2);
    }

    /* Construct box structure */
    MDBox<ND, IntT, MortonT> rootMdBox(mdEvents1Copy.cbegin(),
                                       mdEvents1Copy.cend());
    {
      scoped_wallclock_timer timer(state, "box_structure");
      rootMdBox.distributeEvents(1000, 20);
    }

    /* Record number of events in merged curve */
    state.PauseTiming();
    state.counters["md_events_merged"] = mdEvents1Copy.size();
    state.ResumeTiming();
  }

  average_counters(state);
}
BENCHMARK_TEMPLATE(BM_Merge_Inplace, uint8_t, uint32_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_Inplace, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_Inplace, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_Inplace, uint64_t, uint256_t)
    ->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_Merge_New(benchmark::State &state) {
  constexpr size_t ND(3);
  using Event = MDEvent<ND, IntT, MortonT>;

  typename Event::ZCurve mdEvents1, mdEvents2;
  load_and_convert<ND, IntT, MortonT>(mdEvents1, "/SXD30528_event.nxs");
  load_and_convert<ND, IntT, MortonT>(mdEvents2, "/SXD30529_event.nxs");
  state.counters["md_events_curve_1"] = mdEvents1.size();
  state.counters["md_events_curve_2"] = mdEvents2.size();

  for (auto _ : state) {
    /* Merge event curves */
    typename Event::ZCurve curve;
    {
      scoped_wallclock_timer timer(state, "merge");
      merge_event_curves<Event>(curve, mdEvents1, mdEvents2);
    }

    /* Construct box structure */
    MDBox<ND, IntT, MortonT> rootMdBox(curve.cbegin(), curve.cend());
    {
      scoped_wallclock_timer timer(state, "box_structure");
      rootMdBox.distributeEvents(1000, 20);
    }

    /* Record number of events in merged curve */
    state.PauseTiming();
    state.counters["md_events_merged"] = curve.size();
    state.ResumeTiming();
  }

  average_counters(state);
}
BENCHMARK_TEMPLATE(BM_Merge_New, uint8_t, uint32_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_New, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_New, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_New, uint64_t, uint256_t)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
