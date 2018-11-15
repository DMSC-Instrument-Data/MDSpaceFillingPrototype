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
#include <fstream>

#include "BitInterleaving.h"
#include "EventToMDEventConversion.h"
#include "Instrument.h"
#include "IsisEventNexusLoader.h"
#include "MDBox.h"
#include "MDEvent.h"
#include "MantidEventNexusLoader.h"
#include "Merge.h"
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
void load_and_convert(typename MDEvent<ND, IntT, MortonT>::ZCurve &mdEvents,
                      const std::string &dataFilename, const std::string &intrumentFilename) {
  /* MD space */
  MDSpaceBounds<3> mdSpace;
  if(intrumentFilename.find("wish") != std::string::npos)
    mdSpace = md_space_wish();
  else if(intrumentFilename.find("topaz") != std::string::npos)
    mdSpace = md_space_topaz();
  else if(intrumentFilename.find("sxd") != std::string::npos)
    mdSpace = md_space_sxd();

  /* Load instrument */
  Instrument inst;
  load_instrument(inst, dataDirPath + "/" + intrumentFilename);

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
  for (const auto &name : {"merge"}) {
    state.counters[name] /= state.iterations();
  }
}

class Data{
public:
  Data(const std::vector<std::string>& fileNames, const std::string& instrumentFilename)  {
    if(instrumentFilename.find("wish") != std::string::npos)
      space = md_space_wish();
    else if(instrumentFilename.find("topaz") != std::string::npos)
      space = md_space_topaz();
    else if(instrumentFilename.find("sxd") != std::string::npos)
      space = md_space_sxd();

    load_instrument(instrument, dataDirPath + instrumentFilename);
      for(auto& elem: fileNames)
        data.insert(std::make_pair(elem, std::vector<TofEvent>()));


    for(auto& pair: data) {
      std::cerr << "Loading " << pair.first << "\n";
      MantidEventNexusLoader loader(dataDirPath + "/" + pair.first);
      /* Load a mapping from the NeXus file */
      loader.loadSpectrumDetectorMapping(instrument.spectrum_detector_mapping);
      /* Load ToF events */
      loader.loadAllEvents(pair.second);
    }

  }
public:
  Instrument instrument;
  MDSpaceBounds<3> space;
  std::map<std::string, std::vector<TofEvent>> data;
};

template <typename IntT, typename MortonT>
void dumpMortonNumbers(const std::string& filename, const std::vector<MDEvent<3, IntT, MortonT>>& events) {
  std::fstream fs;
  fs.open(filename.c_str(), std::ios_base::out);
  for(const auto& event: events)
    fs << event.mortonNumber() << "\n";
}


template <typename IntT, typename MortonT>
void bm_merge(benchmark::State &state, Data& data, const std::string& fData1, const std::string& fData2) {
  constexpr size_t ND(3);
  using Event = MDEvent<ND, IntT, MortonT>;

  typename Event::ZCurve mdEvents1, mdEvents2;

  convert_events(mdEvents1, data.data.at(fData1), {false, Eigen::Matrix3f::Identity()}, data.instrument, data.space);
  boost::sort::block_indirect_sort(mdEvents1.begin(), mdEvents1.end());
  convert_events(mdEvents2, data.data.at(fData2), {false, Eigen::Matrix3f::Identity()}, data.instrument, data.space);
  boost::sort::block_indirect_sort(mdEvents2.begin(), mdEvents2.end());

  dumpMortonNumbers(fData1 + "_morton.txt", mdEvents1);
  dumpMortonNumbers(fData2 + "_morton.txt", mdEvents2);

  state.counters["md_events_curve_1"] = mdEvents1.size();
  state.counters["md_events_curve_2"] = mdEvents2.size();

  for (auto _ : state) {
    /* Copy first curve so that original data is not modified */
    state.PauseTiming();
    MDBox<ND, IntT, MortonT> rootMdBox1(mdEvents1.begin(), mdEvents1.end());
    rootMdBox1.distributeEvents(1000, 20);
    MDBox<ND, IntT, MortonT> rootMdBox2(mdEvents2.begin(), mdEvents2.end());
    rootMdBox2.distributeEvents(1000, 20);
    state.ResumeTiming();

    /* Merge event curves */
    {
      scoped_wallclock_timer timer(state, "merge");
      rootMdBox1.merge(rootMdBox2, 1000, 20);
    }

    /* Record number of events in merged curve */
    state.PauseTiming();
    state.counters["md_events_merged"] = rootMdBox1.totalEvents();
    state.ResumeTiming();
  }

  average_counters(state);
}


