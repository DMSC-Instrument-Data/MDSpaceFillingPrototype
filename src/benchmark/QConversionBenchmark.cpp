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
#include "scoped_wallclock_timer.hpp"

const std::string dataDirPath("/home/igudich/work/MDSpaceFillingPrototype/Data/");

MDSpaceBounds<3> md_space_wish() {
  MDSpaceBounds<3> space;
  // clang-format off
  space <<
    -12.0f, 12.0f,
    -3.0f, 3.0f,
    0.0f, 23.0f;
  // clang-format on
  return space;
}

MDSpaceBounds<3> md_space_topaz() {
  MDSpaceBounds<3> space;
  // clang-format off
  space <<
    0.0f, 60.0f,
    -40.0f, 40.0f,
    0.0f, 110.0f;
  // clang-format on
  return space;
}

MDSpaceBounds<3> md_space_sxd() {
  MDSpaceBounds<3> space;
  // clang-format off
  space <<
    -18.0f, 18.0f,
    -7.0f, 17.0f,
    0.0f, 34.0f;
  // clang-format on
  return space;
}

template <size_t ND, typename IntT, typename MortonT>
void do_conversion(benchmark::State &state, const Instrument &inst,
                   const MantidNativeEventList &eventList,
                   const MDSpaceBounds<ND> &mdSpace,
                   const ConversionInfo &convInfo, const size_t splitThreshold,
                   const size_t maxBoxTreeDepth) {

  /* Convert to Q space */
  std::vector<MDEvent<ND, IntT, MortonT>> mdEvents;
  {
    scoped_wallclock_timer timer(state, "q_conversion");
    convert_events_native(mdEvents, eventList, convInfo, inst, mdSpace);
  }

  state.counters["md_events"] += mdEvents.size();

  /* Sort events */
  {
    scoped_wallclock_timer timer(state, "sort");
    boost::sort::block_indirect_sort(mdEvents.begin(), mdEvents.end());
  }

  /* Construct box structure */
  MDBox<ND, IntT, MortonT> rootMdBox(mdEvents.cbegin(), mdEvents.cend());
  {
    scoped_wallclock_timer timer(state, "box_structure");
    rootMdBox.distributeEvents(splitThreshold, maxBoxTreeDepth);
  }

  benchmark::DoNotOptimize(mdEvents);
}

void load_isis(Instrument &inst, std::vector<TofEvent> &events,
               const std::string &instFile, const std::string &dataFile,
               const std::string &dataPath) {
  /* Load instrument */
  load_instrument(inst, instFile);

  IsisEventNexusLoader loader(dataFile, dataPath);

  /* Load a mapping from the NeXus file */
  loader.loadSpectrumDetectorMapping(inst.spectrum_detector_mapping);

  /* Load ToF events */
  std::vector<size_t> frameIdxs(loader.frameCount());
  std::iota(frameIdxs.begin(), frameIdxs.end(), 0);
  loader.loadFrames(events, frameIdxs);
}

void load_mantid(Instrument &inst, std::vector<TofEvent> &events,
                 const std::string &instFile, const std::string &dataFile) {
  /* Load instrument */
  load_instrument(inst, instFile);

  MantidEventNexusLoader loader(dataFile);

  /* Load a mapping from the NeXus file */
  loader.loadSpectrumDetectorMapping(inst.spectrum_detector_mapping);

  /* Load ToF events */
  loader.loadAllEvents(events);
}

/**
 * Average counters (there are counter flags to do this in the latest Google
 * Benchmark)
 */
void average_counters(benchmark::State &state) {
  for (const auto &name :
       {"q_conversion", "sort", "box_structure", "md_events"}) {
    state.counters[name] /= state.iterations();
  }
}

