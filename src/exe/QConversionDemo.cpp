#include <iostream>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <gflags/gflags.h>

#include "EventNexusLoader.h"
#include "EventToMDEventConversion.h"
#include "Instrument.h"

const std::string AllFrames("all");

DEFINE_string(instrument, "instrument.h5", "Instrument geometry file.");
DEFINE_string(data, "raw_data.nxs", "TOF event data file.");
DEFINE_string(dataset, "data", "Path to HDF5 dataset.");
DEFINE_string(frames, "1", "Frames to load.");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  /* Load instrument */
  std::cout << "Load instrument\n";
  Instrument inst;
  load_instrument(inst, FLAGS_instrument);

  EventNexusLoader loader(FLAGS_data, FLAGS_dataset);

  /* Load spectrum to detector mapping */
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
        std::vector<std::string> frameIdxStrings;
        boost::algorithm::split(frameIdxStrings, FLAGS_frames,
                                boost::algorithm::is_any_of(","));
        for (const auto &p : frameIdxStrings) {
          frameIdxs.push_back(std::stol(p));
        }
      }
    }

    loader.loadFrames(events, frameIdxs);
    std::cout << " (loaded " << events.size() << " events)\n";
  }

  /* Convert to Q space */
  std::vector<MDEvent<3, uint16_t, uint64_t>> mdEvents;
  {
    std::cout << "Convert to Q space\n";

    ConversionInfo convInfo{false, Eigen::Matrix3f::Identity()};

    MDSpaceBounds<3> space;
    // clang-format off
    space <<
      -10.0f, 10.0f,
      -10.0f, 10.0f,
      -10.0f, 10.0f;
    // clang-format on

    convert_events(mdEvents, events, convInfo, inst, space);
  }

  /* TODO */
}
