#include <iostream>

#include <gflags/gflags.h>

#include "Instrument.h"

DEFINE_string(filename, "instrument.h5", "Instrument geometry file.");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  Instrument inst;
  load_instrument(inst, FLAGS_filename);

  std::cout << "Source position:\n"
            << inst.source_position << '\n'
            << "Sample position:\n"
            << inst.sample_position << '\n'
            << "Detector count: " << inst.detectors.size() << '\n';

  const std::vector<detid_t> detIds{
      1707000, 1707001, 1707002,
  };
  for (const auto detId : detIds) {
    std::cout << "Detector " << detId << " position:\n"
              << inst.detectors[detId].position << '\n';
  }
}