template <typename IntT, typename MortonT>
void BM_QConversion_WISH_34509(benchmark::State &state) {
  constexpr size_t ND(3);

  Instrument inst;
  std::vector<TofEvent> tofEventsRaw;
  load_isis(inst, tofEventsRaw, dataDirPath + "/wish.h5",
            dataDirPath + "/WISH00034509.nxs", "/raw_data_1/detector_1_events");

  auto eventInfo = preprocess_events(tofEventsRaw);
  auto eventList = getMantidNativeEventList(eventInfo);

  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(state, inst, eventList, md_space_wish(),
                                     {false, Eigen::Matrix3f::Identity()}, 1000,
                                     20);
  }

  average_counters(state);
  for(auto& it: eventList)
    delete it;
}
BENCHMARK_TEMPLATE(BM_QConversion_WISH_34509, uint8_t, uint32_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_34509, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_34509, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_34509, uint64_t, uint256_t)
    ->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_QConversion_WISH_34509_2x(benchmark::State &state) {
  constexpr size_t ND(3);

  Instrument inst;
  std::vector<TofEvent> tofEventsRaw;
  load_isis(inst, tofEventsRaw, dataDirPath + "/wish.h5",
            dataDirPath + "/WISH00034509_2x_larger.nxs",
            "/raw_data_1/detector_1_events");

  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(state, inst, tofEventsRaw, md_space_wish(),
                                     {false, Eigen::Matrix3f::Identity()}, 1000,
                                     20);
  }

  average_counters(state);
}
BENCHMARK_TEMPLATE(BM_QConversion_WISH_34509_2x, uint8_t, uint32_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_34509_2x, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_34509_2x, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_34509_2x, uint64_t, uint256_t)
    ->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_QConversion_WISH_38423(benchmark::State &state) {
  constexpr size_t ND(3);

  Instrument inst;
  std::vector<TofEvent> tofEventsRaw;
  load_mantid(inst, tofEventsRaw, dataDirPath + "/wish.h5",
              dataDirPath + "/WISH00038423_event.nxs");

  auto eventInfo = preprocess_events(tofEventsRaw);
  auto eventList = getMantidNativeEventList(eventInfo);


  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(state, inst, eventList, md_space_wish(),
                                     {false, Eigen::Matrix3f::Identity()}, 1000,
                                     20);
  }

  for(auto& it: eventList)
    delete it;

  average_counters(state);
}
BENCHMARK_TEMPLATE(BM_QConversion_WISH_38423, uint8_t, uint32_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_38423, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_38423, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_38423, uint64_t, uint256_t)
    ->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_QConversion_WISH_37828(benchmark::State &state) {
  constexpr size_t ND(3);

  Instrument inst;
  std::vector<TofEvent> tofEventsRaw;
  load_mantid(inst, tofEventsRaw, dataDirPath + "/wish.h5",
              dataDirPath + "/WISH00037828_event.nxs");

  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(state, inst, tofEventsRaw, md_space_wish(),
                                     {false, Eigen::Matrix3f::Identity()}, 1000,
                                     20);
  }

  average_counters(state);
}
BENCHMARK_TEMPLATE(BM_QConversion_WISH_37828, uint8_t, uint32_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_37828, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_37828, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_37828, uint64_t, uint256_t)
    ->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_QConversion_WISH_37868(benchmark::State &state) {
  constexpr size_t ND(3);

  Instrument inst;
  std::vector<TofEvent> tofEventsRaw;
  load_mantid(inst, tofEventsRaw, dataDirPath + "/wish.h5",
              dataDirPath + "/WISH00037868_event.nxs");

  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(state, inst, tofEventsRaw, md_space_wish(),
                                     {false, Eigen::Matrix3f::Identity()}, 1000,
                                     20);
  }

  average_counters(state);
}
BENCHMARK_TEMPLATE(BM_QConversion_WISH_37868, uint8_t, uint32_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_37868, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_37868, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_37868, uint64_t, uint256_t)
    ->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_QConversion_TOPAZ_3132(benchmark::State &state) {
  constexpr size_t ND(3);

  Instrument inst;
  std::vector<TofEvent> tofEventsRaw;
  load_mantid(inst, tofEventsRaw, dataDirPath + "/topaz.h5",
              dataDirPath + "/TOPAZ_3132_event.nxs");

  auto eventInfo = preprocess_events(tofEventsRaw);
  auto eventList = getMantidNativeEventList(eventInfo);

  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(
        state, inst, eventList, md_space_topaz(),
        {false, Eigen::Matrix3f::Identity()}, 1000, 20);
  }

  average_counters(state);

  for(auto& it: eventList)
    delete it;
}
BENCHMARK_TEMPLATE(BM_QConversion_TOPAZ_3132, uint8_t, uint32_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_TOPAZ_3132, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_TOPAZ_3132, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_TOPAZ_3132, uint64_t, uint256_t)
    ->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_QConversion_SXD_23767(benchmark::State &state) {
  constexpr size_t ND(3);

  Instrument inst;
  std::vector<TofEvent> tofEventsRaw;
  load_mantid(inst, tofEventsRaw, dataDirPath + "/sxd.h5",
              dataDirPath + "/SXD23767_event.nxs");

  auto eventInfo = preprocess_events(tofEventsRaw);
  auto eventList = getMantidNativeEventList(eventInfo);

  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(state, inst, eventList, md_space_sxd(),
                                     {false, Eigen::Matrix3f::Identity()}, 1000,
                                     20);
  }

  average_counters(state);

  for(auto& it: eventList)
    delete it;
}
BENCHMARK_TEMPLATE(BM_QConversion_SXD_23767, uint8_t, uint32_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_SXD_23767, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_SXD_23767, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_SXD_23767, uint64_t, uint256_t)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
