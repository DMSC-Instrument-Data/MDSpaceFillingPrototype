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

#include <chrono>
#include <iostream>
#include <vector>
#include <numeric>

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

void parse_integer_string_array(std::vector<size_t> &numbers,
                                const std::string &str) {
  std::vector<std::string> subStrings;
  boost::algorithm::split(subStrings, str, boost::algorithm::is_any_of(","));
  for (const auto &p : subStrings) {
    numbers.push_back(std::stol(p));
  }
}

void parse_float_string_array(std::vector<float> &numbers,
                              const std::string &str) {
  std::vector<std::string> subStrings;
  boost::algorithm::split(subStrings, str, boost::algorithm::is_any_of(","));
  for (const auto &p : subStrings) {
    numbers.push_back(std::stof(p));
  }
}

class scoped_wallclock_timer {
public:
  using Clock = std::chrono::high_resolution_clock;

public:
  scoped_wallclock_timer(const std::string &name)
      : m_name(name), m_start(Clock::now()) {
    std::cout << "TIMER: " << m_name << " started...\n";
  }

  ~scoped_wallclock_timer() {
    const auto duration = Clock::now() - m_start;
    std::cout << "TIMER: " << m_name << " complete in "
              << std::chrono::duration<float>(duration).count() << " seconds\n";
  }

private:
  const std::string m_name;
  const std::chrono::time_point<Clock> m_start;
};

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


  std::vector<MDEvent<ND, IntT, MortonT>> mdEvents;
  if(0) {
    /* Convert to Q space */
    {
      scoped_wallclock_timer timer("Convert to Q space");

      ConversionInfo convInfo{false, Eigen::Matrix3f::Identity()};

      convert_events(mdEvents, events, convInfo, inst, space);
      std::cout << " (" << mdEvents.size() << " MD events created)\n";
    }
  }


  /* Do preprocessing */
  auto eventInfo = preprocess_events(events);
  auto eventList = getMantidNativeEventList(eventInfo);

  if(1) {
    /* Convert to Q space native */
    mdEvents.clear();
    {
      scoped_wallclock_timer timer("Convert to Q space native");

      ConversionInfo convInfo{false, Eigen::Matrix3f::Identity()};

      convert_events_native(mdEvents, eventList, convInfo, inst, space);
      std::cout << " (" << mdEvents.size() << " MD events created)\n";
    }
  }

  if(0) {
    /* Convert to Q space native 1*/
    mdEvents.clear();
    {
      scoped_wallclock_timer timer("Convert to Q space native 1");

      ConversionInfo convInfo{false, Eigen::Matrix3f::Identity()};

      convert_events_native_1(mdEvents, eventList, convInfo, inst, space);
      std::cout << " (" << mdEvents.size() << " MD events created)\n";
    }
  }

  /* Sort events */
  {
    scoped_wallclock_timer timer("Sort events");
    boost::sort::block_indirect_sort(mdEvents.begin(), mdEvents.end());
  }

  /* Construct box structure */
  MDBox<ND, IntT, MortonT> rootMdBox(mdEvents.cbegin(), mdEvents.cend());
  {
    scoped_wallclock_timer timer("Construct box structure");
    rootMdBox.distributeEvents(FLAGS_split_threshold, FLAGS_max_box_depth);
  }

  /* Save MD events */
  {
    scoped_wallclock_timer timer("Saving MD events");

    using namespace hdf5;

    file::File f =
        file::create("md_events_out.h5", file::AccessFlags::TRUNCATE);
    node::Group rootGroup = f.root();

    dataspace::Simple dataspace{{mdEvents.size()}};
    auto type = datatype::create<std::vector<float>>();

    /* Save signal */
    {
      std::vector<float> signal;
      node::Dataset ds = rootGroup.create_dataset("signal", type, dataspace);
      for (const auto e : mdEvents) {
        signal.push_back(e.signal());
      }
      ds.write(signal);
    }

    /* Save coordinates */
    {
      std::vector<float> qx, qy, qz;
      node::Dataset dsx = rootGroup.create_dataset("qx", type, dataspace);
      node::Dataset dsy = rootGroup.create_dataset("qy", type, dataspace);
      node::Dataset dsz = rootGroup.create_dataset("qz", type, dataspace);
      for (const auto e : mdEvents) {
        const auto coord = e.coordinates(space);
        qx.push_back(coord[0]);
        qy.push_back(coord[1]);
        qz.push_back(coord[2]);
      }
      dsx.write(qx);
      dsy.write(qy);
      dsz.write(qz);
    }
  }
}
