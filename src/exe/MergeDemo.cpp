//
// Created by igudich on 11/6/18.
//

#include <chrono>
#include <iostream>
#include <vector>
#include <numeric>
#include <random>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/sort/sort.hpp>
#include <gflags/gflags.h>
#include <h5cpp/hdf5.hpp>

#include "EventToMDEventConversion.h"
#include "Instrument.h"
#include "IsisEventNexusLoader.h"
#include "MDBox.h"
#include "ArgumentsParsing.h"
#include "scoped_wallclock_timer.h"
#include "Merge.h"

constexpr size_t ND(3);
using IntT = uint16_t;
using MortonT = uint64_t;

const std::string AllFrames("all");

DEFINE_string(instrument, "instrument.h5", "Instrument geometry file.");
DEFINE_string(data, "raw_data.nxs", "TOF event data file.");
DEFINE_string(dataset, "raw_data_1/detector_1_events", "Path to HDF5 dataset.");
DEFINE_string(frames, "0", "Frames to load.");
DEFINE_string(space, "-10,10,-10,10,-10,10", "Q space dimensions.");
DEFINE_uint64(split_threshold, 1000, "Box splitting threshold.");
DEFINE_uint64(max_box_depth, 20, "Maximum box structure tree depth.");
DEFINE_uint64(merge_percentage, 20, "Percentage of events to merge: (100 - mp)% <-- mp%.");

std::vector<std::size_t> random_indexes(std::size_t upper, std::size_t count) {
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> dis(0, upper);
  std::vector<std::size_t> idx_to_merge(count);
  for (auto &idx: idx_to_merge)
    idx = dis(gen);
  return idx_to_merge;
}

std::vector<std::size_t> determ_indexes(std::size_t upper, std::size_t count) {
  std::vector<std::size_t> idx_to_merge(count);
  auto stride = upper / count;
  auto cur = 0;
  for (auto &idx: idx_to_merge) {
    idx = cur;
    cur += stride;
  }
  return idx_to_merge;
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  /* Load instrument */
  Instrument inst;
  {
    scoped_wallclock_timer timer("Load instrument");
    load_instrument(inst, FLAGS_instrument);
  }

  IsisEventNexusLoader loader(FLAGS_data, FLAGS_dataset);

  /* Load spectrum to detector mapping */
  {
    scoped_wallclock_timer timer("Load spectrum to detector mapping");
    loader.loadSpectrumDetectorMapping(inst.spectrum_detector_mapping);
  }

  /* Load ToF events */
  std::vector<TofEvent> events;
  {
    scoped_wallclock_timer timer("Load TOF events");

    std::vector<size_t> frameIdxs;
    {
      if (FLAGS_frames == AllFrames) {
        frameIdxs.resize(loader.frameCount());
        std::iota(frameIdxs.begin(), frameIdxs.end(), 0);
      } else {
        parse_integer_string_array(frameIdxs, FLAGS_frames);
      }
    }

    loader.loadFrames(events, frameIdxs);
    std::cout << " (" << events.size() << " ToF events loaded)\n";
  }

  events.resize(1000000); // TODO tmp

  /* Parse MD space */
  MDSpaceBounds<3> space;
  {
    std::vector<float> extents;
    parse_float_string_array(extents, FLAGS_space);

    space(0, 0) = extents[0];
    space(0, 1) = extents[1];
    space(1, 0) = extents[2];
    space(1, 1) = extents[3];
    space(2, 0) = extents[4];
    space(2, 1) = extents[5];
  }





  /* Do preprocessing */
  auto eventInfo = preprocess_events(events);
  auto eventList = getMantidNativeEventList(eventInfo);

  std::vector<MDEvent<ND, IntT, MortonT>> mdEvents;
  {
    scoped_wallclock_timer timer("Convert to Q space");

    ConversionInfo convInfo{false, Eigen::Matrix3f::Identity()};

    convert_events_native(mdEvents, eventList, convInfo, inst, space);
    std::cout << " (" << mdEvents.size() << " MD events created)\n";
  }

  std::size_t merge_size = mdEvents.size() / 100 * FLAGS_merge_percentage;
  std::vector<MDEvent<ND, IntT, MortonT>> mdEventsToMerge;
  mdEventsToMerge.reserve(merge_size);
  std::vector<MDEvent<ND, IntT, MortonT>> mdEventsBase;
  mdEventsBase.reserve(mdEvents.size() - merge_size);
  auto idx_to_merge = determ_indexes(events.size(), merge_size);
  /*Prepare data for merge*/
  {
    scoped_wallclock_timer timer("Prepare data fo merging");
    std::sort(idx_to_merge.begin(), idx_to_merge.end());
    auto iter = idx_to_merge.begin();
    auto counter = 0;
    for (auto& event: mdEvents) {
      if(iter != idx_to_merge.end()) {
        if (counter == *iter) {
          mdEventsToMerge.emplace_back(event);
          ++iter;
        } else {
          mdEventsBase.emplace_back(event);
        }
      } else {
        mdEventsBase.emplace_back(event);
      }
      ++counter;
    }
  }

  std::cerr << "Events to merge: " << mdEventsToMerge.size() << " ; Events in basic MDWS" << mdEventsBase.size() << "\n";

  /* Sort events */
  {
    scoped_wallclock_timer timer("Sort events");
    boost::sort::block_indirect_sort(mdEventsBase.begin(), mdEventsBase.end());
  }

  /* Construct box structure */
  MDBox<ND, IntT, MortonT> rootMdBox(mdEventsBase.cbegin(), mdEventsBase.cend());
  {
    scoped_wallclock_timer timer("Construct box structure");
    rootMdBox.distributeEvents(FLAGS_split_threshold, FLAGS_max_box_depth);
  }


  /* Sort events */
  {
    scoped_wallclock_timer timer("Sort events to merge");
    boost::sort::block_indirect_sort(mdEventsToMerge.begin(), mdEventsToMerge.end());
  }

#ifdef STORING_EVENTS
  /*Append events*/
  {
    scoped_wallclock_timer timer("Append events");
    rootMdBox.appendEvents(mdEventsToMerge, FLAGS_split_threshold, FLAGS_max_box_depth);
  }
#else // STORING_EVENTS
  /*Merging zcurves*/
  {
    decltype(mdEvents) curve;
    scoped_wallclock_timer timer( "Merging zcurves");
    merge_event_curves<MDEvent<ND, IntT, MortonT>>(curve, mdEvents, mdEventsToMerge);

    /* Construct box structure */
    MDBox<ND, IntT, MortonT> rootMdBox(curve.cbegin(), curve.cend());
    rootMdBox.distributeEvents(FLAGS_split_threshold, FLAGS_max_box_depth);
  }
#endif// STORING_EVENTS
}