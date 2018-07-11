#include <algorithm>
#include <iostream>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <gflags/gflags.h>

#include "Event.h"
#include "EventNexusLoader.h"

const std::string AllFrames("all");

DEFINE_string(filename, "testdata.nxs", "NeXus file.");
DEFINE_string(datasetName, "raw_data_1/detector_1_events",
              "Name of generated dataset.");
DEFINE_string(frames, AllFrames, "Frames to load.");
DEFINE_bool(print, false, "Print output events");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  EventNexusLoader loader(FLAGS_filename, FLAGS_datasetName);
  std::cerr << "Total events in file: " << loader.totalEventCount() << '\n';

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

  std::cerr << "Frames:";
  for (const auto &frameIdx : frameIdxs) {
    std::cerr << ' ' << frameIdx;
  }
  std::cerr << '\n';

  std::vector<Event> events;
  loader.loadFrames(events, frameIdxs);

  std::cerr << "Loaded " << events.size() << " events total.\n";

  if (FLAGS_print) {
    for (const auto &e : events) {
      std::cout << e.id << '\t' << e.tof << '\t' << e.pulse_time << '\n';
    }
  }
}