Data topaz_data({"TOPAZ_3132_event_0_10.nxs",
                 "TOPAZ_3132_event_0_20.nxs",
                 "TOPAZ_3132_event_0_30.nxs",
                 "TOPAZ_3132_event_0_50.nxs",
                 "TOPAZ_3132_event_50_100.nxs",
                 "TOPAZ_3132_event_30_100.nxs",
                 "TOPAZ_3132_event_20_100.nxs",
                 "TOPAZ_3132_event_10_100.nxs",},
                "/topaz.h5");

template <typename IntT, typename MortonT>
void BM_Merge_Topaz_10(benchmark::State &state) {
  bm_merge<IntT, MortonT>(state, topaz_data, "TOPAZ_3132_event_10_100.nxs", "TOPAZ_3132_event_0_10.nxs");
}
BENCHMARK_TEMPLATE(BM_Merge_Topaz_10, uint16_t, uint64_t)
->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_Topaz_10, uint32_t, uint128_t)
->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_Merge_Topaz_20(benchmark::State &state) {
  bm_merge<IntT, MortonT>(state, topaz_data, "TOPAZ_3132_event_20_100.nxs", "TOPAZ_3132_event_0_20.nxs");
}
BENCHMARK_TEMPLATE(BM_Merge_Topaz_20, uint16_t, uint64_t)
->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_Topaz_20, uint32_t, uint128_t)
->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_Merge_Topaz_30(benchmark::State &state) {
  bm_merge<IntT, MortonT>(state, topaz_data, "TOPAZ_3132_event_30_100.nxs", "TOPAZ_3132_event_0_30.nxs");
}
BENCHMARK_TEMPLATE(BM_Merge_Topaz_30, uint16_t, uint64_t)
->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_Topaz_30, uint32_t, uint128_t)
->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_Merge_Topaz_50(benchmark::State &state) {
  bm_merge<IntT, MortonT>(state,  topaz_data,  "TOPAZ_3132_event_50_100.nxs", "TOPAZ_3132_event_0_50.nxs");
}
BENCHMARK_TEMPLATE(BM_Merge_Topaz_50, uint16_t, uint64_t)
->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_Topaz_50, uint32_t, uint128_t)
->Unit(benchmark::kMillisecond);



Data sxd_data({
               "SXD30535_event_0_60.nxs",
               "SXD30535_event_0_87.5.nxs",
               "SXD30535_event_0_98.nxs",
               "SXD30535_event_60_100.nxs",
               "SXD30535_event_87.5_100.nxs",
               "SXD30535_event_98_100.nxs"
               },
              "/sxd.h5");


template <typename IntT, typename MortonT>
void BM_Merge_SXD_40(benchmark::State &state) {
  bm_merge<IntT, MortonT>(state, sxd_data, "SXD30535_event_0_60.nxs", "SXD30535_event_60_100.nxs");
}
BENCHMARK_TEMPLATE(BM_Merge_SXD_40, uint16_t, uint64_t)
->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_SXD_40, uint32_t, uint128_t)
->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_Merge_SXD_12(benchmark::State &state) {
  bm_merge<IntT, MortonT>(state, sxd_data, "SXD30535_event_0_87.5.nxs", "SXD30535_event_87.5_100.nxs");
}
BENCHMARK_TEMPLATE(BM_Merge_SXD_12, uint16_t, uint64_t)
->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_SXD_12, uint32_t, uint128_t)
->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_Merge_SXD_2(benchmark::State &state) {
  bm_merge<IntT, MortonT>(state, sxd_data, "SXD30535_event_0_98.nxs", "SXD30535_event_98_100.nxs");
}
BENCHMARK_TEMPLATE(BM_Merge_SXD_2, uint16_t, uint64_t)
->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_Merge_SXD_2, uint32_t, uint128_t)
->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
