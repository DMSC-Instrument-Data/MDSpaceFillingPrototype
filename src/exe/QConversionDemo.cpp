#include <chrono>
#include <iostream>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/sort/sort.hpp>
#include <gflags/gflags.h>
#include <h5cpp/hdf5.hpp>

#include "EventNexusLoader.h"
#include "EventToMDEventConversion.h"
#include "Instrument.h"

const std::string AllFrames("all");

DEFINE_string(instrument, "instrument.h5", "Instrument geometry file.");
DEFINE_string(data, "raw_data.nxs", "TOF event data file.");
DEFINE_string(dataset, "raw_data_1/detector_1_events", "Path to HDF5 dataset.");
DEFINE_string(frames, "0", "Frames to load.");
DEFINE_string(space, "-10,10,-10,10,-10,10", "Q space dimensions.");

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

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  /* Load instrument */
  std::cout << "Load instrument\n";
  Instrument inst;
  load_instrument(inst, FLAGS_instrument);

  EventNexusLoader loader(FLAGS_data, FLAGS_dataset);

  /* Load spectrum to detector mapping */
  std::cout << "Load spectrum to detector mapping\n";
  loader.loadSpectrumDetectorMapping(inst.spectrum_detector_mapping);

  /* Load ToF events */
  std::vector<TofEvent> events;
  {
    std::cout << "Load ToF events\n";

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

  /* Start timing */
  std::cout << "Timing started\n";
  const auto startTime = std::chrono::high_resolution_clock::now();

  /* Convert to Q space */
  std::vector<MDEvent<3, uint16_t, uint64_t>> mdEvents;
  {
    std::cout << "Convert to Q space\n";

    ConversionInfo convInfo{false, Eigen::Matrix3f::Identity()};

    convert_events(mdEvents, events, convInfo, inst, space);
    std::cout << " (" << mdEvents.size() << " MD events created)\n";
  }

  /* Sort events */
  std::cout << "Sort events\n";
  boost::sort::block_indirect_sort(mdEvents.begin(), mdEvents.end());

  /* Stop timing */
  const auto duration = std::chrono::high_resolution_clock::now() - startTime;
  std::cout << mdEvents.size() << " events done in "
            << std::chrono::duration<float>(duration).count() << " seconds\n";

  /* Save MD events */
  {
    std::cout << "Saving MD events\n";

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
